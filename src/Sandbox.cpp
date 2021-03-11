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
}

void Sandbox::OnDetach()
{
	delete m_Scene;
}

void Sandbox::OnUpdate()
{
	static SceneRendererSpec spec;
	spec.Camera = m_Camera;
	spec.MaxDepth = MAX_DEPTH;
	spec.Width = RES_WIDTH;
	spec.Height = RES_HEIGHT;
	spec.Scene = m_Scene;
	SceneRenderer::BeginScene(spec);
	auto& objects = m_Scene->GetObjects();
	for(auto& object : objects)
	{
		SceneRenderer::SumitObject(object);
	}
	SceneRenderer::EndScene();
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
	if (state == GLUT_DOWN) {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha -= (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			r += (yy - startY) * 0.01f;
			if (r < 0.1f)
				r = 0.1f;
		}
		tracking = 0;
	}
}

void Sandbox::OnMouseMove(int xx, int yy)
{
	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX = -xx + startX;
	deltaY = yy - startY;

	// left mouse button: move camera
	if (tracking == 1) 
	{
		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;
		rAux = r;
	}
	// right mouse button: zoom
	else if (tracking == 2) 
	{
		alphaAux = alpha;
		betaAux = beta;
		rAux = r + (deltaY * 0.01f);
		if (rAux < 0.1f)
			rAux = 0.1f;
	}

	camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camY = rAux * sin(betaAux * 3.14f / 180.0f);

	m_Camera->SetEye({ camX, camY, camZ });
}

void Sandbox::OnMouseScroll(int wheel, int direction, int x, int y)
{
	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r * sin(beta * 3.14f / 180.0f);

	m_Camera->SetEye({ camX, camY, camZ });
}

void Sandbox::OnKeyPress(unsigned char key, int xx, int yy)
{
	switch (key) 
	{

	case 27:
		glutLeaveMainLoop();
		break;

	case 'r':
		camX = Eye.x;
		camY = Eye.y;
		camZ = Eye.z;
		r = Magnitude(Eye);
		beta = asinf(camY / r) * 180.0f / 3.14f;
		alpha = atanf(camX / camZ) * 180.0f / 3.14f;
		break;

	case 'c':
		printf("Camera Spherical Coordinates (%f, %f, %f)\n", r, beta, alpha);
		printf("Camera Cartesian Coordinates (%f, %f, %f)\n", camX, camY, camZ);
		break;
	}
}

void Sandbox::InitScene()
{
	char scenes_dir[70] = "P3D_Scenes/";
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
	RES_WIDTH = m_Camera->GetResX();
	RES_HEIGHT = m_Camera->GetResY();
	printf("\nResolutionX = %d  ResolutionY= %d.\n", RES_WIDTH, RES_HEIGHT);

	
	
}
