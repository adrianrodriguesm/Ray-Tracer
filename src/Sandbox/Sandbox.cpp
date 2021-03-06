#include "pch.h"
#include "Sandbox.h"
#include <fstream>

Sandbox::Sandbox()
{
	
}

Sandbox::~Sandbox()
{
}

#pragma region Callbacks
void Sandbox::OnAttach()
{
	InitScene();
	OnWindowResize(RES_WIDTH, RES_HEIGHT);
}

void Sandbox::OnDetach()
{
	delete m_Camera;
	delete m_Scene;
}

void Sandbox::OnUploadScene()
{
	static SceneRendererSpec spec;
	spec.Camera = m_Camera;
	spec.MaxDepth = MAX_DEPTH;
	spec.Width = RES_WIDTH;
	spec.Height = RES_HEIGHT;
	spec.Scene = m_Scene;
	// Upload the needed data for the renderer
	SceneRenderer::BeginSubmit(spec);
	auto& objects = m_Scene->GetObjects();
	for(auto& object : objects)
	{
		SceneRenderer::SubmitObject(object);
	}
	SceneRenderer::EndSubmit();
	Mat4 ortho = MatFactory::CreateOrthographicProjectionMatrix(0, (float)RES_WIDTH, 0, (float)RES_HEIGHT, -1.0, 1.0);
	m_Camera->SetProjectionMatrix(ortho);

}

void Sandbox::OnWindowClose()
{

}

void Sandbox::OnWindowResize(int w, int h)
{
	RES_WIDTH = (uint32_t)w;
	RES_HEIGHT = (uint32_t)h;
	m_CameraController.OnResize(w, h);
	SceneRenderer::OnResize(w, h);

}

void Sandbox::OnMouseKeyPress(int button, int state, int xx, int yy)
{
	m_CameraController.OnMouseKeyPress(button, state, xx, yy);
}

void Sandbox::OnMouseMove(int xx, int yy)
{
	m_CameraController.OnMouseMove(xx, yy);
}

void Sandbox::OnMouseScroll(int wheel, int direction, int x, int y)
{
	m_CameraController.OnMouseScroll(direction);
}

void Sandbox::OnKeyPress(unsigned char key, int xx, int yy)
{
	switch (key) 
	{

	case 27:
		glutLeaveMainLoop();
		break;

	case 'r':
		m_CameraController.ResetPosition();
		break;

	case 'c':
		m_CameraController.DisplayCameraInfo();
		break;

	case 'g':
		SceneRenderer::ToggleGammaCorrection();
		break;

	case 't':
		SceneRenderer::ToggleToneMapping();
		break;

	case 's':
		SceneRenderer::ToggleShadows();
		break;

	case 'i':
		SceneRenderer::ToggleGenerateImage();
		break;

	case'+':
		SceneRenderer::ChangeTracingDepth(1);
		break;

	case'-':
		SceneRenderer::ChangeTracingDepth(-1);
		break;

	case'1':
		SceneRenderer::SwitchAntialiasingMode(AntialiasingMode::NONE);
		break;

	case'2':
		SceneRenderer::SwitchAntialiasingMode(AntialiasingMode::REGULAR_SAMPLING);
		break;

	case'3':
		SceneRenderer::SwitchAntialiasingMode(AntialiasingMode::JITTERING);
		break;

	case'4':
		SceneRenderer::SwitchAccelererationStructure(AccelerationStructure::NONE);
		break;

	case'5':
		SceneRenderer::SwitchAccelererationStructure(AccelerationStructure::GRID);
		break;

	case'6':
		SceneRenderer::SwitchAccelererationStructure(AccelerationStructure::BVH);
		break;

	}
}

void Sandbox::OnSpecialKeyPress(int key, int xx, int yy)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		ChangeAperture(0.1f);
		break;

	case GLUT_KEY_DOWN:
		ChangeAperture(-0.1f);
		break;
	}
}

#pragma endregion

