#include "BVHNode.h"

void BVHNode::createLeaf(const unsigned int& index, const unsigned int& numberOfObjs)
{
	m_index = index;
	m_numberOfObjs = numberOfObjs;
	m_isLeaf = true;
}

void BVHNode::createNode(unsigned int left, unsigned int numberOfObjs)
{
	m_index = left;
	m_numberOfObjs = numberOfObjs;
	m_isLeaf = false;
}
