#pragma once

#include <vector>
#include <IL/il.h>

#include "Renderer/Camera.h"
#include "Renderer/Color.h"
#include "Math/Vector.h"
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

		Camera* GetCamera() { return camera; }
		Color GetBackgroundColor() { return bgColor; }
		Color GetSkyboxColor(Ray& r);
		bool GetSkyBoxFlg() { return SkyBoxFlg; }

		void SetBackgroundColor(Color a_bgColor) { bgColor = a_bgColor; }
		void LoadSkybox(const char*);
		void SetSkyBoxFlg(bool a_skybox_flg) { SkyBoxFlg = a_skybox_flg; }
		void SetCamera(Camera* a_camera) { camera = a_camera; }

		int GetNumObjects();
		void AddObject(Object* o);
		Object* GetObject(unsigned int index);
		const vector<Object*>& GetObjects() const { return objects; }
		const vector<Light*>& GetLights() const { return lights; }
		int GetNumLights();
		void AddLight(Light* l);
		Light* GetLight(unsigned int index);

		bool load_p3f(const char* name);  //Load NFF file method

	private:
		vector<Object*> objects;
		vector<Light*> lights;

		Camera* camera;
		Color bgColor;  //Background color

		bool SkyBoxFlg = false;

		struct {
			ILubyte* img;
			uint32_t resX;
			uint32_t resY;
			uint32_t BPP; //bytes per pixel
		} skybox_img[6];

	};
}
