#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ImGuiManager/ImGuiManager.h"

#include <iostream>
#include <memory>

namespace SyncShapes 
{
	class Window
	{
	public:
		Window(int width, int height, const char* title);
		~Window();

		void Run();
		inline GLFWwindow* GetWindow() const { return m_Window; }
	private:
		GLFWwindow* m_Window;
		int m_Width, m_Height;
		const char* m_Title;
		std::unique_ptr<ImGuiManager> m_ImGuiManager;

		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	};
}

