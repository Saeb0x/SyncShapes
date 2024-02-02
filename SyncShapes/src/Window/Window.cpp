#include "Window.h"

namespace SyncShapes
{
	Window::Window(int width, int height, const char* title) : m_Width(width), m_Height(height), m_Title(title)
	{
		// Initialize GLFW
		if (!glfwInit())
		{
			std::cerr << "Failed to initialize GLFW" << std::endl;
			std::exit(EXIT_FAILURE);
		}

		// OpenGL context with version 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
		if (!m_Window)
		{
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			std::exit(EXIT_FAILURE);
		}

		// Enable vsync
		glfwSwapInterval(1);

		glfwMakeContextCurrent(m_Window);

		// Initialize GLEW
		if (glewInit() != GLEW_OK) {
			// Handle GLEW initialization failure
			glfwTerminate();
			std::exit(EXIT_FAILURE);
		}

		// Initialize ImGuiManager
		m_ImGuiManager = std::make_unique<ImGuiManager>(m_Window);

		// GLFW callback functions
		glfwSetKeyCallback(m_Window, KeyCallback);
		glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);

		std::cout << "Window initialized" << std::endl;
	}

	Window::~Window()
	{
		// Cleanup GLFW
		glfwDestroyWindow(m_Window);
		glfwTerminate();

		std::cout << "Window destroyed" << std::endl;
	}

	void Window::Run()
	{
		while (!glfwWindowShouldClose(m_Window))
		{
			// Poll for and process events
			glfwPollEvents();

			glClear(GL_COLOR_BUFFER_BIT);

			// Update ImGui
			m_ImGuiManager->Update();

			// Render ImGui over the scene
			m_ImGuiManager->Render();

			// Swap front and back buffers
			glfwSwapBuffers(m_Window);
		}
	}

	void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}

	void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	}
}