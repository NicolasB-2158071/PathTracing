#include "GUI.h"
#include "spdlog/spdlog.h"
#include "../../Lib/ImGUI/imgui_impl_glfw.h"
#include "../../Lib/ImGUI/imgui_impl_opengl3.h"
#include "../../Lib/ImGUI/imgui.h"
#include "../Renderer.h"
#include "../Image/Image.h"

static void glfw_error_callback(int error, const char* description) {
	spdlog::error("GLFW: {} {}", error, description);
}

GUI::GUI(size_t width, size_t height, Renderer& renderer) : m_width{ width }, m_height{ height }, m_renderer{ renderer }, m_framebufferB(m_width * m_height * 3, 0.0f) {
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) {
		spdlog::error("Failed to initialize GLFW");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	
	m_window = glfwCreateWindow(m_width, m_height, "Path tracing", nullptr, nullptr);
	if (m_window == nullptr) {
		spdlog::error("Can't initialize window");
	}
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		spdlog::error("Failed to initialize GLAD");
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();


	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
}

GUI::~GUI() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool GUI::isActive() const {
	return !glfwWindowShouldClose(m_window);
}

void GUI::Draw() {
	glfwPollEvents();

	if (m_renderer.m_start) {
		float gamma{ 1.0f / 2.2f };
		std::transform(m_renderer.m_framebuffer.begin(), m_renderer.m_framebuffer.end(), m_framebufferB.begin(), [this, gamma](auto val) {return glm::pow(val / m_renderer.m_currentSsp, gamma); });
		//std::transform(m_framebuffer.begin(), m_framebuffer.end(), m_framebufferB.begin(), [this](auto val) {return val / m_renderer.m_currentSsp; });
		m_renderer.m_tonemap.Map(m_framebufferB);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, m_framebufferB.data());
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	Show();

	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(m_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(m_window);
}

void GUI::Show() {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin("Rendering");
	ImVec2 window_size = ImGui::GetContentRegionAvail();
	ImGui::Image((ImTextureID)(intptr_t)m_texture, window_size);
	ImGui::End();

	ImGui::Begin("Options");
	float width{ ImGui::GetContentRegionAvail().x };
	ImGui::Text("Samples per pixel = %d", m_renderer.m_currentSsp);
	if (ImGui::Button("Save", ImVec2(width, 0.0f))) {
		Image image{static_cast<unsigned short>(m_width), static_cast<unsigned short>(m_height)};
		image.Save(m_renderer.m_savePath, &m_framebufferB);
	}
	if (ImGui::Button("Start", ImVec2(width, 0.0f))) {
		m_renderer.m_start = true;
	}
	if (ImGui::Button("Stop", ImVec2(width, 0.0f))) {
		m_renderer.m_start = false;
	}
	// TODO: camera coordinates
	ImGui::End();

}
