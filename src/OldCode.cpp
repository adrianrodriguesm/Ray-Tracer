#include "pch.h"
///////////////////////////////////////////////////////////////////////
//
// P3D Course
// (c) 2021 by Jo�o Madeiras Pereira
//Ray Tracing P3F scenes and drawing points with Modern OpenGL
//
///////////////////////////////////////////////////////////////////////
/** /
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <conio.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <IL/il.h>

#include "Scene/scene.h"
#include "Renderer/grid.h"
#include "Math/Vector.h"
#include "Core/Utility.h"
using namespace rayTracer;
#define CAPTION "Whitted Ray-Tracer"

#define VERTEX_COORD_ATTRIB 0
#define COLOR_ATTRIB 1

#define MAX_DEPTH 4
uint32_t FrameCount = 0;

// Current Camera Position
float camX, camY, camZ;

//Original Camera position;
Vec3 Eye;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = 0.0f, beta = 0.0f;
float r = 4.0f;

// Frame counting and FPS computation
long myTime, timebase = 0, frame = 0;
char s[32];

//Enable OpenGL drawing.  
bool drawModeEnabled = true;

// Points defined by 2 attributes: positions which are stored in vertices array and colors which are stored in colors array
float *colors;
float *vertices;
int size_vertices;
int size_colors;

//Array of Pixels to be stored in a file by using DevIL library
uint8_t *img_Data;

GLfloat m[16];  //projection matrix initialized by ortho function

GLuint VaoId;
GLuint VboId[2];

GLuint VertexShaderId, FragmentShaderId, ProgramId;
GLint UniformId;

Scene* scene = NULL;
int RES_X, RES_Y;

int WindowHandle = 0;





/////////////////////////////////////////////////////////////////////// ERRORS

bool isOpenGLError() 
{
	bool isError = false;
	GLenum errCode;
	const GLubyte *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
		isError = true;
		errString = gluErrorString(errCode);
		std::cerr << "OpenGL ERROR [" << errString << "]." << std::endl;
	}
	return isError;
}

void checkOpenGLError(std::string error)
{
	if(isOpenGLError()) {
		std::cerr << error << std::endl;
		exit(EXIT_FAILURE);
	}
}

/////////////////////////////////////////////////////////////////////// SHADERs

const GLchar* VertexShader =
{
	"#version 430 core\n"

	"in vec2 in_Position;\n"
	"in vec3 in_Color;\n"
	"uniform mat4 Matrix;\n"
	"out vec4 color;\n"

	"void main(void)\n"
	"{\n"
	"	vec4 position = vec4(in_Position, 0.0, 1.0);\n"
	"	color = vec4(in_Color, 1.0);\n"
	"	gl_Position = Matrix * position;\n"

	"}\n"
};
//const GLchar* FragmentShader = R"(FFGH)";
const GLchar* FragmentShader =
{
	"#version 430 core\n"

	"in vec4 color;\n"
	"out vec4 out_Color;\n"

	"void main(void)\n"
	"{\n"
	"	out_Color = color;\n"
	"}\n"
};

void createShaderProgram()
{
	VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderId, 1, &VertexShader, 0);
	glCompileShader(VertexShaderId);

	FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderId, 1, &FragmentShader, 0);
	glCompileShader(FragmentShaderId);

	ProgramId = glCreateProgram();
	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);

	glBindAttribLocation(ProgramId, VERTEX_COORD_ATTRIB, "in_Position");
	glBindAttribLocation(ProgramId, COLOR_ATTRIB, "in_Color");
	
	glLinkProgram(ProgramId);
	UniformId = glGetUniformLocation(ProgramId, "Matrix");

	checkOpenGLError("ERROR: Could not create shaders.");
}

void destroyShaderProgram()
{
	glUseProgram(0);
	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);

	glDeleteShader(FragmentShaderId);
	glDeleteShader(VertexShaderId);
	glDeleteProgram(ProgramId);

	checkOpenGLError("ERROR: Could not destroy shaders.");
}

/////////////////////////////////////////////////////////////////////// VAOs & VBOs


void createBufferObjects()
{
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	glGenBuffers(2, VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);

	glBufferData(GL_ARRAY_BUFFER, size_vertices, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VERTEX_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ARRAY_BUFFER, size_colors, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(COLOR_ATTRIB);
	glVertexAttribPointer(COLOR_ATTRIB, 3, GL_FLOAT, 0, 0, 0);
	
// unbind the VAO
	glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glDisableVertexAttribArray(VERTEX_COORD_ATTRIB); 
//	glDisableVertexAttribArray(COLOR_ATTRIB);
	checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void destroyBufferObjects()
{
	glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glDisableVertexAttribArray(COLOR_ATTRIB);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, VboId);
	glDeleteVertexArrays(1, &VaoId);
	checkOpenGLError("ERROR: Could not destroy VAOs and VBOs.");
}

void drawPoints()
{
	FrameCount++;

	glBindVertexArray(VaoId);
	glUseProgram(ProgramId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size_vertices, vertices);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size_colors, colors);

	glUniformMatrix4fv(UniformId, 1, GL_FALSE, m);
	glDrawArrays(GL_POINTS, 0, RES_X*RES_Y);
	glFinish();

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	checkOpenGLError("ERROR: Could not draw scene.");
}

ILuint saveImgFile(const char *filename) {
	ILuint ImageId;

	ilEnable(IL_FILE_OVERWRITE);
	ilGenImages(1, &ImageId);
	ilBindImage(ImageId);

	ilTexImage(RES_X, RES_Y, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, img_Data);
	ilSaveImage(filename);

	ilDisable(IL_FILE_OVERWRITE);
	ilDeleteImages(1, &ImageId);
	if (ilGetError() != IL_NO_ERROR)return ilGetError();

	return IL_NO_ERROR;
}

/////////////////////////////////////////////////////////////////////// CALLBACKS

void timer(int value)
{
	std::ostringstream oss;
	oss << CAPTION << ": " << FrameCount << " FPS @ (" << RES_X << "x" << RES_Y << ")";
	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
	FrameCount = 0;
	glutTimerFunc(1000, timer, 0);
}

bool IsPointInShadow(RayCastHit& hit, const Vec3& lightDir)
{
	Ray shadowFeeler(hit.InterceptionPoint, lightDir);
	for (auto obj : scene->GetObjects()) 
	{
		RayCastHit shadowHit = obj->Intercepts(shadowFeeler);
		if (shadowHit)
			return true;
	}
	return false;
}
RayCastHit GetClosestHit(Ray& ray, float& tmin)
{
	RayCastHit hit, temphit;
	for (auto& obj : scene->GetObjects())
	{	
		temphit = obj->Intercepts(ray);
		if (temphit && temphit.Tdist < tmin)
		{
			tmin = temphit.Tdist;
			hit = temphit;
		}
	}
	return hit;
}
Vec3 RayTracing(Ray ray, int depth, float ior_1)  //index of refraction of medium 1 where the ray is travelling
{
	float tmin = DBL_MAX;
	RayCastHit hit = GetClosestHit(ray, tmin);
	if (!hit)
		return scene->GetBackgroundColor();

	Material* material = hit.Object->GetMaterial();
	Vec3 color = material->GetDiffColor();
	Vec3 ambientColor = material->GetDiffColor();
	Vec3 normal = hit.Object->GetNormal(hit.InterceptionPoint);
	Vec3 viewDir = ray.Direction; // Unit vector
	for (auto& light : scene->GetLights())
	{
		// Beucause 
		Vec3 lightDir = Vec3(light->position - hit.InterceptionPoint).Normalized();
		if (IsPointInShadow(hit, lightDir))
			continue; // Zero light contribution for this point

		// Calculate ligth
		float lambertian = std::fmax(DotProduct(lightDir, normal), 0.0f);
		if (lambertian)
		{		
			Vec3 halfwayVector = viewDir + lightDir;
			Vec3 reflected = halfwayVector.Normalized();
			float specAngle = std::fmax(DotProduct(reflected, -lightDir), 0.0f);
			float specular = pow(specAngle, material->GetShine());

			float KdLamb = material->GetDiffuse() * lambertian;
			Vec3 diffuseColor;
			diffuseColor.r = ambientColor.r * light->color.r * KdLamb;
			diffuseColor.g = ambientColor.g * light->color.g * KdLamb;
			diffuseColor.b = ambientColor.b * light->color.b * KdLamb;

			float ksSpec = material->GetSpecular() * specular;
			Vec3 specularColor;
			specularColor.r = 0.3f * light->color.r * ksSpec;
			specularColor.g = 0.3f * light->color.g * ksSpec;
			specularColor.b = 0.3f * light->color.b * ksSpec;
									  
			color += diffuseColor + specularColor;
		}
	}
	if (depth >= MAX_DEPTH)
		return color;

	// Reflected Ray
	if (material->GetShine() >= 0)
	{
		Ray reflected = RayCastHit::CalculateReflectedRay(hit, viewDir);
		Vec3 reflectedColor = RayTracing(reflected, depth + 1, ior_1);
		color += material->GetSpecular() * reflectedColor;
	}
	// Refracted
	if (material->GetTransmittance() != 0)
	{
		
		Ray refracted = RayCastHit::CalculateRefractedRay(hit, ray, ior_1, material->GetRefrIndex());
		if (refracted.Direction != Vec3(0))
		{
			Vec3 refractedColor = RayTracing(refracted, depth + 1, ior_1);
			color += material->GetTransmittance() * refractedColor;
		}	
	}



	return color;
}
// Render function by primary ray casting from the eye towards the scene's objects
static void renderScene()
{
	int index_pos=0;
	int index_col=0;
	unsigned int counter = 0;
	if (drawModeEnabled) 
	{
		glClear(GL_COLOR_BUFFER_BIT);
		scene->GetCamera()->SetEye(Vec3(camX, camY, camZ));  //Camera motion
	}
	
	for (int y = 0; y < RES_Y; y++)
	{
		for (int x = 0; x < RES_X; x++)
		{
			Vec3 color;

			Vec3 pixel;  //viewport coordinates
			pixel.x = x + 0.5f;  
			pixel.y = y + 0.5f;

			// Calculos of the color
			Ray ray = scene->GetCamera()->PrimaryRay(pixel);
			color = RayTracing(ray, 1, 1.0);
			// Reinhard tonemapping
			static constexpr float exposure = 0.3f;
			color = utils::ApplyToneMapping(color, exposure);
			// Gamma correction
			color = utils::ConvertColorFromLinearToGammaSpace(color);
			
			img_Data[counter++] = (uint8_t)color.r;
			img_Data[counter++] = (uint8_t)color.g;
			img_Data[counter++] = (uint8_t)color.b;

			if (drawModeEnabled) 
			{
				vertices[index_pos++] = (float)x;
				vertices[index_pos++] = (float)y;
				colors[index_col++] = (float)color.r;

				colors[index_col++] = (float)color.g;

				colors[index_col++] = (float)color.b;
			}
		}
	
	}
	if(drawModeEnabled) 
	{
		drawPoints();
		glutSwapBuffers();
	}
	else {
		printf("Terminou o desenho!\n");
		if (saveImgFile("RT_Output.png") != IL_NO_ERROR) {
			printf("Error saving Image file\n");
			exit(0);
		}
		printf("Image file created\n");
	}
}

// Callback function for glutCloseFunc
void cleanup()
{
	destroyShaderProgram();
	destroyBufferObjects();
}

void ortho(float left, float right, float bottom, float top, 
			float nearp, float farp)
{
	m[0 * 4 + 0] = 2 / (right - left);
	m[0 * 4 + 1] = 0.0;
	m[0 * 4 + 2] = 0.0;
	m[0 * 4 + 3] = 0.0;
	m[1 * 4 + 0] = 0.0;
	m[1 * 4 + 1] = 2 / (top - bottom);
	m[1 * 4 + 2] = 0.0;
	m[1 * 4 + 3] = 0.0;
	m[2 * 4 + 0] = 0.0;
	m[2 * 4 + 1] = 0.0;
	m[2 * 4 + 2] = -2 / (farp - nearp);
	m[2 * 4 + 3] = 0.0;
	m[3 * 4 + 0] = -(right + left) / (right - left);
	m[3 * 4 + 1] = -(top + bottom) / (top - bottom);
	m[3 * 4 + 2] = -(farp + nearp) / (farp - nearp);
	m[3 * 4 + 3] = 1.0;
}

void reshape(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, w, h);
	ortho(0, (float)RES_X, 0, (float)RES_Y, -1.0, 1.0);
}

void processKeys(unsigned char key, int xx, int yy)
{
	switch (key) {

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


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
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

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX = -xx + startX;
	deltaY = yy - startY;

	// left mouse button: move camera
	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;
		rAux = r;
	}
	// right mouse button: zoom
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r + (deltaY * 0.01f);
		if (rAux < 0.1f)
			rAux = 0.1f;
	}

	camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camY = rAux * sin(betaAux * 3.14f / 180.0f);
}

void mouseWheel(int wheel, int direction, int x, int y) {

	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r * sin(beta * 3.14f / 180.0f);
}


/////////////////////////////////////////////////////////////////////// SETUP

void setupCallbacks() 
{
	glutKeyboardFunc(processKeys);
	glutCloseFunc(cleanup);
	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshape);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutMouseWheelFunc(mouseWheel);

	glutIdleFunc(renderScene);
	glutTimerFunc(0, timer, 0);
}

void setupGLEW() {
	glewExperimental = GL_TRUE;
	GLenum result = glewInit() ; 
	if (result != GLEW_OK) { 
		std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
		exit(EXIT_FAILURE);
	} 
	GLenum err_code = glGetError();
	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
}

void setupGLUT(int argc, char* argv[])
{
	glutInit(&argc, argv);
	
	glutInitContextVersion(4, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	
	glutInitWindowPosition(100, 250);
	glutInitWindowSize(RES_X, RES_Y);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glDisable(GL_DEPTH_TEST);
	WindowHandle = glutCreateWindow(CAPTION);
	if(WindowHandle < 1) {
		std::cerr << "ERROR: Could not create a new rendering window." << std::endl;
		exit(EXIT_FAILURE);
	}
}


void init(int argc, char* argv[])
{
	// set the initial camera position on its spherical coordinates
	Eye = scene->GetCamera()->GetEye();
	camX = Eye.x;
	camY = Eye.y;
	camZ = Eye.z;
	r = Magnitude(Eye);
	beta = asinf(camY / r) * 180.0f / 3.14f;
	alpha = atanf(camX / camZ) * 180.0f / 3.14f;

	setupGLUT(argc, argv);
	setupGLEW();
	std::cerr << "CONTEXT: OpenGL v" << glGetString(GL_VERSION) << std::endl;
	//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	createShaderProgram();
	createBufferObjects();
	setupCallbacks();
	
}


void init_scene(void)
{
	char scenes_dir[70] = "P3D_Scenes/";
	char input_user[50];
	char scene_name[70];

	while (true) {
		std::cout << "Input the Scene Name: ";
		std::cin >> input_user;
		strcpy_s(scene_name, sizeof(scene_name), scenes_dir);
		strcat_s(scene_name, sizeof(scene_name), input_user);

		std::ifstream file(scene_name, std::ios::in);
		if (file.fail()) {
			printf("\nError opening P3F file.\n");
		}
		else
			break;
	}

	scene = new Scene();
	scene->load_p3f(scene_name);
	RES_X = scene->GetCamera()->GetResX();
	RES_Y = scene->GetCamera()->GetResY();
	printf("\nResolutionX = %d  ResolutionY= %d.\n", RES_X, RES_Y);

	// Pixel buffer to be used in the Save Image function
	//img_Data = (uint8_t*)malloc(3 * RES_X * RES_Y * sizeof(uint8_t));
	img_Data = (uint8_t*)::operator new (3 * RES_X * RES_Y * sizeof(uint8_t));
	if (img_Data == NULL) exit(1);
}

int main(int argc, char* argv[])
{
	//Initialization of DevIL 
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		printf("wrong DevIL version \n");
		exit(0);
	}
	ilInit();

	int ch;
	if (!drawModeEnabled) {

		do {
			init_scene();
			auto timeStart = std::chrono::high_resolution_clock::now();
			renderScene();  //Just creating an image file
			auto timeEnd = std::chrono::high_resolution_clock::now();
			auto passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
			printf("\nDone: %.2f (sec)\n", passedTime / 1000);
		
			std::cout << "\nPress 'y' to render another image or another key to terminate!\n";
			delete(scene);
			//free(img_Data);
			::operator delete(img_Data, 3 * RES_X * RES_Y * sizeof(uint8_t));
			
			ch = _getch();
		} while((toupper(ch) == 'Y'));
	}

	else {   //Use OpenGL to draw image in the screen
		printf("OPENGL DRAWING MODE\n\n");
		init_scene();
		size_vertices = 2 * RES_X*RES_Y * sizeof(float);
		size_colors = 3 * RES_X*RES_Y * sizeof(float);
		vertices = (float*)malloc(size_vertices);
		if (vertices == NULL) exit(1);
		colors = (float*)malloc(size_colors);
		if (colors == NULL) exit(1);
	   
	
		init(argc, argv);
		glutMainLoop();
	}

	free(colors);
	free(vertices);
	printf("Program ended normally\n");
	exit(EXIT_SUCCESS);
}
/**/
///////////////////////////////////////////////////////////////////////