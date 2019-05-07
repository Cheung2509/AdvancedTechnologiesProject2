#include "BoundingVolumeHierachy.h"

#include <algorithm>
#include <stack>
#include <iostream>
#include <functional>

#include "BVHNode.h"
#include "Geometry.h"
#include "Model.h"
#include "Ray.h"

void BVH::buildBVH(std::vector<std::shared_ptr<Geometry>> shapes)
{
	auto start = std::chrono::steady_clock::now();

	for (auto& obj : shapes)
	{
		if (obj->getGeometryType() == GeometryType::PRIMITIVE)
		{
			m_shapes.emplace_back(obj);
		}
		else if (obj->getGeometryType() == GeometryType::MESH)
		{
			auto temp = std::static_pointer_cast<Model>(obj);

			for (auto& triangle : temp->getTriangles())
			{
				m_shapes.emplace_back(triangle);
			}
		}
	}

	m_rootNode = std::make_shared<BVHNode>();

	glm::vec3 min(kInfinity);
	glm::vec3 max(-kInfinity);
	
	Axis axis = Axis::X;
	ComparePrimitives cmp(static_cast<Axis>(axis));
	std::sort(m_shapes.begin(), m_shapes.end(), cmp);

	for (auto& obj : shapes)
	{
		min = glm::min(obj->getBox().getMin(), min);
		max = glm::max(obj->getBox().getMax(), max);
	}

	AABB worldBox(min, max);
	m_rootNode->setBounds(worldBox);
	m_rootNode->createNode(0, m_shapes.size());
	buildRecursiveBVH(0, m_shapes.size(), m_rootNode, 0, axis);

	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<float> time = end - start;

	std::cout << "Time to build BVH: " << time.count() << std::endl;
	std::cout << "Number of nodes in BVH: " << m_numberOfNodes << std::endl;
}

void BVH::buildRecursiveBVH(int leftIndex, int rightIndex, std::shared_ptr<BVHNode> node, int depth, Axis& axis)
{
	if ((rightIndex - leftIndex) <= 4 || depth > 50)
	{
		node->createLeaf(leftIndex, rightIndex);
		m_numberOfNodes++;
	}
	else
	{
		glm::vec3 indexPos = (node->getBounds().getMin() + node->getBounds().getMax()) / 2.0f;
		int splitIndex = 0;

		for (int i = leftIndex; i < rightIndex; i++)
		{
			switch (axis)
			{
			case Axis::X:
				if (m_shapes[i]->getPos().x > indexPos.x)
				{
					splitIndex = i;
					break;
				}
				break;
			case Axis::Y:
				if (m_shapes[i]->getPos().y > indexPos.y)
				{
					splitIndex = i;
					break;
				}
				break;
			case Axis::Z:
				if (m_shapes[i]->getPos().z > indexPos.z)
				{
					splitIndex = i;
					break;
				}
				break;
			}
		}

		auto leftNode = std::make_shared<BVHNode>();
		glm::vec3 min(kInfinity);
		glm::vec3 max(-kInfinity);
		int numberOfObj = 0;

		ComparePrimitives cmp(axis);
		std::sort(m_shapes.begin() + leftIndex, m_shapes.begin() + splitIndex, cmp);
		for (int i = leftIndex; i < splitIndex; i++)
		{
			min = glm::min(m_shapes[i]->getBox().getMin(), min);
			max = glm::max(m_shapes[i]->getBox().getMax(), max);

			numberOfObj++;
		}
		leftNode->setBounds(AABB(min, max));


		//Calculate Right node 
		auto rightNode = std::make_shared<BVHNode>();
		glm::vec3 min2(kInfinity);
		glm::vec3 max2(-kInfinity);
		
		std::sort(m_shapes.begin() + splitIndex, m_shapes.begin() + rightIndex, cmp);
		for (int i = splitIndex; i < rightIndex; i++)
		{
			min2 = glm::min(m_shapes[i]->getBox().getMin(), min);
			max2 = glm::max(m_shapes[i]->getBox().getMax(), max);

			numberOfObj++;
		}

		rightNode->setBounds(AABB(min2, max2));

		node->setLeft(leftNode);
		node->setRight(rightNode);

		node->createNode(leftIndex, rightIndex - leftIndex);

		buildRecursiveBVH(leftIndex, splitIndex, node->getLeft(), depth + 1, axis);
		buildRecursiveBVH(splitIndex, rightIndex, node->getRight(), depth + 1, axis);
		m_numberOfNodes++;
	}
}

