#pragma once

#include <memory>
#include <vector>

#include "glm/glm.hpp"

#include "ComparePrimitives.h"

class Geometry;
class BVHNode;
class Ray;

struct StackItem
{
	std::shared_ptr<BVHNode> ptr;
	float t;
};

class BVH
{
public:
	BVH() = default;
	~BVH() = default;

	void build(std::vector<std::shared_ptr<Geometry>> shapes);
	void buildRecursive(int leftIndex, int rightIndex, std::shared_ptr<BVHNode> node, int depth, Axis& axis);

	bool checkIntersection(Ray* ray, std::shared_ptr<Geometry>& hitObj, std::uint64_t& index, glm::vec2& uv);
private:
	std::vector<std::shared_ptr<Geometry>> m_shapes;
	std::vector<std::shared_ptr<BVHNode>> m_nodes;

	std::shared_ptr<BVHNode> m_rootNode;
};