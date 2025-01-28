#include "Scene.h"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include "spdlog/spdlog.h"
#include "../Exception/ParseException.h"
#include "../Utility/SmoothingNormals.h"
#include "Light/AreaLight.h"
#include "Light/PointLight.h"
#include "Shape/SurfaceIntersection.h"


Scene::Scene() : m_name{}, m_cameraCoordinates{} {}

Scene::Scene(const std::string& geometryFile, const std::string& materialFile) : m_name{ geometryFile }, m_cameraCoordinates{} {
    LoadScene(geometryFile, materialFile);
}

void Scene::LoadScene(const std::string& geometryFile, const std::string& materialFile) {
    try {
        tinyobj::ObjReader reader{ CreateReader(geometryFile) };
        auto& attrib{ reader.GetAttrib() };
        auto& shapes{ reader.GetShapes() };
        std::ifstream materialsFile(materialFile, std::ifstream::binary);
        Json::Value materials;
        materialsFile >> materials;

        // Worked for a certain scene but for others not, so obj requires normals
        //tinyobj::attrib_t outattrib;
        //std::vector<tinyobj::shape_t> outshapes;
        //computeSmoothingShapes(attrib, shapes, outshapes, outattrib);
        //computeAllSmoothingNormals(outattrib, outshapes);
        //auto& att = outattrib;
        //auto& sha = outshapes;
        auto& att = attrib;
        auto& sha = shapes;

        for (size_t s = 0; s < sha.size(); ++s) {
            spdlog::trace("Scene: found shape '{}'", sha[s].name);
            m_shapes.push_back(ReadShape(sha[s], att, materials[sha[s].name]));
        }
        ReadLights(materials);
        if (materials.isMember("camera")) {
            auto& camera{ materials["camera"] };
            m_cameraCoordinates = glm::vec3{ camera["coordinates"][0].asFloat(), camera["coordinates"][1].asFloat(), camera["coordinates"][2].asFloat()};
            spdlog::trace("Scene: found camera {} {} {}", m_cameraCoordinates.x, m_cameraCoordinates.y, m_cameraCoordinates.z);
        }
    } catch (ParseException e) {
        spdlog::error("TinyObjReader: {}", e.what());
    }
}

tinyobj::ObjReader Scene::CreateReader(const std::string& inputFile) const {
    tinyobj::ObjReaderConfig readerConfig;
    readerConfig.mtl_search_path = "";
    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(inputFile, readerConfig)) {
        if (!reader.Error().empty()) {
            throw ParseException{reader.Error().c_str()};
        }
    }
    return reader;
}

Shape Scene::ReadShape(const tinyobj::shape_t& shape, const tinyobj::attrib_t& att, const Json::Value& mat) const {
    Shape s{};
    s.name = shape.name;
    size_t indexOffset{ 0 };
    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
        size_t fv{ size_t(shape.mesh.num_face_vertices[f]) };
        for (size_t v = 0; v < fv; ++v) {
            tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];
            glm::vec3 vertex{
                att.vertices[3 * size_t(idx.vertex_index) + 0],
                att.vertices[3 * size_t(idx.vertex_index) + 1],
                att.vertices[3 * size_t(idx.vertex_index) + 2]
            };
            glm::vec3 normal{
                att.normals[3 * size_t(idx.normal_index) + 0],
                att.normals[3 * size_t(idx.normal_index) + 1],
                att.normals[3 * size_t(idx.normal_index) + 2]
            };
            //int vertexID{ s.GetVertexID(vertex, normal) }; // To slow!!!
            //if (vertexID == -1) {
                s.AddVertex(vertex, normal);
            /*} else {
                s.indices.push_back(vertexID);
            }*/
        }
        indexOffset += fv;
    }
    s.area = s.Area();
    if (mat) {
        s.bxdf = BxDF::CreateFromMaterial(mat);
    }
    return s;
}

