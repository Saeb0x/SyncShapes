#include <GL/glew.h>
#include "ImageProcessor.h"

namespace SyncShapes
{
	std::string ImageProcessor::m_PreprocessingDir("");
	std::string ImageProcessor::m_FeatureExtractionDir("");
	std::unordered_map<std::string, FeatureData> ImageProcessor::m_AllFeatures = std::unordered_map<std::string, FeatureData>();
	bool ImageProcessor::m_ContoursOverlay = false;

	ImageProcessor::ImageProcessor() {}
	ImageProcessor::~ImageProcessor() {}

	void ImageProcessor::DisplayImage(const std::string& imagePath, const std::string& windowName)
	{
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

			if (grayscaleImage) {
				fs::path gifDirectory = fs::path(gifImagePath).parent_path();

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

		FreeImage_DeInitialise();
	}

	void ImageProcessor::ConvertAllGIFsToJPEGs(const std::string& directoryPath)
	{
		std::string DirectoryPath = fs::path(directoryPath).parent_path().string();

		if (fs::exists(DirectoryPath) && fs::is_directory(DirectoryPath)) {
			std::cout << "Directory found: " << DirectoryPath << std::endl;

			for (const auto& entry : fs::directory_iterator(DirectoryPath)) {
				if (entry.is_regular_file()) {
					std::string currentFilePath = entry.path().string();

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

	GLuint ImageProcessor::VisualizeImage(const std::string& imagePath, bool drawContours)
	{
		cv::Mat image = cv::imread(imagePath);

		if (image.empty())
		{
			std::cerr << "Failed to load the image at path: " << imagePath << std::endl;
			return 0;
		}

		if (drawContours) {
			cv::Mat gray;
			cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

			// Apply threshold to create a binary image
			cv::Mat thresh;
			cv::threshold(gray, thresh, 128, 255, cv::THRESH_BINARY);

			// Find contours in the binary image
			std::vector<std::vector<cv::Point>> contours;
			cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

			// Draw contours on the original image
			cv::drawContours(image, contours, -1, cv::Scalar(0, 255, 0), 2);
		}

		cv::resize(image, image, cv::Size(500, 500));

		// Convert the processed image to an OpenGL texture
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		if (image.channels() == 1) {
			// Grayscale image
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image.cols, image.rows, 0, GL_RED, GL_UNSIGNED_BYTE, image.data);
		}
		else if (image.channels() == 3) {
			// Color image (BGR)
			cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
		}
		else if (image.channels() == 4) {
			// Color image with alpha (BGRA)
			cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
		}

		// Texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;
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
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
		cv::equalizeHist(image, image);
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

	void ImageProcessor::ApplyContourAreaFiltering(cv::Mat& inputImage, double minContourArea) {
		cv::Mat thresh;
		cv::threshold(inputImage, thresh, 128, 255, cv::THRESH_BINARY);

		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		for (size_t i = 0; i < contours.size(); ++i) {
			double contourArea = cv::contourArea(contours[i]);

			// Filter contours based on area
			if (contourArea < minContourArea) {
				// Set the region outside the contour to black
				cv::drawContours(inputImage, contours, static_cast<int>(i), cv::Scalar(0, 0, 0), cv::FILLED);
			}
		}
	}

	void ImageProcessor::ApplyContourAreaFilteringToDirectory(const std::string& directoryPath, double minContourArea) {
		fs::path outputDirectory = fs::path(directoryPath) / "contour_area_filtering";

		if (!(fs::exists(outputDirectory) && fs::is_directory(outputDirectory))) {
			// Create the "contour_area_filtering" directory if it doesn't exist
			fs::create_directory(outputDirectory);
		}

		for (const auto& entry : fs::directory_iterator(directoryPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".jpg") {
				cv::Mat image = cv::imread(entry.path().string());

				if (!image.empty()) {
					// Apply contour area filtering directly to the image
					ApplyContourAreaFiltering(image, minContourArea);

					// Save the modified image in the "contour_area_filtering" directory
					fs::path outputPath = outputDirectory / entry.path().filename();
					cv::imwrite(outputPath.string(), image);
				}
			}
		}
	}

	void ImageProcessor::ApplyAllToDirectory(const std::string& directoryPath)
	{
		for (const auto& entry : fs::directory_iterator(directoryPath))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".jpg")
			{
				// Load the original image
				cv::Mat originalImage = cv::imread(entry.path().string());

				if (!originalImage.empty())
				{
					// Apply preprocessing steps
					ApplyNoiseRemoval(originalImage);
					ApplyHoleFilling(originalImage);
					ApplyHistogramEqualization(originalImage);
					ApplyContourAreaFiltering(originalImage, 300);

					cv::imwrite(entry.path().string(), originalImage);
				}
			}
		}
	}

	void ImageProcessor::ExtractShapeFeatures(const std::string& imagePath)
	{
		cv::Mat image = cv::imread(imagePath);
		cv::Mat gray;
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

		cv::Mat thresh;
		cv::threshold(gray, thresh, 128, 255, cv::THRESH_BINARY);

		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		for (const auto& contour : contours)
		{
			cv::Moments moments = cv::moments(contour, false);  // Use binary image
			cv::Mat huMomentsMat;
			cv::HuMoments(moments, huMomentsMat);

			std::vector<double> huMoments;
			huMomentsMat.col(0).rowRange(0, 7).copyTo(huMoments);

			// Normalize the Hu moments
			for (double& moment : huMoments)
			{
				moment = -std::copysign(1.0, moment) * std::log10(std::abs(moment));
			}

			m_AllFeatures[imagePath].numShapes = contours.size();
			m_AllFeatures[imagePath].shapeFeatures.push_back(std::move(huMoments));
		}
	}

	void ImageProcessor::ExtractShapeFeaturesAndSave(const std::string& directoryPath)
	{
		// Clear existing features
		m_AllFeatures.clear();

		for (const auto& entry : fs::directory_iterator(directoryPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".jpg") {
				std::string imageName = entry.path().filename().stem().string();
				std::string imagePath = entry.path().string();

				ExtractShapeFeatures(imagePath);

				// Save features in m_AllFeatures with the image name
				m_AllFeatures[imageName] = std::move(m_AllFeatures[imagePath]);
				m_AllFeatures.erase(imagePath);
			}
		}

		// Save features to a file in a subdirectory named "feature-extraction"
		fs::path directory(directoryPath);
		fs::path subdirectory = directory / "feature-extraction";
		fs::create_directory(subdirectory); m_FeatureExtractionDir = subdirectory.string();

		std::string outputFileName = (subdirectory / "output_features.dat").string();
		SaveFeaturesToFile(outputFileName, m_AllFeatures);
	}

	void ImageProcessor::SaveFeaturesToFile(const std::string& outputFile, const std::unordered_map<std::string, FeatureData>& allFeatures)
	{
		std::ofstream outputFileStream(outputFile, std::ios::binary);

		if (outputFileStream.is_open()) {
			for (const auto& entry : allFeatures) {
				outputFileStream << entry.first << " " << entry.second.numShapes << " ";

				for (const auto& feature_vector : entry.second.shapeFeatures) {
					for (double moment : feature_vector) {
						outputFileStream << std::setprecision(10) << moment << " ";
					}
				}

				outputFileStream << "\n";
			}

			outputFileStream.close();
		}
		else {
			std::cerr << "Failed to open output file: " << outputFile << std::endl;
		}
	}

	GLuint ImageProcessor::VisualizeContours(const std::string& imagePath)
	{
		cv::Mat image = cv::imread(imagePath);

		if (image.empty())
		{
			std::cerr << "Failed to load the image at path: " << imagePath << std::endl;
			return 0;
		}

		cv::Mat gray;
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

		// Apply threshold to create a binary image
		cv::Mat thresh;
		cv::threshold(gray, thresh, 128, 255, cv::THRESH_BINARY);

		// Find contours in the binary image
		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		// Draw contours on the original image
		cv::drawContours(image, contours, -1, cv::Scalar(0, 255, 0), 2);

		// Resize the image to fit the dimensions of the viewport
		cv::resize(image, image, cv::Size(500, 500));

		// Convert the processed image to an OpenGL texture
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		if (image.channels() == 1) {
			// Grayscale image
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image.cols, image.rows, 0, GL_RED, GL_UNSIGNED_BYTE, image.data);
		}
		else if (image.channels() == 3) {
			// Color image (BGR)
			cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
		}
		else if (image.channels() == 4) {
			// Color image with alpha (BGRA)
			cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
		}

		// Texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;
	}

	std::vector<std::pair<std::string, double>> ImageProcessor::RetrieveImages(const std::string& queryImageName, int topK)
	{
		// Retrieve the query features
		const std::vector<std::vector<double>>& queryFeatures = m_AllFeatures[queryImageName].shapeFeatures;

		std::vector<std::pair<std::string, double>> distances;

		for (const auto& entry : m_AllFeatures) {

			double minDistance = std::numeric_limits<double>::infinity();

			for (const auto& storedFeature : entry.second.shapeFeatures) {
				// Calculate distance between query and stored features
				double distance = 0.0;

				for (size_t i = 0; i < queryFeatures.size(); ++i) {
					distance += cv::norm(cv::Mat(queryFeatures[i]), cv::Mat(storedFeature));
				}

				if (distance < minDistance) {
					minDistance = distance;
				}
			}

			distances.push_back({ entry.first, minDistance });
		}

		// Sort the distances vector based on the second element of the pairs (distances)
		std::sort(distances.begin(), distances.end(), [](const auto& a, const auto& b) {
			return a.second < b.second;
			});

		return distances.size() > topK ? std::vector<std::pair<std::string, double>>(distances.begin(), distances.begin() + topK) : distances;
	}
}
