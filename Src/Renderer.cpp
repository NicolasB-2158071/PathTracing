#include "Renderer.h"
#include "spdlog/spdlog.h"
#include <glm/glm.hpp>
#include "Utility/Math.h"

Renderer::Renderer(std::unique_ptr<Integrator>&& integrator, size_t width, size_t height) :
	m_device{ rtcNewDevice(nullptr) },
	m_currentSsp{1},
	m_start{ false },
	m_integrator{std::move(integrator)},
	m_camera{ width, height, m_integrator->GetScene().GetCameraCoordinates() },
	m_framebuffer(width * height * 3, 0.0f) {
	if (!m_device) {
		spdlog::error("Embree: cannot create device {}", static_cast<int>(rtcGetDeviceError(nullptr)));
	}
	rtcSetDeviceErrorFunction(m_device, nullptr, nullptr);
	m_integrator->InitRTCScene(m_device);
}

Renderer::~Renderer() {
	rtcReleaseDevice(m_device);
}

void Renderer::ErrorFunction(void* userPtr, enum RTCError error, const char* str) const {
	spdlog::error("Embree: {} {}", static_cast<int>(error), str);
}

void Renderer::AddRadiance(unsigned short i, unsigned short j, const glm::vec3& rgb) {
	size_t idx{ 3 * j + 3 * m_camera.GetWidth() * i };
	for (int k = 0; k < 3; ++k) {
		m_framebuffer[idx + k] += rgb[k];
	}
}

void Renderer::Render(int spp, bool withGUI, const std::string& savePath) {
	m_savePath = savePath;
	if (withGUI) {
		GUI gui{ m_camera.GetWidth(), m_camera.GetHeight(), *this};
		while (gui.isActive()) {
			if (m_start && m_currentSsp < spp) {
				++m_currentSsp;
				#pragma omp parallel for collapse(2)
				for (int j = 0; j < m_camera.GetHeight(); ++j) {
					for (int i = 0; i < m_camera.GetWidth(); ++i) {
						RTCRayHit rayHit{ m_camera.CreatePrimaryRay(i, j) };
						AddRadiance(j, i, m_integrator->EvaluateSample(rayHit, m_integrator->GetMaxDepth()));
					}
				}
			}
			gui.Draw();
		}
	} else {
		while (m_currentSsp < spp) {
			for (int k = m_currentSsp; k < m_currentSsp * 2; ++k) {
				#pragma omp parallel for collapse(2)
				for (int j = 0; j < m_camera.GetHeight(); ++j) {
					for (int i = 0; i < m_camera.GetWidth(); ++i) {
						RTCRayHit rayHit{ m_camera.CreatePrimaryRay(i, j) };
						AddRadiance(j, i, m_integrator->EvaluateSample(rayHit, m_integrator->GetMaxDepth()));
					}
				}
			}
			m_currentSsp *= 2;
			spdlog::trace("Renderer: rendered with spp = {}", m_currentSsp);
		}
		float gamma{ 1.0f / 2.2f };
		std::transform(m_framebuffer.begin(), m_framebuffer.end(), m_framebuffer.begin(), [this, gamma, spp](auto val) {return glm::pow(val / spp, gamma); });
		m_tonemap.Map(m_framebuffer);
		Image image{ static_cast<unsigned short>(m_camera.GetWidth()), static_cast<unsigned short>(m_camera.GetHeight()) };
		image.Save(m_savePath, &m_framebuffer);
	}
}