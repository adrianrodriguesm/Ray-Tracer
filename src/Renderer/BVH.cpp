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

	int GetLargestAxis(vector<Object*>& objs, int ind1, int ind2)
	{
		Vec3 Min = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
		Vec3 Max = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (int i = ind1; i < ind2; i++)
		{
			Vec3 centroid = objs[i]->GetBoundingBox().centroid();
			// Min
			if (Min.x > centroid.x) 
				Min.x = centroid.x;

			if (Min.y > centroid.y) 
				Min.y = centroid.y;

			if (Min.z > centroid.z) 
				Min.z = centroid.z;

			// Max
			if (Max.x < centroid.x) 
				Max.x = centroid.x;

			if (Max.y < centroid.y) 
				Max.y = centroid.y;

			if (Max.z < centroid.z) 
				Max.z = centroid.z;
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

	int GetSplitIndex(const vector<Object*>& objs, int ind1, int ind2, const Vec3& midPoint, int axis)
	{
		for (int i = ind1; i < ind2; i++)
		{
			switch (axis)
			{
			case(0):	
				if (objs[i]->GetBoundingBox().centroid().x > midPoint.x) 
					return i;
				break;
			case(1):	
				if (objs[i]->GetBoundingBox().centroid().y > midPoint.y) 
					return i;
				break;
			case(2):	
				if (objs[i]->GetBoundingBox().centroid().z > midPoint.z) 
					return i;
				break;
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

			// If one side is empty - use median split
			if (splitIndex == left_index || splitIndex == right_index)
			{
				splitIndex = left_index + (right_index - left_index) / 2;
			}

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
#pragma endregion Build Functions

#pragma region Interception Functions

	RayCastHit BVH::Intercepts(Ray& ray) {
		RayCastHit tempHit;
		RayCastHit closestHit;
		closestHit.Tdist = FLT_MAX;
		closestHit.Hit = false;


		BVHNode* currentNode = nodes[0];

		// Check hit world box
		if (!currentNode->getAABB().intercepts(ray, tempHit.Tdist))
			return RayCastHit(false);


		while (true)
		{
			if (!currentNode->isLeaf())
			{
				// Check hit child nodes
				BVHNode* leftNode = nodes[currentNode->getIndex()];
				float tLeft;
				bool leftHit = (leftNode->getAABB().intercepts(ray, tLeft));//&& tLeft < closestHit.Tdist);

				BVHNode* rightNode = nodes[currentNode->getIndex() + 1.0];
				float tRight;
				bool rightHit = (rightNode->getAABB().intercepts(ray, tRight));//&& tRight < closestHit.Tdist);

				if (leftHit && rightHit)
				{
					// Both hit - stack furthest node for later processing
					if (tLeft <= tRight)
					{
						hit_stack.push(StackItem(rightNode, tRight));
						currentNode = leftNode;
						continue;
					}
					else {
						hit_stack.push(StackItem(leftNode, tLeft));
						currentNode = rightNode;
						continue;
					}
				}
				else if(leftHit){
					// Only left node hit
					currentNode = leftNode;
					continue;
				}
				else if (rightHit)
				{
					// Only right node hit
					currentNode = rightNode;
					continue;
				}
				else
				{
					// No hit - Go to stack popping
				}
			}
			else {
				// Leaf Node - Process objects
				for (int i = currentNode->getIndex(); i < (currentNode->getIndex() + currentNode->getNObjs()); i++)
				{
					RayCastHit tempHit = objects[i]->Intercepts(ray);
					if (tempHit && tempHit.Tdist < closestHit.Tdist)
						closestHit = tempHit;
				}

			}

			// Stack popping
			currentNode = nullptr;
			while (!hit_stack.empty())
			{
				StackItem stackNode = hit_stack.top();
				hit_stack.pop();
				if (stackNode.t < closestHit.Tdist)
				{
					currentNode = stackNode.ptr;
					break;
				}
			}

			if (currentNode == nullptr)
				break; // Termination
		}
		return closestHit; 
	}

	bool BVH::InterceptsShadows(Ray& ray, float lightDist)  //shadow ray with length
	{ 
		RayCastHit tempHit;

		BVHNode* currentNode = nodes[0];

		// Check hit world box
		if (!currentNode->getAABB().intercepts(ray, tempHit.Tdist))
			return false;


		while (true)
		{
			if (!currentNode->isLeaf())
			{
				// Check hit child nodes
				BVHNode* leftNode = nodes[currentNode->getIndex()];
				float tLeft;
				bool leftHit = (leftNode->getAABB().intercepts(ray, tLeft) && tLeft < lightDist);

				BVHNode* rightNode = nodes[currentNode->getIndex() + 1.0];
				float tRight;
				bool rightHit = (rightNode->getAABB().intercepts(ray, tRight) && tRight < lightDist);

				if (leftHit && rightHit)
				{
					// Both hit - stack furthest node for later processing
					if (tLeft <= tRight)
					{
						hit_stack.push(StackItem(rightNode, tRight));
						currentNode = leftNode;
						continue;
					}
					else {
						hit_stack.push(StackItem(leftNode, tLeft));
						currentNode = rightNode;
						continue;
					}
				}
				else if (leftHit) {
					// Only left node hit
					currentNode = leftNode;
					continue;
				}
				else if (rightHit)
				{
					// Only right node hit
					currentNode = rightNode;
					continue;
				}
				else
				{
					// No hit - Goto stack popping
				}
			}
			else {
				// Leaf Node - Process objects
				for (int i = currentNode->getIndex(); i < (currentNode->getIndex() + currentNode->getNObjs()); i++)
				{
					if(objects[i]->GetMaterial()->GetTransmittance() > 0)
						continue; // Transparent object

					RayCastHit tempHit = objects[i]->Intercepts(ray);
					if (tempHit && tempHit.Tdist < lightDist)
						return true;
				}

			}

			// Stack popping
			currentNode = nullptr;
			while (!hit_stack.empty())
			{
				StackItem stackNode = hit_stack.top();
				hit_stack.pop();
				if (stackNode.t < lightDist)
				{
					currentNode = stackNode.ptr;
					break;
				}
				else
					currentNode = nullptr;
			}
			if (currentNode == nullptr)
				break; // Termination
		}
		return false;
	}

#pragma endregion Interception Functions
}