void BVH::buildSAH(std::vector<std::shared_ptr<Geometry>> shapes)
{
	auto start = std::chrono::steady_clock::now();

	for (auto& obj : shapes)
	{
		if (obj->getGeometryType() == GeometryType::PRIMITIVE)
		{
			m_shapes.emplace_back(obj);
		}
		else if (obj->getGeometryType() == GeometryType::MESH)
		{
			auto temp = std::static_pointer_cast<Model>(obj);

			for (auto& triangle : temp->getTriangles())
			{
				m_shapes.emplace_back(triangle);
			}
		}
	}

	m_rootNode = std::make_shared<BVHNode>();

	glm::vec3 min(kInfinity);
	glm::vec3 max(-kInfinity);
	for (auto& obj : shapes)
	{
		min = glm::min(obj->getBox().getMin(), min);
		max = glm::max(obj->getBox().getMax(), max);
	}

	AABB worldBox(min, max);
	m_rootNode->setBounds(worldBox);
	m_rootNode->createNode(0, m_shapes.size());
	buildRecursiveSAH(0, m_shapes.size(), m_rootNode, 0);
	
	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<float> time = end - start;

	std::cout << "Time to build SAH: " << time.count() << std::endl;
	std::cout << "Number of nodes in SAH: " << m_numberOfNodes << std::endl;
}

void BVH::buildRecursiveSAH(const int& leftIndex, const int& rightIndex, std::shared_ptr<BVHNode> node, const int& depth)
{

	if ((rightIndex - leftIndex) <= 4 || depth > 200)
	{
		//If there are less than 4 object or depth has gone too deep create a leaf
		node->createLeaf(leftIndex, rightIndex - leftIndex);
		m_numberOfNodes++;
	}
	else
	{
		//Cost of current node
		float bestCost = 1.0f + node->getBounds().getSurfaceArea() * 2.0f * (rightIndex - leftIndex);

		float bestSplit = kInfinity;

		Axis bestAxis = Axis::NONE;

		int left;
		int right;

		AABB lBox(glm::vec3(kInfinity), glm::vec3(-kInfinity));
		AABB rBox(glm::vec3(kInfinity), glm::vec3(-kInfinity));

		for (int j = 0; j < 3; j++)
		{
			//Get the axis
			Axis axis = static_cast<Axis>(j);

			float start, stop, step;

			//Get the bounds of each axis
			switch (axis)
			{
			case Axis::X:
				start = node->getBounds().getMin().x;
				stop = node->getBounds().getMax().x;
				break;
			case Axis::Y:
				start = node->getBounds().getMin().y;
				stop = node->getBounds().getMax().y;
				break;
			case Axis::Z:
				start = node->getBounds().getMin().z;
				stop = node->getBounds().getMax().z;
				break;
			}

			//if start stop is more than 1e-4
			if (glm::abs(stop - start) < 1e-4)
				continue;

			//Calculate step for test splits
			step = (stop - start) / (1024.0f / (float)(depth + 1));

			//loop over test splits
			for (float testSplit = start + step; testSplit < stop - step; testSplit += step)
			{
				int lCount = 0;
				int rCount = 0;

				for (int i = leftIndex; i < rightIndex; i++)
				{
					float value;

					//Get position of an axis
					switch (axis)
					{
					case Axis::X:
						value = m_shapes[i]->getPos().x;
						break;
					case Axis::Y:
						value = m_shapes[i]->getPos().y;
						break;
					case Axis::Z:
						value = m_shapes[i]->getPos().z;
						break;
					}

					//apply box of left and right.
					if (value < testSplit)
					{
						lBox.setMin(glm::min(lBox.getMin(), m_shapes[i]->getBox().getMin()));
						lBox.setMax(glm::max(lBox.getMax(), m_shapes[i]->getBox().getMax()));
						lCount++;
					}
					else
					{
						rBox.setMin(glm::min(rBox.getMin(), m_shapes[i]->getBox().getMin()));
						rBox.setMax(glm::max(rBox.getMax(), m_shapes[i]->getBox().getMax()));
						rCount++;
					}
				}

				//Continue
				if (lCount <= 1 || rCount <= 1)
					continue;

				//Cost of split
				float totalCost = 1.0f + ((lBox.getSurfaceArea() / rBox.getSurfaceArea()) *lCount * 2) +
					((rBox.getSurfaceArea() / lBox.getSurfaceArea()) *lCount * 2);
				
				//if the cost is less than best cost, replace it
				if (totalCost < bestCost)
				{
					bestCost = totalCost;
					bestSplit = testSplit;
					bestAxis = axis;

					left = lCount;
					right = rCount;
				}
			}
		}

		//If there are no suitible axis then create a leaf
		if (bestAxis == Axis::NONE)
		{
			node->createLeaf(leftIndex, rightIndex - leftIndex);
			m_numberOfNodes++;
			return;
		}

		//Initialize as node
		node->createNode(leftIndex, rightIndex - leftIndex);
		m_numberOfNodes++;

		//Create left node
		node->setLeft(std::make_shared<BVHNode>());
		node->getLeft()->setBounds(lBox);
		std::thread t1(&BVH::buildRecursiveSAH, this, leftIndex, left, node->getLeft(), depth + 1);
		//buildRecursiveSAH(leftIndex, left, node->getLeft(), depth + 1);

		//Create right node
		node->setRight(std::make_shared<BVHNode>());
		node->getRight()->setBounds(rBox);
		//buildRecursiveSAH(left, rightIndex, node->getRight(), depth + 1);
		std::thread t2(&BVH::buildRecursiveSAH, this, left, rightIndex, node->getRight(), depth + 1);

		t1.join();
		t2.join();
	}
}

