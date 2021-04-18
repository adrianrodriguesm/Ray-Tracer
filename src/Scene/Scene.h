#pragma once
#include "Renderer/Camera.h"
#include "Math/Ray.h"
#include "Math/BoundingBox.h"
#include "Scene/SceneObject.h"
#include "Renderer/Light.h"


namespace rayTracer
{
	class Scene
	{
	public:
		Scene();
		virtual ~Scene();

		Camera* GetCamera() { return m_Camera; }
		Vec3 GetBackgroundColor() { return m_BackgroundColor; }
		Vec3 GetSkyboxColor(Ray& r);
		bool GetSkyBoxFlg() { return m_SkyBoxFlag; }

		void SetBackgroundColor(Vec3 a_bgColor) { m_BackgroundColor = a_bgColor; }
		void LoadSkybox(const char*);
		void SetSkyBoxFlg(bool a_skybox_flg) { m_SkyBoxFlag = a_skybox_flg; }
		void SetCamera(Camera* a_camera) { m_Camera = a_camera; }

		int GetNumObjects();
		void AddObject(Object* o);
		Object* GetObject(unsigned int index);
		const std::vector<Object*>& GetObjects() const { return m_Objects; }
		const std::vector<Light*>& GetLights() const { return m_Lights; }
		int GetNumLights();
		void AddLight(Light* l);
		Light* GetLight(unsigned int index);

		//bool load_p3f(const char* name);  //Load NFF file method

	private:
		std::vector<Object*> m_Objects;
		std::vector<Light*> m_Lights;

		Camera* m_Camera;
		Vec3 m_BackgroundColor;  //Background color

		bool m_SkyBoxFlag = false;

		struct
		{
			ILubyte* img;
			uint32_t resX;
			uint32_t resY;
			uint32_t BPP; //bytes per pixel
		} m_SkyboxImage[6];

	};
}
