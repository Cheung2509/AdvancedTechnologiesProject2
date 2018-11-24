#include "BoundingVolumeHierachy.h"

#include <algorithm>

#include "BVHNode.h"
#include "Geometry.h"
#include "ComparePrimitives.h"

void BVH::build(std::vector<std::shared_ptr<Geometry>> shapes)
{
	m_shapes = shapes;

	auto rootNode = std::make_shared<BVHNode>();
	

	glm::vec3 min(0);
	glm::vec3 max(0);
	
	ComparePrimitives cmp(Axis::X);
	Axis axis = Axis::X;
	std::sort(m_shapes.begin(), m_shapes.end(), cmp);

	for (auto& obj : shapes)
	{
		min = glm::min(obj->getBox().getMin() + obj->getPos(), min);
		max =glm::max(obj->getBox().getMax() + obj->getPos(), max);
	}

	AABB worldBox(min, max);
	rootNode->setBounds(worldBox);
	m_nodes.push_back(rootNode);
	buildRecursive(0, m_shapes.size() - 1, rootNode, 0);
	
}

void BVH::buildRecursive(int leftIndex, int rightIndex, std::shared_ptr<BVHNode> node, int depth)
{
	if ((rightIndex - leftIndex) <= 2)
	{
		node->createLeaf(leftIndex, rightIndex);
	}
	else
	{
		int splitIndex = (rightIndex + leftIndex) / 2;

		auto leftNode = std::make_shared<BVHNode>();
		glm::vec3 min(0);
		glm::vec3 max(0);
		int numberOfObj = 0;

		//Calculate lefnode
		ComparePrimitives cmp(Axis::Y);
		std::sort(m_shapes.begin() + leftIndex, m_shapes.begin() + splitIndex, cmp);
		for (int i = leftIndex; i <= splitIndex; i++)
		{
			min = glm::min(m_shapes[i]->getBox().getMin() + m_shapes[i]->getPos() , min);
			max = glm::max(m_shapes[i]->getBox().getMax() + m_shapes[i]->getPos(), max);

			numberOfObj++;
		}
		leftNode->setBounds(AABB(min, max));
		leftNode->createNode(leftIndex, numberOfObj);


		//Calculate Right node 
		auto rightNode = std::make_shared<BVHNode>();
		min = glm::vec3(0);
		max = glm::vec3(0);

		
		std::sort(m_shapes.begin() + splitIndex, m_shapes.begin() + rightIndex, cmp);
		for (int i = splitIndex; i <= rightIndex; i++)
		{
			min = glm::min(m_shapes[i]->getBox().getMin() + m_shapes[i]->getPos(), min);
			max = glm::max(m_shapes[i]->getBox().getMax() + m_shapes[i]->getPos(), max);

			numberOfObj++;
		}
		rightNode->setBounds(AABB(min, max));
		rightNode->createNode(splitIndex, rightIndex);

		m_nodes.push_back(leftNode);
		m_nodes.push_back(rightNode);
		
		buildRecursive(leftIndex, splitIndex, leftNode, depth + 1);
		buildRecursive(splitIndex, rightIndex, rightNode, depth + 1);
	}
}
