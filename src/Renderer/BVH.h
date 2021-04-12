#pragma once
#include "Scene/Scene.h"
#include "Core/Constant.h"
#include <vector>
#include <stack>
#include <queue>
#include <cmath>

namespace rayTracer
{

	class BVH
	{
		class Comparator {
		public:
			int dimension;

			bool operator() (Object* a, Object* b) {
				float ca = a->GetBoundingBox().centroid().GetAxisValue(dimension);
				float cb = b->GetBoundingBox().centroid().GetAxisValue(dimension);
				return ca < cb;
			}
		};

		class BVHNode {
		private:
			AABB bbox;
			bool leaf;
			unsigned int n_objs;
			unsigned int index;	// if leaf == false: index to left child node,
								// else if leaf == true: index to first Intersectable (Object *) in objects vector

		public:
			BVHNode(void);
			void setAABB(AABB& bbox_);
			void makeLeaf(unsigned int index_, unsigned int n_objs_);
			void makeNode(unsigned int left_index_);
			bool isLeaf() { return leaf; }
			unsigned int getIndex() { return index; }
			unsigned int getNObjs() { return n_objs; }
			AABB& getAABB() { return bbox; };
		};

	private:
		int Threshold = 2;
		std::vector<Object*> objects;
		std::vector<BVH::BVHNode*> nodes;

		struct StackItem {
			BVHNode* ptr;
			float t;
			StackItem(BVHNode* _ptr, float _t) : ptr(_ptr), t(_t) { }
		};

		std::stack<StackItem> hit_stack;

		void build_recursive(int left_index, int right_index, BVHNode* node);
	public:
		BVH(void);
		int getNumObjects();

		void Build(std::vector<Object*>& objects);
		bool Traverse(Ray& ray, Object** hit_obj, Vec3& hit_point);
		bool Traverse(Ray& ray);
	};

}
