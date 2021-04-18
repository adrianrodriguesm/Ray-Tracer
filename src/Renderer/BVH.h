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
	public:
		BVH() = default;
		virtual ~BVH() {}
		int GetNumObjects() { return m_Objects.size(); }
		void Build(std::vector<Object*>& objects);
		RayCastHit Intercepts(Ray& ray);
		bool InterceptsShadows(Ray& ray, float lightDist);
	public:
		static const uint32_t MinLeafPrimitives = 2;
		static const uint32_t MaxDepth = 30;
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
		// --- New struct --- 
		struct Joint
		{
			AABB LeftBBox, RightBBox;
			uint32_t ChildIndex;
		};

		struct Leaf
		{
			uint32_t Start, Size;
			bool Flag = false;
		};

		struct Node
		{
			Leaf Leaf;
			Joint Joint;
			
			void MakeLeaf(uint32_t start, uint32_t size)
			{
				Leaf.Flag = true;
				Leaf.Start = start;
				Leaf.Size = size;

			}
			bool IsLeaf() const { return Leaf.Flag; }
			uint32_t GetNumObjects() { return Leaf.Size; }
			uint32_t GetObjectIndex() { return Leaf.Start; }
			uint32_t GetChildIndex() { return Joint.ChildIndex; }
			void SetJoint(AABB leftBBox, AABB rightBBox, uint32_t childIndex)
			{
				Joint.LeftBBox = leftBBox;
				Joint.RightBBox = rightBBox;
				Joint.ChildIndex = childIndex;
			}
		};
		struct Work
		{
			uint32_t Start, NumObjects, NodeIndex, Depth;
			AABB BBox;
			Work() = default;
			Work(uint32_t start, uint32_t numPrimitives, uint32_t node, uint32_t depth, const AABB& bbox)
				:Start(start), NumObjects(numPrimitives), NodeIndex(node), Depth(depth), BBox(bbox){}
		};
		// --- End  --- 
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
			void makeNode(unsigned int left_index_);
			bool isLeaf() { return leaf; }
			unsigned int getIndex() { return index; }
			unsigned int getNObjs() { return n_objs; }
			AABB& getAABB() { return bbox; };
		};
		struct StackItem
		{
			BVHNode* ptr;
			float t;
			StackItem(BVHNode* _ptr, float _t) : ptr(_ptr), t(_t) { }
		};
	private:
		void BuildRecursive(int leftIndex, int rightIndex, BVHNode* node, uint32_t depth);
		void ConstructRecursive(uint32_t numObject, const AABB& bbox);
	private:
		int Threshold = 2;
		uint32_t m_Depth = 1;
		std::vector<Object*> m_Objects;
		std::vector<BVH::BVHNode*> nodes;
		std::vector<BVH::Node> m_Nodes;
		std::stack<StackItem> hit_stack;

	};

}
