#pragma once
#include "Camera.h"
#include "Scene/Scene.h"
#include "Math/Ray.h"
#include "Math/Random.h"
#include "AntiAliasing.h"
#include "Core/Application.h"
#include "Core/Utility.h"
#include <GL/glew.h>
#include <Core/Base.h>
#include <GL/freeglut.h>
namespace rayTracer
{
	struct SceneRendererSpec
	{
		Camera* Camera;
		uint32_t MaxDepth;
		uint32_t Width, Height;
		Scene* Scene;
	};

	class SceneRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginSubmit(SceneRendererSpec spec);
		static void EndSubmit();
		static void SubmitObject(Object* obj);
		static void OnResize(int width, int height);
		static void Render();

		// Rendering Options
		static void ToggleGammaCorrection();
		static void ToggleToneMapping();
		static void ToggleShadows();
		static void ChangeTracingDepth(int change);
		static void SwitchAntialiasingMode(AntialiasingMode newMode);

		// Getters
		static std::vector<Vec2> GetLightSamplingArray();
		static bool GetGammaCorrection();
		static bool GetToneMapping();
		static int GetTracingDepth();
		static float GetAperture();
		static AntialiasingMode GetAntialiasingMode();

	private:
		// Ray tracing Methods
		static Vec3 TraceRays(Ray& ray, uint32_t depth, float refractionIndex);
		static RayCastHit GetClosestHit(Ray& ray);
		static RayCastHit IsPointInShadow(RayCastHit& hit, Vec3& lightDir, float lightDistance);
		static Vec3 BlinnPhong(Material* mat, Light* light, Vec3& lightDir, Vec3& viewDir, Vec3& normal, float intensity);
		// Here is were the render of the points happen
		static void Flush();
		static void DrawPoints();
		static void GenerateImage();
		static uint32_t SaveImage(const std::string& filename);
		// Buffers 
		static void CreateBuffers();
		static void DestroyBuffers();
		// Shaders
		static void CreateShaders();
		static void DestroyShaders();
		// Main data (e.g image, colors, vertices)
		static void InitData();
		static void DestroyData();
	};

}