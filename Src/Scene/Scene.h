#pragma once

#include <embree4/rtcore.h>
#include "tiny_obj_loader.h"
#include <string>
#include <memory>
#include "Shape/Shape.h"
#include "Light/Light.h"
#include <json/json.h>

class Scene {
public:
	Scene();
	Scene(const std::string& geometryFile, const std::string& materialFile);

	void LoadScene(const std::string& geometryFile, const std::string& materialFile);
	RTCScene Build(RTCDevice device);

	glm::vec3 GetCameraCoordinates() const;
	SurfaceIntersection GetIntersection(RTCScene scene, RTCRayHit& ray) const;

	const std::unique_ptr<Light>& GetLight(int ID) const;
	const std::vector<std::unique_ptr<Light>>& GetLights() const;

private:
	std::string m_name;
	std::vector<Shape> m_shapes;
	std::vector<std::unique_ptr<Light>> m_lights;
	glm::vec3 m_cameraCoordinates;

	tinyobj::ObjReader CreateReader(const std::string& inputFile) const;
	Shape ReadShape(const tinyobj::shape_t& shape, const tinyobj::attrib_t& att, const Json::Value& mat) const;
	void ReadLights(const Json::Value& mat);
};