void Sandbox::InitScene()
{
	char scenes_dir[70] = "assets/scenesP3D/";
	char input_user[50];
	char scene_name[70];
	bool random_scene = false;

	while (true) 
	{
		std::cout << "Input the Scene Name: ";
		std::cin >> input_user;
		if (strcmp(input_user, "random") == 0)
		{
			random_scene = true;
			break;
		}
		strcpy_s(scene_name, sizeof(scene_name), scenes_dir);
		strcat_s(scene_name, sizeof(scene_name), input_user);

		std::ifstream file(scene_name, std::ios::in);
		if (file.fail())
			printf("\nError opening P3F file.\n");
		else
			break;
	}

	if (random_scene)
		m_Scene = SceneLoader::CreateRandomScene();
	else
		m_Scene = SceneLoader::LoadP3D(scene_name);	

	if (m_Scene->GetNumObjects() == 0)
	{
		std::cout << std::endl << "No objects in scene - Adding custom objects from sandbox" << std::endl << std::endl;
		AddObjects();
	}

	m_Camera = m_Scene->GetCamera();
	m_CameraController = CameraController(m_Camera);
	RES_WIDTH = m_Camera->GetResX();
	RES_HEIGHT = m_Camera->GetResY();
	Application::Get().SetResolution(RES_WIDTH, RES_HEIGHT);
	printf("\nResolutionX = %d  ResolutionY= %d.\n", RES_WIDTH, RES_HEIGHT);
	//printf("Warning: Resolution parameters are set in application class.\n");

	std::cout << std::endl << "------------- OPTIONS ---------------" << std::endl;
	std::cout << std::endl << "Gamma correction: " << (SceneRenderer::GetGammaCorrection() ? "ON" : "OFF") << std::endl << std::endl;

	std::cout << "Tone mapping: " << (SceneRenderer::GetToneMapping ? "ON" : "OFF") << std::endl << std::endl;

	std::cout << "Max Depth: " << SceneRenderer::GetTracingDepth() << std::endl << std::endl;

	std::cout << "Camera Aperture: " << m_Camera->GetAperture() << std::endl << std::endl;

	AntialiasingMode mode = SceneRenderer::GetAntialiasingMode();
	std::string modeName = mode == AntialiasingMode::NONE ? "None" : mode == AntialiasingMode::JITTERING ? "Jittering" : "Regular Sampling";
	std::cout << "Antialiasing Mode: " << modeName << std::endl << std::endl;

	AccelerationStructure accStruct = SceneRenderer::GetAccelerationStruct();
	std::string structName = accStruct == AccelerationStructure::NONE ? "None" : accStruct == AccelerationStructure::GRID ? "Grid" : "BVH";
	std::cout << "Acceleration Structure: " << structName << std::endl << std::endl;

	std::cout << "----------------------------------" << std::endl << std::endl;

	std::cout << "------------- INPUTS ---------------" << std::endl;
	std::cout << "'G' - Toggle Off/On Gamma Correction" << std::endl << std::endl;
	std::cout << "'T' - Toggle Off/On Tonemapping" << std::endl << std::endl;
	std::cout << "'S' - Toggle Off/On Shadows (Global)" << std::endl << std::endl;
	std::cout << "'I' - Toggle to take an screenshot" << std::endl << std::endl;
	std::cout << "'+'/'-' - Increment/Decrement Max Depth" << std::endl << std::endl;
	std::cout << "'UP'/'DOWN' - Increment/Decrement Aperture" << std::endl << std::endl;
	std::cout << "'1' - No antialiasing | '2' - Regular Sampling | '3' - Jittering" << std::endl << std::endl;
	std::cout << "'4' - No Acceleration Structure | '5' - Grid Acceleration | '6' - BVH Acceleration " << std::endl << std::endl;
	std::cout << "----------------------------------" << std::endl;

}

/// <summary>
/// Custom function to dynamically add objects to scene
/// </summary>
void Sandbox::AddObjects()
{
	//Sphere* testSphere = new Sphere(Vec3(0,5,0), 5);
	Material* mat = new Material(Vec3(0.2,0.2,0.7), 0.3, Vec3(0.2, 0.2, 0.2), 0.7, 20, 1, 1.0f, 0);
	Material* transparentChildMat = new Material(Vec3(0.2,0.2,0.7), 0.3, Vec3(0.2, 0.2, 0.2), 0.7, 20, 1, 2, 0);
	Material* diffuseMat = new Material(Vec3(0.8,0.8,0.8), 1.0f, Vec3(0.2, 0.2, 0.2), 0.0f, 1.0f, 0, 1, 0);

	BubbleSphere* testSphere = new BubbleSphere(Vec3(0,5,0), 5, 10, 0.6f, transparentChildMat);
	testSphere->SetSampleStep(0.01f);
	testSphere->SetMaterial(mat);
	//testSphere->SetTestChildren();
	//std::cout << *testSphere << std::endl;

	m_Scene->AddObject(testSphere);
	SceneRenderer::SubmitObject(testSphere);

	// Lights
	//AreaLight* aLight = new AreaLight(Vec3(0, 15, 0), Vec3(0.2f, 0.2f, 0.8f), Vec3(1, 0, 0), Vec3(0, 0, 1), 4);
	Light* pointLight = new Light(Vec3(0, 15, 0), Vec3(0.2f, 0.2f, 0.8f));

	//m_Scene->AddLight(aLight);
	m_Scene->AddLight(pointLight);
}

void Sandbox::ChangeAperture(float change)
{
	//float newAperture = m_Camera->SetAperture(m_Camera->GetAperture() + change);
	float newAperture = m_CameraController.IncreaseAperture(change);
	std::cout << "Aperture: " << newAperture << std::endl;
}
