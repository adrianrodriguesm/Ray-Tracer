#pragma once
#include "Camera.h"
#include "Scene/Scene.h"
#include "Math/Ray.h"
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
		static void SumitRenderSpec(SceneRendererSpec spec);
		static void SumitObject(Object* obj);
		static void OnResize(int width, int height);	
		static void ToggleGammaCorrection();	
		static void ToggleToneMapping();
		static void ChangeTracingDepth(int change);
		static void Render();
	private:
		// Ray tracing Methods
		static Vec3 TraceRays(Ray& ray, uint32_t depth, float refractionIndex);
		static RayCastHit GetClosestHit(Ray& ray, float tmin);
		static RayCastHit IsPointInShadow(RayCastHit& hit, Vec3& lightDir, float lightDistance);
		static Vec3 BlinnPhong(Material* mat, Light* light, Vec3& lightDir, Vec3& viewDir, Vec3& normal);
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