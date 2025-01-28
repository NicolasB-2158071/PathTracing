#pragma once

#include <glm/vec3.hpp>
#include "../Utility/Ray.h"
#include "../Sampler/Sampler.h"

class Camera {
public:
	Camera(size_t imageWidth, size_t imageHeight, const glm::vec3& center = glm::vec3{0.0f}, float focalLength = 1.0f);

	glm::vec3 GetViewportPixelCenter(int u, int v);
	glm::vec3 GetCameraCenter() const;

	RTCRayHit CreatePrimaryRay(unsigned int i, unsigned j);

	size_t GetWidth() const;
	size_t GetHeight() const;

private:
	void CalculateViewportLeftPixelCenter(const glm::vec3& viewportU, const glm::vec3& viewportV);

	Sampler m_sampler;
	size_t m_imageWidth;
	size_t m_imageHeight;
	float m_focalLength;
	glm::vec3 m_center;
	glm::vec3 m_pixelDeltaU;
	glm::vec3 m_pixelDeltaV;
	glm::vec3 m_viewportLeftPixelCenter;
};

// Simple camera: https://raytracing.github.io/books/RayTracingInOneWeekend.html#rays,asimplecamera,andbackground