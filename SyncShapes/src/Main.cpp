#include <iostream>
#include "Window/Window.h"

int main()
{
	
	SyncShapes::Window mainWindow(1200, 768, "SyncShapes");

	mainWindow.Run();

	std::cout << "Application exited" << std::endl;

	return 0;
}