#pragma once

#include "AABB.h"
#include "Geometry.h"

#include <memory>
#include <future >
#include <vector>

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
	const AABB& getBounds() const { return m_bounds; }

	const std::shared_ptr<BVHNode>& getLeft() const { return m_left; }
	const std::shared_ptr<BVHNode>& getRight() const { return m_right; }

	void setLeft(std::shared_ptr<BVHNode> node) { m_left = node; }
	void setRight(std::shared_ptr<BVHNode> node) { m_right = node; }

	void addShape(std::shared_ptr<Geometry> shape) { m_shapes.push_back(shape); }
	const std::vector<std::shared_ptr<Geometry>>& getShapes() const { return m_shapes; }

private:
	unsigned int m_numberOfObjs;
	bool m_isLeaf;
	unsigned int m_index;
	AABB m_bounds;

	std::vector<std::shared_ptr<Geometry>> m_shapes;

	std::shared_ptr<BVHNode> m_left;
	std::shared_ptr<BVHNode> m_right;
};