void Scene::ReadLights(const Json::Value& mat) {
    for (const auto& shapeName : mat["areaLights"].getMemberNames()) {
        auto& areaLight{ mat["areaLights"][shapeName] };
        auto shape{ std::find_if(m_shapes.begin(), m_shapes.end(), [&shapeName](const Shape& x) { return x.name == shapeName; }) };
        if (shape == m_shapes.end()) {
            spdlog::error("Scene: found area light corresponding to unkown shape '{}'", shapeName);
            continue;
        }
        spdlog::trace("Scene: identified shape '{}' as an area light", shape->name);
        m_lights.push_back(std::make_unique<AreaLight>(*shape, 
            glm::vec3{areaLight["radiance"][0].asFloat(), areaLight["radiance"][1].asFloat(), areaLight["radiance"][2].asFloat()}, areaLight["twoSided"].asBool()
        ));
        shape->areaLight = static_cast<AreaLight*>(m_lights[m_lights.size() - 1].get());
    }
    for (auto& pointLight : mat["pointLights"]) {
        spdlog::trace("Scene: Found point light '{}'", pointLight["name"].asString());
        m_lights.push_back(std::make_unique<PointLight>(
            glm::vec3{ pointLight["pos"][0].asFloat(), pointLight["pos"][1].asFloat(), pointLight["pos"][2].asFloat() },
            glm::vec3{ pointLight["radiance"][0].asFloat(), pointLight["radiance"][1].asFloat(), pointLight["radiance"][2].asFloat() }
        ));
    }
}

RTCScene Scene::Build(RTCDevice device) {
	if (m_name.empty()) {
		spdlog::error("Scene: no scene loaded");
		return RTCScene{};
	}
    RTCScene scene{ rtcNewScene(device) };
    for (auto& shape : m_shapes) {
        RTCGeometry geom{ rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE) };
        rtcSetGeometryVertexAttributeCount(geom, 1);

        float* vertices{
            static_cast<float*>(rtcSetNewGeometryBuffer(
                geom,
                RTC_BUFFER_TYPE_VERTEX,
                0,
                RTC_FORMAT_FLOAT3,
                3 * sizeof(float),
                shape.vertices.size() / 3
            )) 
        };
        for (int i = 0; i < shape.vertices.size(); ++i) {
            vertices[i] = shape.vertices[i];
        }

        float* normals{
            static_cast<float*>(rtcSetNewGeometryBuffer(
                geom,
                RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
                0,
                RTC_FORMAT_FLOAT3,
                3 * sizeof(float),
                shape.normals.size() / 3
            ))
        };
        for (int i = 0; i < shape.normals.size(); ++i) {
            normals[i] = shape.normals[i];
        }

        unsigned* indices{
            static_cast<unsigned*>(rtcSetNewGeometryBuffer(
                geom,
                RTC_BUFFER_TYPE_INDEX,
                0,
                RTC_FORMAT_UINT3,
                3 * sizeof(unsigned),
                shape.GetNumFaces()
            ))
        };
        for (int i = 0; i < shape.indices.size(); ++i) {
            indices[i] = shape.indices[i];
        }
        
        rtcCommitGeometry(geom);
        rtcAttachGeometry(scene, geom); // geomId is sequential
        shape.rtcGeometry = geom;
    }

    rtcCommitScene(scene);
	return scene;
}

glm::vec3 Scene::GetCameraCoordinates() const {
    return m_cameraCoordinates;
}

SurfaceIntersection Scene::GetIntersection(RTCScene rtcScene, RTCRayHit& rayHit) const {
    SurfaceIntersection si{};
    rtcIntersect1(rtcScene, &rayHit);
    if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
        return si;
    }

    glm::vec3 dir{ rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z };

    si.shape = &m_shapes[rayHit.hit.geomID];
    si.pos = glm::vec3{glm::vec3{ rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z} + dir * rayHit.ray.tfar };
    rtcInterpolate1(rtcGetGeometry(rtcScene, rayHit.hit.geomID), rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &si.normal.x, nullptr, nullptr, 3);
    si.normal = glm::normalize(si.normal);
    return si;
}

const std::unique_ptr<Light>& Scene::GetLight(int ID) const {
    return m_lights[ID];
}

const std::vector<std::unique_ptr<Light>>& Scene::GetLights() const {
    return m_lights;
}