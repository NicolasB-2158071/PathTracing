#pragma once

#include <embree4/rtcore.h>
#include "Scene/Scene.h"
#include "Image/Image.h"
#include "Integrator/Integrator.h"
#include "GUI/GUI.h"
#include "Camera/Camera.h"
#include "PostProcessing/ToneMapTypes/ClampToneMap.h"
#include "PostProcessing/ToneMapTypes/ReinhardToneMap.h"

class Renderer {
public:
	Renderer(std::unique_ptr<Integrator>&& integrator, size_t width = 800, size_t height = 800);
	~Renderer();
	friend class GUI;

	void Render(int spp, bool withGUI, const std::string& savePath);

private:
	RTCDevice m_device;
	void ErrorFunction(void* userPtr, enum RTCError error, const char* str) const;
	int m_currentSsp;
	bool m_start;
	std::string m_savePath;
	std::vector<float> m_framebuffer;
	std::unique_ptr<Integrator> m_integrator;
	Camera m_camera;
	ReinhardToneMap m_tonemap;

	void AddRadiance(unsigned short i, unsigned short j, const glm::vec3& rgb);
};