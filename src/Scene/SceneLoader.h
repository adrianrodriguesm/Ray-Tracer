#pragma once
#include <Scene/Scene.h>
namespace rayTracer
{
	class SceneLoader
	{
	public:
		static Scene* LoadP3D(const std::string filepath);
	};
}