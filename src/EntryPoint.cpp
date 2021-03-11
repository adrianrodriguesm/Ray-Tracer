#include "Core/Application.h"
#include "Sandbox.h"
/**/
int main(int argc, char** argv)
{
	Sandbox* layer = new Sandbox();
	// Define the application specifications
	AplicationSpecification appSpec;
	appSpec.AppLayer = layer;
	appSpec.argc = argc;
	appSpec.argv = argv;
	// TODO : currently only one mode is supported
	appSpec.Mode = RenderMode::DrawMode;
	Application* app = new Application(appSpec);
	app->Run();
	delete app;
}
/**/