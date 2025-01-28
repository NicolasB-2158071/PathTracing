#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>



class Renderer;

class GUI {
public:
	GUI(size_t width, size_t height, Renderer& renderer);
	~GUI();

	bool isActive() const;
	void Draw();

private:
	GLFWwindow* m_window;
	size_t m_width;
	size_t m_height;
	std::vector<float> m_framebufferB;
	GLuint m_texture;
	Renderer& m_renderer;

	void Show();
};