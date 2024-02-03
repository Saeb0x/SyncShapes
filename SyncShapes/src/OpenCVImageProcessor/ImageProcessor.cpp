#include <GL/glew.h>
#include "ImageProcessor.h"

namespace SyncShapes
{
	std::string ImageProcessor::m_PreprocessingDir("");

	ImageProcessor::ImageProcessor() {}
	ImageProcessor::~ImageProcessor() {}

	void ImageProcessor::DisplayImage(const std::string& imagePath, const std::string& windowName)
	{
		// Load the image using OpenCV
		cv::Mat image = cv::imread(imagePath);

		if (!image.empty()) {
			cv::namedWindow(windowName, cv::WINDOW_NORMAL);
			cv::imshow(windowName, image);
			cv::waitKey(0);
		}
		else {
			std::cout << "Failed to load the image. Cannot display." << std::endl;
		}
	}

	GLuint ImageProcessor::GetTexture(const std::string& imagePath)
	{
		cv::Mat inputImage = cv::imread(imagePath);

		if (inputImage.empty()) {
			std::cout << "Failed to load the image at path: " << imagePath << std::endl;
			return 0;
		}

		// Resize the image to fit the dimensions of the viewport
		cv::resize(inputImage, inputImage, cv::Size(500, 500));

		// Convert the processed image to an OpenGL texture
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		if (inputImage.channels() == 1) {
			// Grayscale image
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, inputImage.cols, inputImage.rows, 0, GL_RED, GL_UNSIGNED_BYTE, inputImage.data);
		}
		else if (inputImage.channels() == 3) {
			// Color image (BGR)
			cv::cvtColor(inputImage, inputImage, cv::COLOR_BGR2RGB);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, inputImage.cols, inputImage.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, inputImage.data);
		}
		else if (inputImage.channels() == 4) {
			// Color image with alpha (BGRA)
			cv::cvtColor(inputImage, inputImage, cv::COLOR_BGRA2RGBA);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, inputImage.cols, inputImage.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, inputImage.data);
		}

		// Texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		return textureID;
	}

	ImageDetails ImageProcessor::GetImageDetails(const std::string& imagePath) {
		ImageDetails details;
		cv::Mat inputImage = cv::imread(imagePath);

		if (inputImage.empty()) {
			std::cout << "Failed to load the image at path: " << imagePath << std::endl;
			return details;
		}

		if (inputImage.channels() == 1) {
			details.type = "Grayscale";
		}
		else if (inputImage.channels() == 3 && inputImage.type() == CV_8UC3) {
			cv::Mat channels[3];
			cv::split(inputImage, channels);

			// Check if all three channels are identical
			if (cv::countNonZero(channels[0] != channels[1]) == 0 &&
				cv::countNonZero(channels[0] != channels[2]) == 0) {
				details.type = "Grayscale";
			}
			else {
				details.type = "RGB";
			}
		}
		else {
			details.type = "Unknown";
		}

		// Image size
		details.width = inputImage.cols;
		details.height = inputImage.rows;

		return details;
	}

	cv::Mat ImageProcessor::ResizeImage(const std::string& imagePath, int width, int height) {

		cv::Mat inputImage = cv::imread(imagePath);

		if (!inputImage.empty()) {
			cv::Mat resizedImage;
			cv::resize(inputImage, resizedImage, cv::Size(width, height));
			return resizedImage;
		}
		else {
			std::cout << "Failed to load the image!" << std::endl;
			return cv::Mat();
		}
	}

	void ImageProcessor::ConvertGIFToJPEG(const std::string& gifImagePath)
	{
		FreeImage_Initialise();

		// Loading the GIF image using FreeImage
		FIBITMAP* gifImage = FreeImage_Load(FIF_GIF, gifImagePath.c_str(), GIF_DEFAULT);

		if (gifImage) {
			// Convert the GIF image to grayscale if it has multiple channels
			FIBITMAP* grayscaleImage = FreeImage_ConvertToGreyscale(gifImage);

			if (grayscaleImage) { // Check if the conversion was successful
				// Get the directory path of the GIF image
				fs::path gifDirectory = fs::path(gifImagePath).parent_path();

				// Get the filename without extension
				std::string filenameWithoutExtension = fs::path(gifImagePath).stem().string();

				// Create a "pre-processing" directory
				fs::path outputDirectory = gifDirectory / "pre-processing"; m_PreprocessingDir = outputDirectory.string();
				if (fs::create_directory(outputDirectory) || fs::exists(outputDirectory)) {
					// Save as JPEG in the "pre-processing" directory with the original filename
					fs::path jpegPath = outputDirectory / (filenameWithoutExtension + ".jpg");
					FreeImage_Save(FIF_JPEG, grayscaleImage, jpegPath.string().c_str(), JPEG_DEFAULT);
				}
				else {
					std::cerr << "Failed to create the 'pre-processing' directory." << std::endl;
				}

				// Unload the images
				FreeImage_Unload(gifImage);
				FreeImage_Unload(grayscaleImage);
			}
			else {
				std::cerr << "Failed to convert the GIF image to grayscale." << std::endl;
			}
		}
		else {
			std::cerr << "Failed to load the GIF image at path: " << gifImagePath << std::endl;
		}

		// Deinitialize FreeImage
		FreeImage_DeInitialise();
	}

	void ImageProcessor::ConvertAllGIFsToJPEGs(const std::string& directoryPath)
	{
		// Extract the directory part of the given path
		std::string DirectoryPath = fs::path(directoryPath).parent_path().string();

		// Check if the directory exists
		if (fs::exists(DirectoryPath) && fs::is_directory(DirectoryPath)) {
			std::cout << "Directory found: " << DirectoryPath << std::endl;

			// Iterate through all files in the directory
			for (const auto& entry : fs::directory_iterator(DirectoryPath)) {
				if (entry.is_regular_file()) {
					std::string currentFilePath = entry.path().string();

					// Check if the file has a .gif extension
					if (fs::path(currentFilePath).extension() == ".gif") {
						ConvertGIFToJPEG(currentFilePath);
					}
				}
			}
		}
		else {
			std::cerr << "The specified directory does not exist or is not a directory." << std::endl;
		}
	}

	void ImageProcessor::ApplyNoiseRemoval(cv::Mat& image)
	{
		// GaussianBlur for noise removal
		cv::GaussianBlur(image, image, cv::Size(5, 5), 0);
	}

	void ImageProcessor::ApplyNoiseRemovalToDirectory(const std::string& directoryPath)
	{
		// Create a "noiseremoval" directory inside the original directory
		fs::path outputDirectory = fs::path(directoryPath) / "noiseremoval";

		if (!fs::create_directory(outputDirectory) && !fs::exists(outputDirectory))
		{
			// Log an error if the directory creation fails
			std::cerr << "Failed to create 'noiseremoval' directory in: " << directoryPath << std::endl;
			return;
		}

		for (const auto& entry : fs::directory_iterator(directoryPath))
		{
			// Check if the file is a JPEG image
			if (entry.is_regular_file() && entry.path().extension() == ".jpg")
			{
				// Load the image
				cv::Mat image = cv::imread(entry.path().string());

				if (!image.empty())
				{
					// Apply noise removal
					ApplyNoiseRemoval(image);

					// Save the modified image to the "noiseremoval" folder
					fs::path outputPath = outputDirectory / entry.path().filename();
					cv::imwrite(outputPath.string(), image);
				}
			}
		}
	}

	void ImageProcessor::ApplyHoleFilling(cv::Mat& image)
	{
		cv::Mat grayscaleImage;
		cv::cvtColor(image, grayscaleImage, cv::COLOR_BGR2GRAY);

		cv::Mat binaryMask;
		cv::threshold(grayscaleImage, binaryMask, 1, 255, cv::THRESH_BINARY);

		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(binaryMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		cv::drawContours(image, contours, -1, cv::Scalar(255, 255, 255), cv::FILLED);
	}

	void ImageProcessor::ApplyHoleFillingToDirectory(const std::string& directoryPath) {
		fs::path outputDirectory = fs::path(directoryPath) / "holefilling";

		if (!(fs::exists(outputDirectory) && fs::is_directory(outputDirectory))) {
			// Create the "holefilling" directory if it doesn't exist
			fs::create_directory(outputDirectory);
		}

		for (const auto& entry : fs::directory_iterator(directoryPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".jpg") {
				cv::Mat image = cv::imread(entry.path().string());

				if (!image.empty()) {
					ApplyHoleFilling(image);

					// Save the modified image in the "holefilling" directory
					fs::path outputPath = outputDirectory / entry.path().filename();
					cv::imwrite(outputPath.string(), image);
				}
			}
		}
	}

	void ImageProcessor::ApplyHistogramEqualization(cv::Mat& image) {
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);  // Convert to grayscale if not already
		cv::equalizeHist(image, image);  // Apply Histogram Equalization
	}

	void ImageProcessor::ApplyHistogramEqualizationToDirectory(const std::string& directoryPath) {
		fs::path outputDirectory = fs::path(directoryPath) / "histogramequalization";

		if (!(fs::exists(outputDirectory) && fs::is_directory(outputDirectory))) {
			// Create the "histogramequalization" directory if it doesn't exist
			fs::create_directory(outputDirectory);
		}

		for (const auto& entry : fs::directory_iterator(directoryPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".jpg") {
				cv::Mat image = cv::imread(entry.path().string());

				if (!image.empty()) {
					ApplyHistogramEqualization(image);

					// Save the modified image in the "histogramequalization" directory
					fs::path outputPath = outputDirectory / entry.path().filename();
					cv::imwrite(outputPath.string(), image);
				}
			}
		}
	}

	void ImageProcessor::ApplyNoiseRemovalAndHoleFillingAndHistogramEqualizationToDirectory(const std::string& directoryPath)
	{
		fs::path outputDirectory = fs::path(directoryPath) / "noiseremoval-holefilling-histogramequalization";

		if (!(fs::exists(outputDirectory) && fs::is_directory(outputDirectory))) {
			// Create the "noiseremoval-holefilling" directory if it doesn't exist
			fs::create_directory(outputDirectory);
		}

		for (const auto& entry : fs::directory_iterator(directoryPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".jpg") {
				cv::Mat image = cv::imread(entry.path().string());

				if (!image.empty()) {
					ApplyNoiseRemoval(image);
					ApplyHoleFilling(image);
					ApplyHistogramEqualization(image);

					// Save the modified image in the "noiseremoval-holefilling" directory
					fs::path outputPath = outputDirectory / entry.path().filename();
					cv::imwrite(outputPath.string(), image);
				}
			}
		}
	}
}