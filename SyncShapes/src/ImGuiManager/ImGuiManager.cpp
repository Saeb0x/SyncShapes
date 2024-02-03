#include <windows.h>  // Windows header for API functions
#include <commdlg.h>  // Windows file dialog header
#include <chrono>

#include "ImGuiManager.h"

namespace SyncShapes
{
	ImGuiManager::ImGuiManager(GLFWwindow* window) : m_Window(window), m_ImagePath(""), m_TextureID(0), m_TextureID2(0)
	{
		// Initialize ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Enable docking
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// Initialize ImGui for GLFW and OpenGL
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330 core");

		std::cout << "ImGuiManager initialized" << std::endl;
	}

	ImGuiManager::~ImGuiManager() {
		// Cleanup ImGui
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		std::cout << "ImGuiManager destroyed" << std::endl;
	}

	void ImGuiManager::Update() {
		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

		// Updates

	}

	void ImGuiManager::Render() {
		ShowImageProcessingEditor();
		ShowViewport();
		ShowLogger();

		// End ImGui frame
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiManager::ShowImageProcessingEditor() {
		ImGui::Begin("Editor");

		ImGui::TextWrapped("Welcome to SyncShapes Content-Based Image Retrieval (CBIR) System!");
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
		ImGui::TextWrapped("A lightweight yet powerful content-based image retrieval system. It focuses on efficient shape synchronization, allowing users to retrieve images based on their shape features.");

		ImGui::Separator(); ImGui::Spacing();
		ImGui::TextWrapped("Query An Image:");

		// File Dialog for Image Loading
		if (ImGui::Button("Load Image From Dataset")) {
			OPENFILENAMEW ofn;  // Wide character version
			wchar_t szFile[260]; // Buffer to store wide file path
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = szFile;  // Wide character buffer
			ofn.lpstrFile[0] = L'\0'; // Wide character constant
			ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
			ofn.lpstrFilter = L"Image Files\0*.bmp;*.jpg;*.jpeg;*.png;*.gif\0All Files\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			// Display the file dialog
			if (GetOpenFileNameW(&ofn) == TRUE) {
				std::wstring wideImagePath = ofn.lpstrFile;
				std::string imagePath(wideImagePath.begin(), wideImagePath.end()); m_ImagePath = imagePath;
			}

			std::string WorkingDirectoryPath = fs::path(m_ImagePath).parent_path().string();
			Log("Loaded Image Directory Path: ", WorkingDirectoryPath);

			auto imagesConversionStartTime = std::chrono::high_resolution_clock::now();
			ImageProcessor::ConvertAllGIFsToJPEGs(m_ImagePath);
			auto imagesConversionEndTime = std::chrono::high_resolution_clock::now();
			auto imagesConversionDuration = std::chrono::duration_cast<std::chrono::seconds>(imagesConversionEndTime - imagesConversionStartTime);
			Log("Pre-Processing: converting GIFs to JPEGs completed. Time: " + std::to_string(imagesConversionDuration.count()) + " s");
		}

		ImGui::Separator(); ImGui::Spacing();
		// Pre-processing Options
		ImGui::TextWrapped("Pre-processing Options:");

		// Checkbox for Noise Removal
		static bool applyNoiseRemoval = false;
		ImGui::Checkbox("Apply Noise Removal", &applyNoiseRemoval);

		// Checkbox for Hole Filling
		static bool applyHoleFilling = false;
		ImGui::Checkbox("Apply Hole Filling", &applyHoleFilling);

		// Checkbox for Histogram Equalization
		static bool applyHistogramEqualization = false;
		ImGui::Checkbox("Apply Histogram Equalization", &applyHistogramEqualization);

		// Preprocessing button
		if (ImGui::Button("Apply Preprocessing")) {
			if (!m_ImagePath.empty()) {
				if (!(applyNoiseRemoval || applyHoleFilling || applyHistogramEqualization)) {
					Log("No preprocessing option selected. Please choose at least one option.");
				}
				else {
					if (applyNoiseRemoval && applyHoleFilling && applyHistogramEqualization) {
						auto bothOperationsStartTime = std::chrono::high_resolution_clock::now();
						ImageProcessor::ApplyNoiseRemovalAndHoleFillingAndHistogramEqualizationToDirectory(ImageProcessor::m_PreprocessingDir);
						auto bothOperationsEndTime = std::chrono::high_resolution_clock::now();
						auto bothOperationsDuration = std::chrono::duration_cast<std::chrono::seconds>(bothOperationsEndTime - bothOperationsStartTime);
						Log("Pre-Processing: Noise Removal, Hole Filling, and Histogram Equalization completed. Time: " + std::to_string(bothOperationsDuration.count()) + " s");
					}
					else {
						int selectedOptionsCount = (applyNoiseRemoval ? 1 : 0) + (applyHoleFilling ? 1 : 0) + (applyHistogramEqualization ? 1 : 0);

						if (selectedOptionsCount == 2) {
							Log("Preprocessing allows for individual operation testing or all applied together for the retrieval system main aim or output");
						}
						else {
							if (applyNoiseRemoval) {
								auto noiseRemovalStartTime = std::chrono::high_resolution_clock::now();
								ImageProcessor::ApplyNoiseRemovalToDirectory(ImageProcessor::m_PreprocessingDir);
								auto noiseRemovalEndTime = std::chrono::high_resolution_clock::now();
								auto noiseRemovalDuration = std::chrono::duration_cast<std::chrono::seconds>(noiseRemovalEndTime - noiseRemovalStartTime);
								Log("Pre-Processing: Noise Removal completed. Time: " + std::to_string(noiseRemovalDuration.count()) + " s");
							}

							if (applyHoleFilling) {
								auto holeFillingStartTime = std::chrono::high_resolution_clock::now();
								ImageProcessor::ApplyHoleFillingToDirectory(ImageProcessor::m_PreprocessingDir);
								auto holeFillingEndTime = std::chrono::high_resolution_clock::now();
								auto holeFillingDuration = std::chrono::duration_cast<std::chrono::seconds>(holeFillingEndTime - holeFillingStartTime);
								Log("Pre-Processing: Hole Filling completed. Time: " + std::to_string(holeFillingDuration.count()) + " s");
							}

							if (applyHistogramEqualization) {
								auto histogramEqualizationStartTime = std::chrono::high_resolution_clock::now();
								ImageProcessor::ApplyHistogramEqualizationToDirectory(ImageProcessor::m_PreprocessingDir);
								auto histogramEqualizationEndTime = std::chrono::high_resolution_clock::now();
								auto histogramEqualizationDuration = std::chrono::duration_cast<std::chrono::seconds>(histogramEqualizationEndTime - histogramEqualizationStartTime);
								Log("Pre-Processing: Histogram Equalization completed. Time: " + std::to_string(histogramEqualizationDuration.count()) + " s");
							}
						}
					}
				}
			}
			else {
				Log("Cannot apply preprocessing. Please load an image from the dataset first.");
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("?", ImVec2(20, 20))) {
			ImGui::OpenPopup("Preprocessing Help");
		}

		// Tooltip with instructions for preprocessing options
		if (ImGui::BeginPopup("Preprocessing Help")) {
			ImGui::Text("Preprocessing Options:");
			ImGui::Text("1. Apply Noise Removal: Remove noise from the image using Gaussian blur.");
			ImGui::Text("2. Apply Hole Filling: Fill small holes in the image using morphological operations.");
			ImGui::Text("3. Apply Histogram Equalization: Enhance contrast using histogram equalization.");

			ImGui::EndPopup();
		}

		ImGui::Separator(); ImGui::Spacing();
		ImGui::TextWrapped("Feature Extraction:"); 

		ImGui::Separator(); ImGui::Spacing();
		if (ImGui::Button("How To Use?", ImVec2(100, 40))) {
			ImGui::OpenPopup("Instructions");
		}

		if (ImGui::BeginPopup("Instructions")) {
			ImGui::Text("How To Use SyncShapes?");
			ImGui::Separator();

			// Editor Instructions
			ImGui::Text("\nEditor:");
			ImGui::Text("1. The editor serves as the control center for processing images.");

			// Viewport Instructions
			ImGui::Text("\nViewport:");
			ImGui::Text("1. The 'Viewport' displays the preprocessed/processed images.");
			ImGui::Text("2. Use the 'Show Image' button to view your results.");
			ImGui::Text("3. Clear the viewport with the 'Clear Viewport' button.");

			// Log Area Instructions
			ImGui::Text("\nLog Area:");
			ImGui::Text("1. The 'Log' area logs important messages and actions.");
			ImGui::Text("2. Clear the log with the 'Clear Log' button.");
			ImGui::Text("3. Execution time is printed for each operation in the log area.");

			// Preprocessing Options Instructions
			ImGui::Text("\nPreprocessing:");
			ImGui::Text("1. Preprocessing is the first stage of multiple stages for content-based image retrieval.");
			ImGui::Text("2. Select from various options like Noise Removal, Hole Filling, etc.");

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void ImGuiManager::ShowViewport() {
		ImGui::Begin("Viewport");

		ImVec2 viewportSize(500, 500);

		// Display the image texture in the ImGui window
		ImGui::Image((void*)(intptr_t)m_TextureID, viewportSize);

		if (ImGui::Button("Show Image"))
		{
			OPENFILENAMEW ofn;  // Wide character version
			wchar_t szFile[260]; // Buffer to store wide file path
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = szFile;  // Wide character buffer
			ofn.lpstrFile[0] = L'\0'; // Wide character constant
			ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
			ofn.lpstrFilter = L"Image Files\0*.bmp;*.jpg;*.jpeg;*.png;*.gif\0All Files\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			// Display the file dialog
			if (GetOpenFileNameW(&ofn) == TRUE)
			{
				std::wstring wideImagePath = ofn.lpstrFile;
				std::string imagePath(wideImagePath.begin(), wideImagePath.end()); m_ImagePath = imagePath;

				m_TextureID = ImageProcessor::GetTexture(m_ImagePath);
				
				std::string imageName = fs::path(m_ImagePath).filename().string();
				ImageDetails details = ImageProcessor::GetImageDetails(m_ImagePath);
				Log("Selected Image: " + imageName + ". Type: " + details.type + ". Size = " + std::to_string(details.width) + "*" + std::to_string(details.height));
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Clear Viewport")) {
			if (m_TextureID == 0)
			{
				Log("Viewport is already cleared");
			}
			else
			{
				m_TextureID = 0; // 0 is an invalid texture ID, which causes to clear the viewport
				Log("Viewport cleared");
			}
		}
		
		ImGui::End();
	}

	void ImGuiManager::ShowLogger()
	{
		ImGui::Begin("Log");

		// Counter variable for numbering log messages
		int logCounter = 1;

		for (const auto& logMessage : m_LogBuffer) {
			ImGui::TextWrapped("[%d] %s", logCounter++, logMessage.c_str());
		}

		if (!m_LogBuffer.empty())
		{
			ImGui::NewLine();
			if (ImGui::Button("Clear Log", ImVec2(100, 25))) {
				m_LogBuffer.clear();
			}
		}

		ImGui::End();
	}
}