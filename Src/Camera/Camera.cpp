#include "Camera.h"

Camera::Camera(
	size_t imageWidth,
	size_t imageHeight,
	const glm::vec3& center,
	float focalLength
) : m_focalLength{ focalLength }, m_center{ center }, m_sampler{}, m_imageWidth{ imageWidth }, m_imageHeight{ imageHeight } {
	float viewportHeight = 2.0f;
	float viewportWidth = viewportHeight * (static_cast<float>(imageWidth / imageHeight));

	glm::vec3 viewportU{ viewportWidth, 0, 0 };
	glm::vec3 viewportV{ 0, -viewportHeight, 0 };
	m_pixelDeltaU = viewportU / static_cast<float>(imageWidth);
	m_pixelDeltaV = viewportV / static_cast<float>(imageHeight);
	CalculateViewportLeftPixelCenter(viewportU, viewportV);
}

glm::vec3 Camera::GetViewportPixelCenter(int u, int v) {
	glm::vec2 offset{ m_sampler.GetUV1D() - 0.5f, m_sampler.GetUV1D() - 0.5f};
	return m_viewportLeftPixelCenter + ((static_cast<float>(u) + offset.x) * m_pixelDeltaU) + ((static_cast<float>(v) + offset.y) * m_pixelDeltaV);
}

glm::vec3 Camera::GetCameraCenter() const {
	return m_center;
}

RTCRayHit Camera::CreatePrimaryRay(unsigned int i, unsigned j) {
	glm::vec3 pixelCenter{ GetViewportPixelCenter(i, j) };
	glm::vec3 cameraCenter{ GetCameraCenter() };
	glm::vec3 rayDirection{ glm::normalize(pixelCenter - cameraCenter) };

	return CreateRayHit(cameraCenter, rayDirection);
}

size_t Camera::GetWidth() const {
	return m_imageWidth;
}

size_t Camera::GetHeight() const {
	return m_imageHeight;
}

void Camera::CalculateViewportLeftPixelCenter(const glm::vec3& viewportU, const glm::vec3& viewportV) {
	glm::vec3 viewportLeft{
		m_center - glm::vec3{0, 0, m_focalLength} -
		(viewportU / 2.0f) -
		(viewportV / 2.0f)
	};
	m_viewportLeftPixelCenter = viewportLeft + static_cast<float>(0.5) * (m_pixelDeltaU + m_pixelDeltaV);
}
