#include "Application.h"
#include <IL/il.h>
#include "Base.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Math/Random.h"
#include "Renderer/SceneRenderer.h"
namespace rayTracer
{
	Application* Application::s_Instance = nullptr;
	Application::Application(AplicationSpecification spec)
		: m_Spec(spec)
	{
		ASSERT(!s_Instance, "Application already created!");
		s_Instance = this;
		// -- DEVIL
		{
			ASSERT(ilGetInteger(IL_VERSION_NUM) >= IL_VERSION, "Wrong DevIL version!")
			ilInit();
		}
		InitWindow();
		InitGraphicContext();
		SceneRenderer::Init();
		SetCallBacks();
		// Random Seed
		Random::Init();
		m_Spec.AppLayer->OnAttach();
	}
	Application::~Application()
	{
		m_Spec.AppLayer->OnDetach();
		delete m_Spec.AppLayer;
	}
	void Application::Run()
	{
		m_Spec.AppLayer->OnUploadScene();
		SceneRenderer::Render();

		glutMainLoop();
		
	}
	void Application::InitWindow()
	{
		glutInit(&m_Spec.argc, m_Spec.argv);

		glutInitContextVersion(4, 3);
		glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
		glutInitContextProfile(GLUT_CORE_PROFILE);

		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

		glutInitWindowPosition(100, 250);
		glutInitWindowSize(m_Spec.Width, m_Spec.Height);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
		glDisable(GL_DEPTH_TEST);
		m_GlutWindow = glutCreateWindow(m_Spec.Caption.c_str());
		ASSERT(m_GlutWindow >= 1, "ERROR: Could not create a new rendering window.");
	}
	// TODO CHECK if needed
	static int FrameCount = 0;
	void timer(int value)
	{
		auto& app = Application::Get();
		AplicationSpecification appSpec = app.GetSpecification();
		std::ostringstream oss;
		
		oss << appSpec.Caption << ": " << FrameCount << " FPS @ (" << appSpec.Width << "x" << appSpec.Height << ")";
		std::string s = oss.str();
		glutSetWindow(app.GetNativeWindow());
		glutSetWindowTitle(s.c_str());
		FrameCount = 0;
		glutTimerFunc(1000, timer, 0);
	}

	static void OnMouseKeyPress(int button, int state, int xx, int yy)
	{
		Application::Get().GetLayer().OnMouseKeyPress(button, state, xx, yy);
	}

	static void OnKeyPress(unsigned char key, int xx, int yy)
	{
		Application::Get().GetLayer().OnKeyPress(key,  xx, yy);
	}

	static void OnMouseMove(int xx, int yy)
	{
		Application::Get().GetLayer().OnMouseMove(xx, yy);
	}

	static void OnMouseScroll(int wheel, int direction, int x, int y)
	{
		Application::Get().GetLayer().OnMouseScroll(wheel, direction, x, y);
	}

	static void OnWindowClose()
	{
		SceneRenderer::Shutdown();
		Application::Get().GetLayer().OnWindowClose();
	}

	static void OnUpdate()
	{
		SceneRenderer::Render();
		FrameCount++;
	}

	static void OnWindowResize(int w, int h)
	{
		Application::Get().GetLayer().OnWindowResize(w, h);
	}

	void Application::SetCallBacks()
	{
		//if(m_Spec.Mode != RenderMode::GenerateImage)
			
		glutDisplayFunc(OnUpdate);
		glutKeyboardFunc(OnKeyPress);
		glutCloseFunc(OnWindowClose);
		glutReshapeFunc(OnWindowResize);
		glutMouseFunc(OnMouseKeyPress);
		glutMotionFunc(OnMouseMove);
		glutMouseWheelFunc(OnMouseScroll);

		glutIdleFunc(OnUpdate);
		glutTimerFunc(0, timer, 0);
	}
	void Application::InitGraphicContext()
	{
		glewExperimental = GL_TRUE;
		GLenum result = glewInit();
		ASSERT(result == GLEW_OK, "ERROR glewInit");
		printf("Vendor: %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version: %s\n", glGetString(GL_VERSION));
		printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		std::cerr << "CONTEXT: OpenGL v" << glGetString(GL_VERSION) << std::endl;
	}
}