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

void BVH::buildRecursiveBVH(int leftIndex, int rightIndex, std::shared_ptr<BVHNode> node, int depth, Axis& axis)
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

	glm::vec3 min(0);
	glm::vec3 max(0);

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
	buildRecursiveSAH(0, m_shapes.size(), m_rootNode, 0, axis);


	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<float> time = end - start;

	std::cout << "Time to build SAH: " << time.count() << std::endl;
}

void BVH::buildRecursiveSAH(int leftIndex, int rightIndex, std::shared_ptr<BVHNode> node, int depth, Axis& axis)
{
	if ((rightIndex - leftIndex) <= 4)
	{
		node->createLeaf(leftIndex, rightIndex);
	}
	else
	{
		float pSA = node->getBounds().getSurfaceArea();
		float pCI = node->getNumberOfObjects() * pSA * 1.0f;

		float ci;

		int splitIndex = 0;

		auto leftNode = std::make_shared<BVHNode>();
		AABB leftBounds;
		auto rightNode = std::make_shared<BVHNode>();
		AABB rightBounds;

		Axis axis = Axis::X;

		float cost = 0.0f;

		do
		{
			int rNumberOfObj = 0;
			int lNumberOfObj = 0;

			glm::vec3 lmin;
			glm::vec3 lmax;
			glm::vec3 rmin;
			glm::vec3 rmax;

			ComparePrimitives cmp(static_cast<Axis>(axis));

			std::sort(m_shapes.begin() + leftIndex, m_shapes.begin() + splitIndex, cmp);
			for (int i = leftIndex; i < splitIndex; i++)
			{
				lmin = glm::min(m_shapes[i]->getBox().getMin(), lmin);
				lmax = glm::max(m_shapes[i]->getBox().getMax(), lmax);
				lNumberOfObj++;
			}

			std::sort(m_shapes.begin() + splitIndex, m_shapes.begin() + rightIndex, cmp);
			for (int i = splitIndex; i < rightIndex; i++)
			{
				rmin = glm::min(m_shapes[splitIndex + i]->getBox().getMin(), lmin);
				rmax = glm::max(m_shapes[splitIndex + i]->getBox().getMax(), lmax);
				rNumberOfObj++;
			}

			AABB leftBox(lmin, lmax);
			AABB rightBox(rmin, rmax);

			cost = calculateCost(1.0f, 2.0f, lNumberOfObj, rNumberOfObj,  
								 leftBox.getSurfaceArea(), rightBox.getSurfaceArea(), pSA);
			
			if (cost > pCI)
			{
				leftBounds = leftBox;
				rightBounds = rightBox;
			}
			else
			{
				splitIndex++;
			}
		} while (cost > pCI);
		
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

		leftNode->createNode(leftIndex, splitIndex);
		leftNode->setBounds(leftBounds);
		rightNode->createNode(splitIndex, rightIndex);
		rightNode->setBounds(rightBounds);
		
		node->setRight(rightNode);
		node->setLeft(leftNode);

		buildRecursiveSAH(leftIndex, splitIndex, leftNode, depth + 1, axis);
		buildRecursiveSAH(leftIndex, splitIndex, leftNode, depth + 1, axis);
	}
}

const float & BVH::calculateCost(const float & ct, const float & ci, const int & nl, const int & nr, const float & saL, const float & saR, const float & saP)
{
	return ct + ((saL / saP) * nl * ci) + ((saR / saP)* nr * ci);
}

bool BVH::checkIntersection(Ray * ray, std::shared_ptr<Geometry>& hitObj, std::uint64_t & index, glm::vec2 & uv)
{
	std::stack<StackItem> stackItems;
	std::shared_ptr<BVHNode> currentNode = m_rootNode;
	StackItem item;
	item.ptr = m_rootNode;
	stackItems.emplace(item);

	do
	{
		if (currentNode->getBounds().checkRayCollision(ray))
		{
			if (currentNode->isLeaf())
			{
				float t1 = kInfinity;
				float t2 = kInfinity;

				if (currentNode->getLeft()->getBounds().checkRayCollision(ray, t1)
					|| currentNode->getRight()->getBounds().checkRayCollision(ray, t2))
				{
					if (t1 < t2)
					{
						currentNode = currentNode->getLeft();

						if (t2 != kInfinity)
						{
							StackItem farthest;
							farthest.ptr = stackItems.top().ptr->getRight();
							farthest.t = t1;
							stackItems.emplace(farthest);
						}
					}
					else
					{
						currentNode = currentNode->getRight();

						if (t2 != kInfinity)
						{
							StackItem farthest;
							farthest.ptr = stackItems.top().ptr->getLeft();
							farthest.t = t2;
							stackItems.emplace(farthest);
						}
					}

				}

			}
			else
			{
				for (int i = currentNode->getIndex();
					 i < currentNode->getIndex() + currentNode->getNumberOfObjects();
					 i++)
				{
					if (m_shapes[i]->getBox().checkRayCollision(ray))
					{
						if (m_shapes[i]->intersect(ray, index, uv))
						{
							hitObj = m_shapes[i];
						}
					}
				}
			}
		}


		if (!stackItems.empty())
			currentNode = stackItems.top().ptr;

		stackItems.pop();
	} while (!stackItems.empty());

	return (hitObj != nullptr);
}
