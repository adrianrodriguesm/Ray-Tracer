#pragma once
#include "Scene/Scene.h"
#include "Math/Random.h"
namespace rayTracer
{
	class SceneLoader
	{
	public:
		static Scene* LoadP3D(const std::string filepath);
		static Scene* Create_random_scene();
	};
}