#include "pch.h"
#include "Core/Application.h"
#include "Sandbox/Sandbox.h"

int main(int argc, char** argv)
{
	Sandbox* layer = new Sandbox();
	// Define the application specifications
	ApplicationSpecification appSpec;
	appSpec.AppLayer = layer;
	appSpec.argc = argc;
	appSpec.argv = argv;
	appSpec.Mode = RenderMode::GenerateImage;
	Application* app = new Application(appSpec);
	app->Run();
	delete app;
}
