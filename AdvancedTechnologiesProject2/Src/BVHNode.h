#pragma once

#include "AABB.h"

class BVHNode
{
public:
	BVHNode() = default;
	~BVHNode() = default;

	void setBounds(const AABB& box) { m_bounds = box; }
	void createLeaf(const unsigned int& index, const unsigned int& numberOfObjs);
	void createNode(unsigned int left, unsigned int numberOfObjs);

	const bool& isLeaf() const { return m_isLeaf; }
	const unsigned int& getIndex() const { return m_index; }
	const unsigned int& getNumberOfObjects() { return m_numberOfObjs; }
	const AABB& getBouds() const { return m_bounds; }

private:
	AABB m_bounds;
	bool m_isLeaf;
	unsigned int m_numberOfObjs;
	unsigned int m_index;
};