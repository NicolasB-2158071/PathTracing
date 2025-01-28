#pragma once

#include <embree4/rtcore.h>
#include <glm/glm.hpp>
#include "../Scene/Scene.h"
#include "../Sampler/Sampler.h"
#include "../Sampler/LightSampler.h"

class Integrator {
public:
	Integrator(Scene& scene, int maxDepth);
	~Integrator();
	
	void InitRTCScene(RTCDevice device);
	const Scene& GetScene() const;

	virtual glm::vec3 EvaluateSample(RTCRayHit& ray, int depth) = 0;
	int GetMaxDepth() const;

	static std::unique_ptr<Integrator> CreateFromName(const std::string& name, Scene& scene, int maxDepth);

protected:
	RTCScene m_rtcScene;
	Scene& m_scene;
	Sampler m_sampler;
	LightSampler m_lightSampler;
	int m_maxDepth;

	bool V(const glm::vec3& x, const glm::vec3& y) const;
	float G(const glm::vec3& x, const glm::vec3& nx, const glm::vec3& y, const glm::vec3& ny) const;
};