#pragma once

#include <vector>
#include "Scene/Scene.h"

namespace rayTracer
{
	class Grid
	{
	public:
		Grid();
		Grid(const std::vector<Object*> objects);
		virtual ~Grid();
		
		uint32_t GetNumObjects();
		Object* GetSceneObject(uint32_t index);
		// TODO removed
		void AddObject(Object* o);

		// Set up grid cells
		void BuildGrid();   
		RayCastHit Intercepts(Ray& r);
		bool Traverse(Ray& ray, Object** hitobject, Vec3& hitpoint);  //(const Ray& ray, double& tmin, ShadeRec& sr)
		//Traverse for shadow ray
		bool Traverse(Ray& ray);  
	private:
		Vec3 FindMinBounds();
		Vec3 FindMaxBounds();
		RayCastHit GetClossestHitInsideCell(std::vector<Object*> sceneObjects, Ray& ray);
		//Setup function for Grid traversal
		bool InitTraverse(Ray& ray, int& ix, int& iy, int& iz, float& dtx, float& dty, float& dtz, float& tx_next, float& ty_next, float& tz_next,
			int& ix_step, int& iy_step, int& iz_step, int& ix_stop, int& iy_stop, int& iz_stop);
	private:
		std::vector<Object*> m_SceneObjects;
		std::vector<std::vector<Object*>> m_Cells;
		// Number of cells in the x, y, and z directions
		Vec3Int m_CellNumberPerDim;
		// factor that allows to vary the number of cells
		float m_CellMultiplierFactor = 2.0f; // Approximately 8 times more cells than objects

		AABB m_BBox;	
	};
}
