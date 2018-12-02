#include "BoundingVolumeHierachy.h"

#include <algorithm>
#include <stack>
#include <iostream>

#include "BVHNode.h"
#include "Geometry.h"
#include "Ray.h"

void BVH::buildBVH(std::vector<std::shared_ptr<Geometry>> shapes)
{
	auto start = std::chrono::steady_clock::now();
	m_shapes = shapes;

	m_rootNode = std::make_shared<BVHNode>();

	glm::vec3 min(0);
	glm::vec3 max(0);
	
	Axis axis = Axis::X;
	ComparePrimitives cmp(static_cast<Axis>(axis));
	std::sort(m_shapes.begin(), m_shapes.end(), cmp);

	for (auto& obj : shapes)
	{
		min = glm::min(obj->getBox().getMin(), min);
		max =glm::max(obj->getBox().getMax(), max);
	}

	AABB worldBox(min, max);
	m_rootNode->setBounds(worldBox);
	m_rootNode->createNode(0, m_shapes.size());
	buildRecursiveBVH(0, m_shapes.size(), m_rootNode, 0, axis);

	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<float> time = end - start;

	std::cout << "Time to build BVH: " << time.count() << std::endl;
}

void BVH::buildRecursiveBVH(int leftIndex, int rightIndex, std::shared_ptr<BVHNode>& node, int depth, Axis& axis)
{
	if ((rightIndex - leftIndex) <= 4)
	{
		node->createLeaf(leftIndex, rightIndex);
	}
	else
	{
		float indexPos = (node->getBounds().getMin().x + node->getBounds().getMax().x) / 2;
		int splitIndex = 0;

		for (int i = leftIndex; i < rightIndex; i++)
		{
			if (m_shapes[i]->getPos().x > indexPos)
			{
				splitIndex = i;
				break;
			}
		}

		auto leftNode = std::make_shared<BVHNode>();
		glm::vec3 min = m_shapes[leftIndex]->getBox().getMin();
		glm::vec3 max = m_shapes[leftIndex]->getBox().getMax();;
		int numberOfObj = 0;

		switch (axis)
		{
		case Axis::X:
			axis = Axis::Y;
			break;
		case Axis::Y:
			axis = Axis::Z;
			break;
		case Axis::Z:
			axis = Axis::X;
			break;
		}

		ComparePrimitives cmp(axis);
		std::sort(m_shapes.begin() + leftIndex, m_shapes.begin() + splitIndex, cmp);
		for (int i = leftIndex; i < splitIndex; i++)
		{
			min = glm::min(m_shapes[i]->getBox().getMin(), min);
			max = glm::max(m_shapes[i]->getBox().getMax(), max);

			numberOfObj++;
		}
		leftNode->setBounds(AABB(min, max));
		leftNode->createNode(leftIndex, numberOfObj);


		//Calculate Right node 
		auto rightNode = std::make_shared<BVHNode>();
		min = m_shapes[splitIndex]->getBox().getMin();
		max = m_shapes[splitIndex]->getBox().getMax();;


		std::sort(m_shapes.begin() + splitIndex, m_shapes.begin() + rightIndex, cmp);
		for (int i = splitIndex; i < rightIndex; i++)
		{
			min = glm::min(m_shapes[i]->getBox().getMin(), min);
			max = glm::max(m_shapes[i]->getBox().getMax(), max);

			numberOfObj++;
		}

		rightNode->setBounds(AABB(min, max));
		rightNode->createNode(splitIndex, rightIndex);

		node->setLeft(leftNode);
		node->setRight(rightNode);



		buildRecursiveBVH(leftIndex, splitIndex, leftNode, depth + 1, axis);
		buildRecursiveBVH(splitIndex, rightIndex, rightNode, depth + 1, axis);
	}
}

void BVH::buildSAH(std::vector<std::shared_ptr<Geometry>> shapes)
{
	auto start = std::chrono::steady_clock::now();

	m_shapes = shapes;
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
}

