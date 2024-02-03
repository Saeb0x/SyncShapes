# SyncShapes
A university assignment, featuring a lightweight yet powerful content-based image retrieval system. This assignment focuses on efficient shape synchronization, allowing users to retrieve images based on their shape features.

## Getting Started
Visual Studio 2022 is recommended, as SyncShapes is officially untested on other development environments while focusing on a Windows build. No build system generators are used at the moment.

1. **Downloading the repository:**
------------------------------

Start by cloning the repository with the following command: ```git clone --recursive https://github.com/Saeb0x/SyncShapes.git```.

If the repository was cloned non-recursively previously, use the following command to clone the necessary submodules ```git submodule update --init```.

2. **Configuring the dependencies:**
------------------------------
Build and configure the following dependencies as submodules:
    - GLFW
    - ImGui
    - OpenCV
Additionally, for convenience, GLEW and FreeImage precompiled binaries are included.

3. **Building the Project:**
------------------------------
Open the project in Visual Studio 2022 ```SyncShapes/SyncShapes.sln```, configure the necessary settings, and build the project.

Ensure that the build process includes the necessary dependencies and that the project compiles successfully.
