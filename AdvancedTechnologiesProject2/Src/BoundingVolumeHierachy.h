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
	BVHNode* ptr;
	float t;
};

class BVH
{
public:
	BVH() = default;
	~BVH() = default;

	void buildBVH(std::vector<std::shared_ptr<Geometry>> shapes);
	void buildRecursiveBVH(int leftIndex, int rightIndex, std::shared_ptr<BVHNode>& node, int depth, Axis& axis);

	void buildSAH(std::vector<std::shared_ptr<Geometry>> shapes);
	void buildRecursiveSAH(int leftIndex, int rightIndex, std::shared_ptr<BVHNode> node, int depth);

	bool checkIntersection(Ray* ray, Geometry** hitObj, std::uint64_t& index, glm::vec2& uv);
private:
	std::vector<std::shared_ptr<Geometry>> m_shapes;

	std::shared_ptr<BVHNode> m_rootNode;
};