void BVH::buildRecursiveSAH(int leftIndex, int rightIndex, std::shared_ptr<BVHNode> node, int depth)
{
	if ((rightIndex - leftIndex) <= 4)
	{
		node->createLeaf(leftIndex, rightIndex - leftIndex);
	}
	else
	{
		float mincost = (rightIndex - leftIndex) * (node->getBounds().getSurfaceArea());

		float bestSplit = kInfinity;

		Axis bestAxis = Axis::NONE;

		for (int j = 0; j < 3; j++)
		{
			Axis axis = static_cast<Axis>(j);

			float start, stop, step;

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

			if (glm::abs(stop - start) < 1e-4)
				continue;

			step = (stop - start) / (1024.0f / (float)(depth + 1));


			glm::vec3 lMin(kInfinity);
			glm::vec3 lMax(-kInfinity);

			glm::vec3 rMin(kInfinity);
			glm::vec3 rMax(-kInfinity);

			int lCount = 0;
			int rCount = 0;

			for (float testSplit = start + step; testSplit < stop - step; testSplit += step)
			{


				for (int i = leftIndex; i < rightIndex; i++)
				{
					float value;

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
					if (value < testSplit)
					{
						lMin = glm::min(lMin, m_shapes[i]->getBox().getMin());
						lMax = glm::max(lMax, m_shapes[i]->getBox().getMax());
						lCount++;
					}
					else
					{
						rMin = glm::min(rMin, m_shapes[i]->getBox().getMin());
						rMax = glm::max(rMax, m_shapes[i]->getBox().getMax());
						rCount++;
					}
				}

				if (lCount <= 1 || rCount <= 1)
					continue;

				AABB leftBox(lMin, lMax);
				AABB rightBox(rMin, rMax);

				float totalCost = leftBox.getSurfaceArea() * lCount + rightBox.getSurfaceArea() * rCount;

				if (totalCost < mincost)
				{
					mincost = totalCost;
					bestSplit = testSplit;
					bestAxis = axis;
				}
			}
		}

		if (bestAxis == Axis::NONE)
		{
			node->createLeaf(leftIndex, rightIndex - leftIndex);
			return;
		}

		glm::vec3 lMin(kInfinity);
		glm::vec3 lMax(-kInfinity);

		glm::vec3 rMin(kInfinity);
		glm::vec3 rMax(-kInfinity);

		int splitIndex = leftIndex;

		for (int i = leftIndex; i < rightIndex; i++)
		{
			float value;

			switch (bestAxis)
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

			if (value < bestSplit)
			{
				lMin = glm::min(lMin, m_shapes[i]->getBox().getMin());
				lMax = glm::max(lMax, m_shapes[i]->getBox().getMax());
				splitIndex++;
			}
			else
			{
				rMin = glm::min(rMin, m_shapes[i]->getBox().getMin());
				rMax = glm::max(rMax, m_shapes[i]->getBox().getMax());
			}
		}

		node->createNode(leftIndex, rightIndex - leftIndex);

		node->setLeft(std::make_shared<BVHNode>());
		node->getLeft()->setBounds(AABB(lMin, lMax));
		buildRecursiveSAH(leftIndex, splitIndex, node->getLeft(), depth + 1);

		node->setRight(std::make_shared<BVHNode>());
		node->getRight()->setBounds(AABB(rMin, rMax));
		buildRecursiveSAH(splitIndex, rightIndex, node->getRight(), depth + 1);
	}
}

bool BVH::checkIntersection(Ray* ray, Geometry** hitObj, std::uint64_t & index, glm::vec2 & uv)
{
	std::stack<StackItem> stackItems;
	
	BVHNode* currentNode = m_rootNode.get();
	float t = kInfinity;

	do
	{
		if (currentNode->getBounds().checkRayCollision(ray))
		{
			if (!currentNode->isLeaf())
			{
				float t1 = kInfinity;
				float t2 = kInfinity;

				bool leftIntersect = currentNode->getLeft()->getBounds().checkRayCollision(ray, t1);
				bool rightIntersect = currentNode->getBounds().checkRayCollision(ray, t2);

				if (leftIntersect && rightIntersect)
				{
					if (t1 > t2)
					{
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

				if (leftIntersect && !rightIntersect)
				{
					StackItem node;
					node.ptr = currentNode->getLeft().get();
					node.t = t1;
					stackItems.push(node);
				}

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
				for (int i = currentNode->getIndex(); i < currentNode->getIndex() + currentNode->getNumberOfObjects(); i++)
				{
					float distance = kInfinity;
					if (m_shapes[i]->intersect(ray, index, uv, distance))
					{
						if (distance < t)
						{
							t = distance;
							*hitObj = m_shapes[i].get();
						}
					}
				}
			}
		}

		if (!stackItems.empty())
		{
			currentNode = stackItems.top().ptr;
			stackItems.pop();
		}
	} while (!stackItems.empty());
	

	return (hitObj != nullptr);
}
