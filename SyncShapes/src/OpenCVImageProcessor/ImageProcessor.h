#pragma once

#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <FreeImage.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace SyncShapes
{
	struct ImageDetails
	{
		std::string type;
		int width;
		int height;
	};

	struct FeatureData {
		int numShapes;
		std::vector<std::vector<double>> shapeFeatures;
	};

	class ImageProcessor
	{
	public:
		ImageProcessor();
		~ImageProcessor();

		static void DisplayImage(const std::string& imagePath, const std::string& windowName);

		static GLuint GetTexture(const std::string& imagePath);
		static ImageDetails GetImageDetails(const std::string& imagePath);
		static cv::Mat ResizeImage(const std::string& imagePath, int width, int height);
		static void ConvertGIFToJPEG(const std::string& gifImagePath);
		static void ConvertAllGIFsToJPEGs(const std::string& directoryPath);
		static GLuint VisualizeImage(const std::string& imagePath, bool drawContours);

		// Pre-processing Stage
		static void ApplyNoiseRemoval(cv::Mat& image);
		static void ApplyNoiseRemovalToDirectory(const std::string& directoryPath);
		static void ApplyHoleFilling(cv::Mat& image);
		static void ApplyHoleFillingToDirectory(const std::string& directoryPath);
		static void ApplyHistogramEqualization(cv::Mat& image);
		static void ApplyHistogramEqualizationToDirectory(const std::string& directoryPath);
		static void ApplyContourAreaFiltering(cv::Mat& inputImage, double minContourArea);
		static void ApplyContourAreaFilteringToDirectory(const std::string& directoryPath, double minContourArea);
		static void ApplyAllToDirectory(const std::string& directoryPath);

		// Feature Extraction Stage
		static void ExtractShapeFeatures(const std::string& imagePath);
		static void ExtractShapeFeaturesAndSave(const std::string& directoryPath);
		static void SaveFeaturesToFile(const std::string& outputFile, const std::unordered_map<std::string, FeatureData>& allFeatures);
		static GLuint VisualizeContours(const std::string& imagePath);

		// Retrieval Stage
		static std::vector<std::pair<std::string, double>> RetrieveImages(const std::string& queryImageName, int topK);
	public:
		static std::string m_PreprocessingDir;
		static std::string m_FeatureExtractionDir;
		static std::unordered_map<std::string, FeatureData> m_AllFeatures;
		static bool m_ContoursOverlay;
	};
}

