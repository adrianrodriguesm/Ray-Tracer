#include "Sandbox.h"


Sandbox::Sandbox()
{
	
}

Sandbox::~Sandbox()
{
}

void Sandbox::OnAttach()
{
	InitScene();
	//AddObjects();
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
	SceneRenderer::BeginSumit(spec);
	auto& objects = m_Scene->GetObjects();
	for(auto& object : objects)
	{
		SceneRenderer::SumitObject(object);
	}
	SceneRenderer::EndSumit();
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
	Mat4 ortho = MatFactory::CreateOrthographicProjectionMatrix(0, (float)RES_WIDTH, 0, (float)RES_HEIGHT, -1.0, 1.0);
	m_Camera->SetProjectionMatrix(ortho);
	m_Camera->OnResize(w, h);
	SceneRenderer::OnResize(w, h);

}

void Sandbox::OnMouseKeyPress(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN) 
	{
		lastX = xx;
		lastY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP)
	{
		tracking = 0;
	}
}

void Sandbox::OnMouseMove(int xx, int yy)
{
	if (tracking == 0)
		return;
	Vec3 finalPos = m_Camera->GetEye();
	int deltaX, deltaY;

	deltaX = -xx + lastX;
	deltaY = yy - lastY;
	lastX = xx;
	lastY = yy;

	// left mouse button: move camera
	if (tracking == 1) 
	{
		alpha += deltaX;
		beta += deltaY;


		
		if (beta > 85.0f)
			beta = 85.0f;
		else if (beta < -85.0f)
			beta = -85.0f;

		Vec3 center = m_Camera->GetCenter();
		Vec3 tempPoint = m_Camera->GetEye() - center;// Translate to center

		Qtrn pitch = Qtrn(beta, { -1,0,0 });
		Qtrn yaw = Qtrn(alpha, { 0,1,0 });
		Qtrn rot = pitch * yaw;
		Qtrn rotatedPoint = rot * Qtrn(0, startPos.x, startPos.y, startPos.z) * conjugate(rot);

		finalPos = Vec3(rotatedPoint.x + center.x, rotatedPoint.y + center.y, rotatedPoint.z + center.z); // Translate back
	}
	// right mouse button: zoom
	else if (tracking == 2) 
	{
		//alphaAux = alpha;
		//betaAux = beta;
		r = r + (deltaY * 0.01f);
		if (r < 0.1f)
			r = 0.1f;

		finalPos += Normalize(finalPos - m_Camera->GetCenter()) * r;
	}

	m_Camera->SetEye(finalPos);
	//camX = center.x + r * sin(beta * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	//camZ = center.z + r * cos(alpha * 3.14f / 180.0f) * cos(alpha * 3.14f / 180.0f);
	//camY = center.y + r * sin(beta * 3.14f / 180.0f);

	//m_Camera->SetEye({ camX, camY, camZ });
}

void Sandbox::OnMouseScroll(int wheel, int direction, int x, int y)
{
	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	//camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	//camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	//camY = r * sin(beta * 3.14f / 180.0f);

	//m_Camera->SetEye({ camX, camY, camZ });
}

void Sandbox::OnKeyPress(unsigned char key, int xx, int yy)
{
	switch (key) 
	{

	case 27:
		glutLeaveMainLoop();
		break;

	case 'r':
		m_Camera->SetEye(startPos);
		r = Magnitude(startPos - m_Camera->GetCenter());
		beta = 0;//asinf(camY / r) * 180.0f / 3.14f;
		alpha = 0;//atanf(camX / camZ) * 180.0f / 3.14f;
		break;

	case 'c':
		printf("Camera Spherical Coordinates (%f, %f, %f)\n", r, beta, alpha);
		//printf("Camera Cartesian Coordinates (%f, %f, %f)\n", camX, camY, camZ);
		m_Camera->DumpSelf();
		break;

	case 'g':
		SceneRenderer::ToggleGammaCorrection();
		break;

	case 't':
		SceneRenderer::ToggleToneMapping();
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
	}
}

void Sandbox::InitScene()
{
	char scenes_dir[70] = "assets/scenesP3D/";
	char input_user[50];
	char scene_name[70];

	while (true) 
	{
		std::cout << "Input the Scene Name: ";
		std::cin >> input_user;
		strcpy_s(scene_name, sizeof(scene_name), scenes_dir);
		strcat_s(scene_name, sizeof(scene_name), input_user);

		std::ifstream file(scene_name, std::ios::in);
		if (file.fail())
			printf("\nError opening P3F file.\n");
		else
			break;
	}

	m_Scene = SceneLoader::LoadP3D(scene_name);	
	m_Camera = m_Scene->GetCamera();
	this->startPos = m_Camera->GetEye();
	this->r = (m_Camera->GetCenter() - m_Camera->GetEye()).Magnitude();
	RES_WIDTH = m_Camera->GetResX();
	RES_HEIGHT = m_Camera->GetResY();
	printf("\nResolutionX = %d  ResolutionY= %d.\n", RES_WIDTH, RES_HEIGHT);
	printf("Warning: Resolution parameters are set in application class.\n");

	std::cout << std::endl << "Gamma correction: " << (SceneRenderer::GetGammaCorrection() ? "ON" : "OFF") << std::endl;
	std::cout << "'G' - Toggle Off/On" << std::endl << std::endl;

	std::cout << "Tone mapping: " << (SceneRenderer::GetToneMapping ? "ON" : "OFF") << std::endl;
	std::cout << "'T' - Toggle Off/On" << std::endl << std::endl;

	std::cout << "Max Depth: " << SceneRenderer::GetTracingDepth() << std::endl;
	std::cout << "'+'/'-' - Increment/Decrement depth " << std::endl << std::endl;

	AntialiasingMode mode = SceneRenderer::GetAntialiasingMode();
	std::string modeName = mode == AntialiasingMode::NONE ? "None" : mode == AntialiasingMode::JITTERING ? "Jittering" : "Regular Sampling";
	std::cout << "Antialiasing Mode: " << modeName << std::endl;
	std::cout << "'1' - No antialiasing | '2' - Regular Sampling | '3' - Jittering" << std::endl << std::endl;

}

/// <summary>
/// Custom function to dynamically add objects to scene
/// </summary>
void Sandbox::AddObjects()
{
	Sphere* testSphere = new Sphere(Vec3(0,5,0), 5);
	Material* mat = new Material(Vec3(0.2,0.2,0.7), 0.3, Vec3(0.2, 0.2, 0.2), 0.7, 20, 1, 2);
	testSphere->SetMaterial(mat);
	m_Scene->AddObject(testSphere);
	SceneRenderer::SumitObject(testSphere);
}