bool BVH::checkIntersection(Ray* ray, Geometry** hitObj, std::uint64_t & index, glm::vec2 & uv)
{
	std::stack<StackItem> stackItems;
	
	BVHNode* currentNode = m_rootNode.get();
	float t = kInfinity;

	do
	{
		//If ray collidaes with teh curren node
		if (currentNode->getBounds().checkRayCollision(ray))
		{
			if (!currentNode->isLeaf())
			{
				//If it is not leaf then check intersection of child nodes 
				float t1 = kInfinity;
				float t2 = kInfinity;

				bool leftIntersect = currentNode->getLeft()->getBounds().checkRayCollision(ray, t1);
				bool rightIntersect = currentNode->getBounds().checkRayCollision(ray, t2);

				//if both intersect with ray
				if (leftIntersect && rightIntersect)
				{
					
					if (t1 > t2)
					{
						//add left first then right node into stack
						StackItem farthest;
						farthest.ptr = currentNode->getRight().get();
						farthest.t = t2;
						stackItems.push(farthest);

						StackItem nearest;
						nearest.ptr = currentNode->getLeft().get();
						nearest.t = t1;
						stackItems.push(nearest);


					}
					else
					{
						//add right first then left node into stack
						StackItem farthest;
						farthest.ptr = currentNode->getLeft().get();
						farthest.t = t1;
						stackItems.push(farthest);

						StackItem nearest;
						nearest.ptr = currentNode->getRight().get();
						nearest.t = t2;
						stackItems.push(nearest);
					}
				}

				//Add only left if no itersection on right
				if (leftIntersect && !rightIntersect)
				{
					StackItem node;
					node.ptr = currentNode->getLeft().get();
					node.t = t1;
					stackItems.push(node);
				}

				//Add only right if no itersection on left
				if (rightIntersect && !leftIntersect)
				{
					StackItem node;
					node.ptr = currentNode->getRight().get();
					node.t = t2;
					stackItems.push(node);
				}
			}
			else
			{
				//If a leaf loop through all geometry in the node
				for (int i = currentNode->getIndex(); i < currentNode->getIndex() + currentNode->getNumberOfObjects(); i++)
				{
					float distance = kInfinity;
					
					//Check AABB collision first
					if (m_shapes[i]->getBox().checkRayCollision(ray))
					{
						//Check for collistion
						if (m_shapes[i]->intersect(ray, index, uv, distance))
						{
							//If distance is less than closest shape hit by ray , replace it
							if (distance < t)
							{
								t = distance;
								*hitObj = m_shapes[i].get();
							}
						}
					}
				}
			}
		}

		//if not empty go to the next node
		if (!stackItems.empty())
		{
			currentNode = stackItems.top().ptr;
			stackItems.pop();
		}
	} while (!stackItems.empty());
	

	return (hitObj != nullptr);
}
