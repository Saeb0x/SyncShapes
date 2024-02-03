#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <opencv2/opencv.hpp>

#include <iostream>

#include "OpenCVImageProcessor/ImageProcessor.h"

namespace SyncShapes
{
	class ImGuiManager
	{
	public:
		ImGuiManager(GLFWwindow* window);
		~ImGuiManager();

		void Update();
		void Render();

		inline void Log(const char* message) { m_LogBuffer.push_back(message); }

		template <typename... Args>
		inline void Log(Args... args) {
			std::string logMessage;

			// + operator to concatenate arguments into the logMessage string
			((logMessage += args), ...);

			m_LogBuffer.push_back(logMessage.c_str());
		}

	private:
		GLFWwindow* m_Window;
		GLuint m_TextureID; GLuint m_TextureID2;
		std::string m_ImagePath;
		std::vector<std::string> m_LogBuffer;

		void ShowImageProcessingEditor();
		void ShowViewport();
		void ShowLogger();
	};
}


