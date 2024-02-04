# SyncShapes
A university assignment, featuring a lightweight yet powerful content-based image retrieval system. This assignment focuses on efficient shape synchronization, allowing users to retrieve images based on their shape features.

## Getting Started
Visual Studio 2022 is recommended, as SyncShapes is officially untested on other development environments while focusing on a Windows build. No build system generators are used at the moment.

1. <u>**Downloading the repository:**</u>

Start by cloning the repository with the following command: ```git clone --recursive https://github.com/Saeb0x/SyncShapes.git```.

If the repository was cloned non-recursively previously, use the following command to clone the necessary submodules ```git submodule update --init```.

2. <u>**Configuring the dependencies:**</u>

Build and configure the following dependencies as submodules: GLFW, ImGui, and OpenCV. Additionally, for convenience, GLEW and FreeImage precompiled binaries are included.

3. <u>**Building the Project:**</u>

Open the project in Visual Studio 2022 ```SyncShapes/SyncShapes.sln```, configure the necessary settings, and build the project.

Ensure that the build process includes the necessary dependencies and that the project compiles successfully.

## Features

- [x] **Pre-processing Operations:** SyncShapes includes a set of pre-processing operations such as noise removal, hole filling, histogram equalization, and contour area filtering to enhance image quality, reduce noise, and highlight relevant information. The aim is to create a cleaner and more representative image for feature extraction.
- [x] **Feature Extraction:** The system extracts shape features from images, utilizing the Hu Moments for effective content-based image retrieval. Hu Moments are seven numerical values that describe the shape characteristics of an image. They are invariant to translation, scale, and rotation, making them suitable for shape recognition.
- [x] **Image Retrieval:** SyncShapes provides an image retrieval functionality that allows users to find similar images based on their shape features.
- [x] **User-friendly GUI:** SyncShapes features an intuitive GUI powered by ImGui, providing an Editor for interactive image manipulation and procssing, a Viewport for images visualization including contour overlay, and a Log Area for tracking system activities and execution time for each operation.
- [ ]  **Cross-Dataset Retrieval:** Extend the system to support retrieval across multiple datasets, allowing users to find similarities between images from different sources.
- [ ]  **Cluster-Based Organization:** Organize images into clusters or groups based on similarity, providing users with a visual overview of how images are related.
- [ ] **Cross-Platform Support:** While SyncShapes is currently designed for Windows environments, there are aspirations to extend its compatibility to other operating systems in the future.
- [ ] **Build System Generators:** Future updates may include the integration of build system generators to facilitate smoother compilation and deployment on a wider range of development environments.

## Screenshots
![SyncShapes Screenshot1](https://github.com/Saeb0x/SyncShapes/assets/56490771/42a047b4-b061-4f6e-b42e-f89836d8aed5)

---
![SyncShapes Screenshot2](https://github.com/Saeb0x/SyncShapes/assets/56490771/8067b787-0676-4478-9b35-64edca6e812a)

