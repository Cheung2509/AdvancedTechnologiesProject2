#pragma once

#include <memory>
#include <vector>

class Geometry;
class BVHNode;

class BVH
{
public:
	BVH() = default;
	~BVH() = default;

	void build(std::vector<std::shared_ptr<Geometry>> shapes);
	void buildRecursive(int leftIndex, int rightIndex, std::shared_ptr<BVHNode> node, int depth);

private:
	std::vector<std::shared_ptr<Geometry>> m_shapes;
	std::vector<std::shared_ptr<BVHNode>> m_nodes;
};