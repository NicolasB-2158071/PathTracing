#pragma once

#include <vector>
#include <string>
#include <memory>
#include <embree4/rtcore.h>
#include <glm/glm.hpp>
#include "Bxdf/Bxdf.h"
#include "../Light/AreaLight.h"

struct Shape {
	std::string name;
	std::vector<float> vertices; // Unpacked, duplicate free
	std::vector<float> normals;
	std::vector<unsigned> indices;
	std::unique_ptr<BxDF> bxdf;
	RTCGeometry rtcGeometry = nullptr;
	AreaLight* areaLight = nullptr;
	float area;

	Shape() = default;
	Shape(const Shape& shape) = default;
	Shape(Shape&& shape) = default;

	~Shape() {
		rtcReleaseGeometry(rtcGeometry);
	}

	size_t GetNumVertices() const {
		return indices.size();
	}

	size_t GetNumFaces(int vertexFace = 3) const {
		return GetNumVertices() / vertexFace;
	}

	bool IsVertexEqual(int i, const glm::vec3& vertex) const {
		return vertices[i] == vertex.x && vertices[i + 1] == vertex.y && vertices[i + 2] == vertex.z;
	}

	bool IsNormalEqual(int i, const glm::vec3& normal) const {
		return normals[i] == normal.x && normals[i + 1] == normal.y && normals[i + 2] == normal.z;
	}

	int GetVertexID(const glm::vec3& vertex, const glm::vec3& normal) const {
		for (int i = 0; i < vertices.size(); i += 3) {
			if (IsVertexEqual(i, vertex) && IsNormalEqual(i, normal)) {
				return i / 3;
			}
		}
		return -1;
	}

	void AddVertex(const glm::vec3& vertex, const glm::vec3& normal) {
		vertices.push_back(vertex.x);
		vertices.push_back(vertex.y);
		vertices.push_back(vertex.z);
		normals.push_back(normal.x);
		normals.push_back(normal.y);
		normals.push_back(normal.z);
		indices.push_back((vertices.size() / 3) - 1);
	}

	glm::vec3 GetVertex(int faceID, int vertexID) const {
		return glm::vec3{ vertices[indices[faceID * 3 + vertexID] * 3], vertices[indices[faceID * 3 + vertexID] * 3 + 1], vertices[indices[faceID * 3 + vertexID] * 3 + 2] };
	}

	std::vector<glm::vec3> GetFace(int faceID) const {
		return { GetVertex(faceID, 0), GetVertex(faceID, 1), GetVertex(faceID, 2) };
	}

	glm::vec3 SampleUniform(int faceID, const glm::vec2& u) const {
		float alpha{ 1 - std::sqrt(u.x) };
		float beta{ (1 - u.y) * std::sqrt(u.x) };
		float gamma{ u.y * std::sqrt(u.x) };
		return alpha * GetVertex(faceID, 0) + beta * GetVertex(faceID, 1) + gamma * GetVertex(faceID, 2);
	}

	glm::vec2 GetBarycentric(const glm::vec3& p, int faceID) const {
		glm::vec3 a{ GetVertex(faceID, 0) };
		glm::vec3 v0{ GetVertex(faceID, 1) - a }, v1{ GetVertex(faceID, 2) - a }, v2{ p - a };
		float d00{ glm::dot(v0, v0) }, d01{ glm::dot(v0, v1) }, d11{ glm::dot(v1, v1) }, d20{ glm::dot(v2, v0) }, d21{ glm::dot(v2, v1) };
		float invDenom{ 1.0f / (d00 * d11 - d01 * d01) };
		return glm::vec2{ (d11 * d20 - d01 * d21) * invDenom, (d00 * d21 - d01 * d20) * invDenom };
	}

	float GetArea() const {
		return area;
	}

	float Area() const {
		float area{};
		for (int i = 0; i < GetNumFaces(); ++i) {
			std::vector<glm::vec3> face{ GetFace(i) };
			area += 0.5f * glm::length(glm::cross(face[1] - face[0], face[2] - face[0]));
		}
		return area;
	}
};