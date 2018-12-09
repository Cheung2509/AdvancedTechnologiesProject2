#pragma once

#include "Triangle.h"
#include <vector>

class Model : public Geometry
{
public:
	Model() = default;
	Model(const std::string& filePath, const glm::vec3& colour);
	~Model() = default;

	const std::vector<std::unique_ptr<Triangle>>& getTriangles() const { return m_triangles; }

	bool intersect(Ray* ray, std::uint64_t& index, glm::vec2& uv, float& t);
private:
	unsigned int m_numOfTriangles;
	std::vector<std::unique_ptr<Triangle>> m_triangles;
	std::vector<std::unique_ptr<unsigned int>> m_index;
	std::vector<std::unique_ptr<glm::vec3>> m_normals;
	std::vector<std::unique_ptr<glm::vec2>> m_textureCoordinates;
};