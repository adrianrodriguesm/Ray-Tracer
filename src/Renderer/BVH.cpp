#include "BVH.h"

using namespace std;
namespace rayTracer
{


	BVH::BVHNode::BVHNode() {
		this->leaf = false;
		this->index = 0;
		this->n_objs = 0;
	}

	void BVH::BVHNode::setAABB(AABB& bbox_) { this->bbox = bbox_; }

	void BVH::BVHNode::makeLeaf(unsigned int index_, unsigned int n_objs_) {
		this->leaf = true;
		this->index = index_;
		this->n_objs = n_objs_;
	}

	void BVH::BVHNode::makeNode(unsigned int left_index_) {
		this->leaf = false;
		this->index = left_index_;
		this->n_objs = 0; 
	}


	BVH::BVH(void) {}

	int BVH::getNumObjects() { return objects.size(); }

#pragma region Build Functions
	void BVH::Build(vector<Object*>& objs) 
	{

		BVHNode* root = new BVHNode();

		Vec3 min = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
		Vec3 max = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		AABB world_bbox = AABB(min, max);

		for (Object* obj : objs) {
			AABB bbox = obj->GetBoundingBox();
			world_bbox.extend(bbox);
			objects.push_back(obj);
		}
		world_bbox.Min.x -= EPSILON; world_bbox.Min.y -= EPSILON; world_bbox.Min.z -= EPSILON;
		world_bbox.Max.x += EPSILON; world_bbox.Max.y += EPSILON; world_bbox.Max.z += EPSILON;
		root->setAABB(world_bbox);
		nodes.push_back(root);
		build_recursive(0, objects.size(), root); // -> root node takes all the 
	}

	void BVH::build_recursive(int left_index, int right_index, BVHNode* node) {
		//PUT YOUR CODE HERE


		 //right_index, left_index and split_index refer to the indices in the objects vector
		// do not confuse with left_nodde_index and right_node_index which refer to indices in the nodes vector. 
		 // node.index can have a index of objects vector or a index of nodes vector

		if ((right_index - left_index) <= Threshold)
		{
			node->makeLeaf(left_index, right_index - left_index);
			return;
		}
		else {
			node->makeNode(nodes.size());

			// Sort objects by largest axis
			Comparator cmp = Comparator();
			int largestAxis = GetLargestAxis(objects, left_index, right_index);
			cmp.dimension = largestAxis;
			sort(objects.begin() + left_index, objects.begin() + right_index, cmp);
			
			// Find split
			Vec3 midPoint = node->getAABB().centroid();
			int splitIndex = GetSplitIndex(objects, left_index, right_index, midPoint, largestAxis);

			// Make sure no side is empty
			if (splitIndex == left_index) 
				splitIndex++;
			else if(splitIndex == right_index)
				splitIndex--;

			// Create child nodes
			BVHNode* left = new BVHNode();
			BVHNode* right = new BVHNode();
			left->setAABB(CalculateBoundingBox(objects, left_index, splitIndex));
			right->setAABB(CalculateBoundingBox(objects, splitIndex, right_index));
			nodes.push_back(left);
			nodes.push_back(right);

			// Recurse
			build_recursive(left_index, splitIndex, left);
			build_recursive(splitIndex, right_index, right);
		}

	}

	int GetLargestAxis(vector<Object*>& objs, int ind1, int ind2)
	{
		// TODO: Check if you can just find longest axis on bounding box
		Vec3 Min = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
		Vec3 Max = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		for (Object* obj : objs)
		{
			Vec3 centroid = obj->GetBoundingBox().centroid();
			if (Min.x > centroid.x) Min.x = centroid.x;
			if (Min.y > centroid.y) Min.y = centroid.y;
			if (Min.z > centroid.z) Min.z = centroid.z;

			if (Max.x < centroid.x) Max.x = centroid.x;
			if (Max.y < centroid.y) Max.y = centroid.y;
			if (Max.z < centroid.z) Max.z = centroid.z;
		}
		Vec3 diff = Max - Min;
		float maxLength = MAX3(diff.x, diff.y, diff.z);
		if (diff.x == maxLength)
			return 0;
		else if (diff.y == maxLength)
			return 1;
		else
			return 2;
	}

	int GetSplitIndex(vector<Object*>& objs, int ind1, int ind2, Vec3 midPoint, int axis)
	{
		for (int i = ind1; i < ind2; i++)
		{
			switch (axis)
			{
			case(0):	if (objs[i]->GetBoundingBox().centroid().x > midPoint.x) return i;
			case(1):	if (objs[i]->GetBoundingBox().centroid().y > midPoint.y) return i;
			default:	if (objs[i]->GetBoundingBox().centroid().z > midPoint.z) return i;
			}
		}
	}

	AABB CalculateBoundingBox(vector<Object*>& objs, int ind1, int ind2)
	{
		AABB result;
		for (int i = ind1; i < ind2; i++) {
			AABB bbox = objs[i]->GetBoundingBox();
			result.extend(bbox);
		}
		return result;
	}

#pragma endregion Build Functions


	bool BVH::Traverse(Ray& ray, Object** hit_obj, Vec3& hit_point) {
		float tmp;
		float tmin = FLT_MAX;  //contains the closest primitive intersection
		bool hit = false;

		BVHNode* currentNode = nodes[0];

		//PUT YOUR CODE HERE
	}

	bool BVH::Traverse(Ray& ray) {  //shadow ray with length
		float tmp;

		//double length = ray.Direction.length(); //distance between light and intersection point
		//ray.Direction.normalize();

		//PUT YOUR CODE HERE
	}
}