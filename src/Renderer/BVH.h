#pragma once
#include "Scene/Scene.h"
#include "Core/Constant.h"
#include <stack>

namespace rayTracer
{
	class BVH
	{
	private:
		struct Comparator 
		{
			int Dimension;
			Comparator() = default;
			Comparator(int dimension) : Dimension(dimension) {}
			bool operator() (Object* a, Object* b) 
			{
				float ca = a->GetBoundingBox().Centroid().GetAxisValue(Dimension);
				float cb = b->GetBoundingBox().Centroid().GetAxisValue(Dimension);
				return ca < cb;
			}
		};
		class BVHNode 
		{
		private:
			AABB bbox;
			bool leaf;
			unsigned int n_objs;
			unsigned int index;	// if leaf == false: index to left child node,
								// else if leaf == true: index to first Intersectable (Object *) in objects vector

		public:
			BVHNode();
			void SetAABB(AABB& bbox_);
			void MakeLeaf(unsigned int index_, unsigned int n_objs_);
			void MakeNode(unsigned int left_index_);
			bool IsLeaf() { return leaf; }
			unsigned int GetIndex() { return index; }
			unsigned int GetNumObjs() { return n_objs; }
			AABB& GetAABB() { return bbox; };
		};
		struct StackItem
		{
			BVHNode* ptr;
			float t;
			StackItem(BVHNode* _ptr, float _t) : ptr(_ptr), t(_t) { }
		};

	public:
		BVH() = default;
		virtual ~BVH();
		int GetNumObjects() { return m_Objects.size(); }
		void Build(std::vector<Object*>&objects);
		RayCastHit Intercepts(Ray & ray);
		bool InterceptsShadows(Ray & ray, float lightDist);
		static const uint32_t MinLeafPrimitives = 2;
		static const uint32_t MaxDepth = 30;

	private:
		void BuildRecursive(int leftIndex, int rightIndex, BVHNode * node, uint32_t depth);
		uint32_t m_Depth = 1;
		std::vector<Object*> m_Objects;
		std::vector<BVH::BVHNode*> m_BVHNodes;
		std::stack<StackItem> m_HitStack;
	};

}
