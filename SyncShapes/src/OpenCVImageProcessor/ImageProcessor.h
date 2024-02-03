#pragma once

#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <FreeImage.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace SyncShapes
{
	struct ImageDetails
	{
		std::string type;
		int width;
		int height;
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

		static void ApplyNoiseRemoval(cv::Mat& image);
		static void ApplyNoiseRemovalToDirectory(const std::string& directoryPath);
		static void ApplyHoleFilling(cv::Mat& image);
		static void ApplyHoleFillingToDirectory(const std::string& directoryPath);
		static void ApplyHistogramEqualization(cv::Mat& image);
		static void ApplyHistogramEqualizationToDirectory(const std::string& directoryPath);
		static void ApplyNoiseRemovalAndHoleFillingAndHistogramEqualizationToDirectory(const std::string& directoryPath);

		// More advanced image processing functionalities can be added here

	public:
		static std::string m_PreprocessingDir;
	};
}

