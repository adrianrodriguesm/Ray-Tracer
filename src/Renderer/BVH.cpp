#include "pch.h"
#include "BVH.h"

namespace rayTracer
{
	BVH::BVHNode::BVHNode() 
	{
		this->leaf = false;
		this->index = 0;
		this->n_objs = 0;
	}

	void BVH::BVHNode::SetAABB(AABB& bbox_) { this->bbox = bbox_; }

	void BVH::BVHNode::MakeLeaf(unsigned int index_, unsigned int n_objs_) 
	{
		this->leaf = true;
		this->index = index_;
		this->n_objs = n_objs_;
	}

	void BVH::BVHNode::MakeNode(unsigned int left_index_) 
	{
		this->leaf = false;
		this->index = left_index_;
		this->n_objs = 0; 
	}
#pragma region Build Functions

	int GetLargestAxis(const std::vector<Object*>& objs, int ind1, int ind2)
	{
		Vec3 Min = FLOAT_MAX;
		Vec3 Max = FLOAT_MIN;

		for (int i = ind1; i < ind2; i++)
		{
			Vec3 centroid = objs[i]->GetBoundingBox().Centroid();
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
		float maxLength = std::max({ diff.x, diff.y, diff.z });

		if (diff.x == maxLength)
			return 0;
		else if (diff.y == maxLength)
			return 1;
		else
			return 2;
	}

	int GetSplitIndex(const std::vector<Object*>& objs, int ind1, int ind2, const Vec3& midPoint, int axis)
	{
		for (int i = ind1; i < ind2; i++)
		{
			Vec3 centroid = objs[i]->GetBoundingBox().Centroid();
			switch (axis)
			{
			case(0):	
				if (centroid.x > midPoint.x)
					return i;
				break;
			case(1):	
				if (centroid.y > midPoint.y)
					return i;
				break;
			case(2):	
				if (centroid.z > midPoint.z)
					return i;
				break;
			}
		}
	}

	AABB CalculateBoundingBox(const std::vector<Object*>& objs, int ind1, int ind2)
	{
		AABB result;
		result.Min = FLOAT_MAX;
		result.Max = FLOAT_MIN;
		for (int i = ind1; i < ind2; i++) 
		{
			AABB bbox = objs[i]->GetBoundingBox();
			result.Extend(bbox);
		}
		return result;
	}

	BVH::~BVH()
	{
		m_Objects.clear();
		for (auto bvhNode : m_BVHNodes)
		{
			delete bvhNode;
		}
		m_BVHNodes.clear();
	}

	void BVH::Build(std::vector<Object*>& objects)
	{
		BVHNode* root = new BVHNode();

		Vec3 min = FLOAT_MAX;
		Vec3 max = FLOAT_MIN;
		AABB world_bbox = AABB(min, max);
		m_Objects.reserve(objects.size());
		for (Object* obj : objects)
		{
			AABB bbox = obj->GetBoundingBox();
			world_bbox.Extend(bbox);
			m_Objects.push_back(obj);
		}
		world_bbox.Min.x -= EPSILON; world_bbox.Min.y -= EPSILON; world_bbox.Min.z -= EPSILON;
		world_bbox.Max.x += EPSILON; world_bbox.Max.y += EPSILON; world_bbox.Max.z += EPSILON;
		root->SetAABB(world_bbox);
		m_BVHNodes.push_back(root);
		BuildRecursive(0, m_Objects.size(), root, 0);
	}

	void BVH::BuildRecursive(int leftIndex, int rightIndex, BVHNode* node, uint32_t depth)
	{

		if ((rightIndex - leftIndex) <= MinLeafPrimitives || depth >= MaxDepth)
		{
			node->MakeLeaf(leftIndex, rightIndex - leftIndex);
			return;
		}
		else 
		{
			// Current node index is equal to the left child node index
			node->MakeNode(m_BVHNodes.size());

			// Sort objects by largest axis
			Comparator cmp = Comparator();
			int largestAxis = GetLargestAxis(m_Objects, leftIndex, rightIndex);
			cmp.Dimension = largestAxis;
			// Sort the elements along the largest axis
			std::sort(m_Objects.begin() + leftIndex, m_Objects.begin() + rightIndex, cmp);
			
			// Find split
			Vec3 midPoint = node->GetAABB().Centroid();
			int splitIndex = GetSplitIndex(m_Objects, leftIndex, rightIndex, midPoint, largestAxis);

			// If one side is empty - use median split
			if (splitIndex <= leftIndex || splitIndex >= rightIndex)
			{
				splitIndex = leftIndex + (rightIndex - leftIndex) / 2;
			}

			// Create child nodes
			BVHNode* left = new BVHNode();
			BVHNode* right = new BVHNode();
			left->SetAABB(CalculateBoundingBox(m_Objects, leftIndex, splitIndex));
			right->SetAABB(CalculateBoundingBox(m_Objects, splitIndex, rightIndex));
			m_BVHNodes.push_back(left);
			m_BVHNodes.push_back(right);

			// Recurse
			depth++;
			BuildRecursive(leftIndex, splitIndex, left, depth);
			BuildRecursive(splitIndex, rightIndex, right, depth);
		}

	}
#pragma endregion Build Functions

#pragma region Interception Functions
	static constexpr float F32_HITEPSILON = 1.0e-5f;
	RayCastHit BVH::Intercepts(Ray& ray)
	{
		RayCastHit tempHit;
		RayCastHit closestHit;
		closestHit.Tdist = FLOAT_MAX;
		closestHit.Hit = false;


		BVHNode* currentNode = m_BVHNodes[0];

		// Check hit world box
		if (!currentNode->GetAABB().Intercepts(ray, tempHit.Tdist))
			return RayCastHit(false);


		// Traverse the BHV
		while (true)
		{
			if (!currentNode->IsLeaf())
			{
				// Check hit child nodes
				BVHNode* leftNode = m_BVHNodes[currentNode->GetIndex()];
				float tLeft;
				bool leftHit = (leftNode->GetAABB().Intercepts(ray, tLeft) && tLeft < closestHit.Tdist);

				BVHNode* rightNode = m_BVHNodes[currentNode->GetIndex() + 1.0];
				float tRight;
				bool rightHit = (rightNode->GetAABB().Intercepts(ray, tRight) && tRight < closestHit.Tdist);

				if (leftHit && rightHit)
				{
					// Both hit - stack furthest node for later processing
					if (tLeft <= tRight)
					{
						m_HitStack.push(StackItem(rightNode, tRight));
						currentNode = leftNode;
						continue;
					}
					else
					{
						m_HitStack.push(StackItem(leftNode, tLeft));
						currentNode = rightNode;
						continue;
					}
				}
				else if (leftHit)
				{
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
				// No hit - Go to stack popping
			}
			else
			{
				// Leaf Node - Process objects
				for (int i = currentNode->GetIndex(); i < (currentNode->GetIndex() + currentNode->GetNumObjs()); i++)
				{
					RayCastHit tempHit = m_Objects[i]->Intercepts(ray);
					if (tempHit && tempHit.Tdist < closestHit.Tdist)
						closestHit = tempHit;
				}

			}

			// Stack popping
			currentNode = nullptr;
			while (!m_HitStack.empty())
			{
				StackItem stackNode = m_HitStack.top();
				m_HitStack.pop();
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

		BVHNode* currentNode = m_BVHNodes[0];

		// Check hit world box
		if (!currentNode->GetAABB().Intercepts(ray, tempHit.Tdist))
			return false;


		while (true)
		{
			if (!currentNode->IsLeaf())
			{
				// Check hit child nodes
				BVHNode* leftNode = m_BVHNodes[currentNode->GetIndex()];
				float tLeft;
				bool leftHit = (leftNode->GetAABB().Intercepts(ray, tLeft) && tLeft < lightDist);

				BVHNode* rightNode = m_BVHNodes[currentNode->GetIndex() + 1.0];
				float tRight;
				bool rightHit = (rightNode->GetAABB().Intercepts(ray, tRight) && tRight < lightDist);

				if (leftHit && rightHit)
				{
					// Both hit - stack furthest node for later processing
					if (tLeft <= tRight)
					{
						m_HitStack.push(StackItem(rightNode, tRight));
						currentNode = leftNode;
						continue;
					}
					else 
					{
						m_HitStack.push(StackItem(leftNode, tLeft));
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
				for (int i = currentNode->GetIndex(); i < (currentNode->GetIndex() + currentNode->GetNumObjs()); i++)
				{
					if (m_Objects[i]->GetMaterial()->GetTransmittance() > 0)
						continue; // Transparent object

					RayCastHit tempHit = m_Objects[i]->Intercepts(ray);
					if (tempHit && tempHit.Tdist < lightDist)
						return true;
				}

			}

			// Stack popping
			currentNode = nullptr;
			while (!m_HitStack.empty())
			{
				StackItem stackNode = m_HitStack.top();
				m_HitStack.pop();
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
		/**/
		return false;
	}

#pragma endregion Interception Functions
}