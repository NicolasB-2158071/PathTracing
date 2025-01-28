#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <json/json.h>

struct SurfaceIntersection;

enum class BxDFType {
	DIFFUSE,
	SPECULAR,
	GLOSSY,
	TRANSMISSION,
	GLOSSYTRANSMISSION
};

struct BxDFSample {
	glm::vec3 f;
	glm::vec3 wi;
	float pdf;
	BxDFType type;
};

class BxDF {
public:
	virtual glm::vec3 F(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const = 0;
	virtual BxDFSample SampleF(float u, const glm::vec2& uv, const glm::vec3& wo, const SurfaceIntersection& si) const = 0;

	static std::unique_ptr<BxDF> CreateFromMaterial(const Json::Value& mat);
};