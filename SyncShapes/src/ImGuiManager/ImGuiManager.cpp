#include <windows.h>  // Windows header for API functions
#include <commdlg.h>  // Windows file dialog header
#include "ImGuiManager.h"

namespace SyncShapes
{
	ImGuiManager::ImGuiManager(GLFWwindow* window) : m_Window(window), m_ImagePath(""), m_TextureID(0), m_TextureID2(0)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Enable docking
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330 core");

		m_LogBuffer.push_back("All subsystems initialized and ready to work!");
		std::cout << "ImGuiManager initialized" << std::endl;
	}

	ImGuiManager::~ImGuiManager() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		std::cout << "ImGuiManager destroyed" << std::endl;
	}

	void ImGuiManager::Update() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
	}

	void ImGuiManager::Render() {
		ShowImageProcessingEditor();
		ShowViewport();
		ShowLogger();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiManager::ShowImageProcessingEditor() {
		ImGui::Begin("Editor");

		ImGui::TextWrapped("Welcome to SyncShapes Content-Based Image Retrieval (CBIR) System!");
		ImGui::Spacing(); ImGui::Spacing();
		ImGui::TextWrapped("A lightweight yet powerful content-based image retrieval system. It focuses on efficient shape synchronization, allowing users to retrieve images based on their shape features.");

		ImGui::Separator(); ImGui::Spacing();
		ImGui::TextWrapped("Query An Image:");

		// File Dialog for Image Loading
		ImGui::Spacing();
		if (ImGui::Button("Load Image From Dataset")) {
			OPENFILENAMEW ofn;  // Wide character version
			wchar_t szFile[260]; // Buffer to store wide file path
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = szFile;  // Wide character buffer
			ofn.lpstrFile[0] = L'\0'; // Wide character constant
			ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
			ofn.lpstrFilter = L"Image Files (.bmp,.jpg,.jpeg,.png, and .gif)\0*.bmp;*.jpg;*.jpeg;*.png;*.gif\0All Files\0*.*\0";
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
			Log("Dataset Directory Path: ", WorkingDirectoryPath);

			auto imagesConversionStartTime = std::chrono::high_resolution_clock::now();
			ImageProcessor::ConvertAllGIFsToJPEGs(m_ImagePath);
			auto imagesConversionEndTime = std::chrono::high_resolution_clock::now();
			auto imagesConversionDuration = std::chrono::duration_cast<std::chrono::seconds>(imagesConversionEndTime - imagesConversionStartTime);
			Log("Pre-Processing: converting GIFs to JPEGs completed. Time: " + std::to_string(imagesConversionDuration.count()) + " s.");
		}

		ImGui::Separator(); ImGui::Spacing();
		ImGui::TextWrapped("Pre-processing:");

		ImGui::Spacing(); ImGui::Spacing();
		ImGui::TextWrapped("A set of operations applied to input images before extracting features or performing further analysis. Its purpose is to enhance image quality, reduce noise, and highlight relevant information. The aim to create a cleaner and more representative image for feature extraction.");

		ImGui::Spacing();
		static bool applyNoiseRemoval = false;
		ImGui::Checkbox("Apply Noise Removal", &applyNoiseRemoval);

		static bool applyHoleFilling = false;
		ImGui::Checkbox("Apply Hole Filling", &applyHoleFilling);

		static bool applyHistogramEqualization = false;
		ImGui::Checkbox("Apply Histogram Equalization", &applyHistogramEqualization);

		static bool applyContourAreaFiltering = false;
		ImGui::Checkbox("Apply Contour Area Filtering", &applyContourAreaFiltering);

		ImGui::Spacing();
		if (ImGui::Button("Apply Preprocessing")) {
			if (!ImageProcessor::m_PreprocessingDir.empty()) {
				if (!(applyNoiseRemoval || applyHoleFilling || applyHistogramEqualization || applyContourAreaFiltering)) {
					Log("Warning: No preprocessing option selected. Please choose at least one option.");
				}
				else {
					if (applyNoiseRemoval && applyHoleFilling && applyHistogramEqualization) {
						auto bothOperationsStartTime = std::chrono::high_resolution_clock::now();
						ImageProcessor::ApplyAllToDirectory(ImageProcessor::m_PreprocessingDir);
						auto bothOperationsEndTime = std::chrono::high_resolution_clock::now();
						auto bothOperationsDuration = std::chrono::duration_cast<std::chrono::seconds>(bothOperationsEndTime - bothOperationsStartTime);
						Log("Pre-Processing: Noise Removal, Hole Filling, Histogram Equalization, and Contour Area Filtering completed. Time: " + std::to_string(bothOperationsDuration.count()) + " s.");
					}
					else {
						int selectedOptionsCount = (applyNoiseRemoval ? 1 : 0) + (applyHoleFilling ? 1 : 0) + (applyHistogramEqualization ? 1 : 0);

						if (selectedOptionsCount == 2) {
							Log("Warning: Preprocessing allows for individual operation testing or all applied together for the retrieval system main aim or output.");
						}
						else {
							if (applyNoiseRemoval) {
								auto noiseRemovalStartTime = std::chrono::high_resolution_clock::now();
								ImageProcessor::ApplyNoiseRemovalToDirectory(ImageProcessor::m_PreprocessingDir);
								auto noiseRemovalEndTime = std::chrono::high_resolution_clock::now();
								auto noiseRemovalDuration = std::chrono::duration_cast<std::chrono::seconds>(noiseRemovalEndTime - noiseRemovalStartTime);
								Log("Pre-Processing: Noise Removal completed. Time: " + std::to_string(noiseRemovalDuration.count()) + " s.");
							}

							if (applyHoleFilling) {
								auto holeFillingStartTime = std::chrono::high_resolution_clock::now();
								ImageProcessor::ApplyHoleFillingToDirectory(ImageProcessor::m_PreprocessingDir);
								auto holeFillingEndTime = std::chrono::high_resolution_clock::now();
								auto holeFillingDuration = std::chrono::duration_cast<std::chrono::seconds>(holeFillingEndTime - holeFillingStartTime);
								Log("Pre-Processing: Hole Filling completed. Time: " + std::to_string(holeFillingDuration.count()) + " s.");
							}

							if (applyHistogramEqualization) {
								auto histogramEqualizationStartTime = std::chrono::high_resolution_clock::now();
								ImageProcessor::ApplyHistogramEqualizationToDirectory(ImageProcessor::m_PreprocessingDir);
								auto histogramEqualizationEndTime = std::chrono::high_resolution_clock::now();
								auto histogramEqualizationDuration = std::chrono::duration_cast<std::chrono::seconds>(histogramEqualizationEndTime - histogramEqualizationStartTime);
								Log("Pre-Processing: Histogram Equalization completed. Time: " + std::to_string(histogramEqualizationDuration.count()) + " s.");
							}

							if (applyContourAreaFiltering) {
								auto contourAreaFilteringStartTime = std::chrono::high_resolution_clock::now();
								ImageProcessor::ApplyContourAreaFilteringToDirectory(ImageProcessor::m_PreprocessingDir, 300);
								auto contourAreaFilteringEndTime = std::chrono::high_resolution_clock::now();
								auto contourAreaFilteringDuration = std::chrono::duration_cast<std::chrono::seconds>(contourAreaFilteringEndTime - contourAreaFilteringStartTime);
								Log("Pre-Processing: Contour Area Filtering completed. Time: " + std::to_string(contourAreaFilteringDuration.count()) + " s.");
							}
						}
					}
				}
			}
			else {
				Log("Error: Cannot apply preprocessing. Please load an image from the dataset first.");
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("?", ImVec2(20, 20))) {
			ImGui::OpenPopup("Preprocessing Help");
		}

		if (ImGui::BeginPopup("Preprocessing Help")) {
			ImGui::Text("Preprocessing Options:");
			ImGui::Text("1. Apply Noise Removal: Remove noise from the image using Gaussian blur.");
			ImGui::Text("2. Apply Hole Filling: Fill small holes in the image using morphological operations.");
			ImGui::Text("3. Apply Histogram Equalization: Enhance contrast using histogram equalization.");
			ImGui::Text("4. Apply Contour Area Filling: Remove small artifacts by filtering contours based on area.");

			ImGui::EndPopup();
		}

		ImGui::Separator(); ImGui::Spacing();
		ImGui::TextWrapped("Feature Extraction:");

		ImGui::Spacing(); ImGui::Spacing();
		ImGui::TextWrapped("Feature Extraction with Hu Moments. Hu Moments are seven numerical values that describe the shape characteristics of an image. They are invariant to translation, scale, and rotation, making them suitable for shape recognition.");

		ImGui::Spacing();
		if (ImGui::Button("Apply Feature Extraction"))
		{
			if (!ImageProcessor::m_PreprocessingDir.empty())
			{
				auto FeatureExtractionStartTime = std::chrono::high_resolution_clock::now();
				ImageProcessor::ExtractShapeFeaturesAndSave(ImageProcessor::m_PreprocessingDir); ImageProcessor::m_ContoursOverlay = true;
				auto FeatureExtractionEndTime = std::chrono::high_resolution_clock::now();
				auto FeatureExtractionDuration = std::chrono::duration_cast<std::chrono::seconds>(FeatureExtractionEndTime - FeatureExtractionStartTime);
				Log("Feature Extraction with Hu Moments completed. Time: " + std::to_string(FeatureExtractionDuration.count()) + " s.");
			}
			else
				Log("Error: Cannot apply Feature Extraction. Please apply Pre-processing first.");
		}

		ImGui::Separator(); ImGui::Spacing();
		ImGui::TextWrapped("Image Retrieval:");

		static int topK = 5;
		ImGui::InputInt("Similar Images Count", &topK);

		ImGui::Spacing();
		if (ImGui::Button("Apply Retrieval"))
		{
			if (!ImageProcessor::m_FeatureExtractionDir.empty())
			{
				auto ImageRetrievalStartTime = std::chrono::high_resolution_clock::now();

				std::string imageNameWithoutExtension = fs::path(m_ImagePath).stem().string();
				std::vector<std::pair<std::string, double>> retrievalResults = ImageProcessor::RetrieveImages(imageNameWithoutExtension, topK);

				Log("Image Retrieval Results:");
				for (const auto& result : retrievalResults)
				{
					Log("Image: " + result.first + ", Distance: " + std::to_string(result.second));
				}

				auto ImageRetrievalEndTime = std::chrono::high_resolution_clock::now();
				auto ImageRetrievalDuration = std::chrono::duration_cast<std::chrono::seconds>(ImageRetrievalEndTime - ImageRetrievalStartTime);

				Log("Image Retrieval completed. Time: " + std::to_string(ImageRetrievalDuration.count()) + " s.");
			}
			else
				Log("Error: Cannot apply Image Retrieval. Please apply Feature Extraction first.");
		}

		ImGui::Separator(); ImGui::Spacing();
		if (ImGui::Button("How To Use?", ImVec2(100, 40))) {
			ImGui::OpenPopup("Instructions");
		}

		if (ImGui::BeginPopup("Instructions")) {
			ImGui::Text("How To Use SyncShapes?");
			ImGui::Separator();

			ImGui::Text("\nEditor:");
			ImGui::Text("1. The editor serves as the control center for processing images.");

			ImGui::Text("\nViewport:");
			ImGui::Text("1. The viewport displays the preprocessed/processed images.");
			ImGui::Text("2. Use the 'Show Image' button to view your results.");
			ImGui::Text("3. Clear the viewport with the 'Clear Viewport' button.");

			ImGui::Text("\nLog Area:");
			ImGui::Text("1. The log area logs important messages and actions.");
			ImGui::Text("2. Clear the log with the 'Clear Log' button.");
			ImGui::Text("3. Execution time is printed for each operation in the log area.");

			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Text("\xc2\xa9 2024 Saeb K. H. Naser. All rights reserved.");

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void ImGuiManager::ShowViewport() {
		ImGui::Begin("Viewport");

		ImVec2 viewportSize(500, 500);

		ImGui::Image((void*)(intptr_t)m_TextureID, viewportSize);

		if (ImGui::Button("Show Image"))
		{
			OPENFILENAMEW ofn;
			wchar_t szFile[260];
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = L'\0';
			ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
			ofn.lpstrFilter = L"Image Files (.bmp,.jpg,.jpeg,and .png)\0*.bmp;*.jpg;*.jpeg;*.png;\0All Files\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

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
				Log("Warning: Viewport is already cleared.");
			}
			else
			{
				m_TextureID = 0; // 0 is an invalid texture ID, which clears the viewport
				Log("Viewport cleared.");
			}
		}

		ImGui::SameLine();
		static bool viewContourOverlay = false;
		if (ImGui::Checkbox("View Contour Overlay", &viewContourOverlay))
		{
			if (!m_ImagePath.empty())
			{
				m_TextureID = ImageProcessor::VisualizeImage(m_ImagePath, viewContourOverlay);
			}
			else
			{
				viewContourOverlay = false;
				Log("Error: Show an image first to visualize its contours.");
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
			if (logMessage.find("Error") != std::string::npos) {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "[%d] %s", logCounter++, logMessage.c_str());
			}
			else if (logMessage.find("Warning") != std::string::npos) {
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[%d] %s", logCounter++, logMessage.c_str());
			}
			else {
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[%d] %s", logCounter++, logMessage.c_str());
			}
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