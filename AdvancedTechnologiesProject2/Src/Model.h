#pragma once

#include "Triangle.h"
#include <vector>
#include <string>

class Model : public Geometry
{
public:
	Model() = default;
	Model(const std::string& filePath, const glm::vec3& colour, const glm::vec3& pos);
	~Model() = default;

	void loadOBJ(const std::string& filePath);

	const std::vector<std::shared_ptr<Triangle>>& getTriangles() { return m_triangles; }

	bool intersect(Ray* ray, std::uint64_t& index, glm::vec2& uv, float& t);

	void getSurfaceData(const glm::vec3& pHit, glm::vec3& nHit, glm::vec2& tex) const {}
	void getSurfaceProperties(const glm::vec3& hitPoint, const glm::vec3& viewDirection,
							  const uint64_t& index, const glm::vec2& uv, glm::vec3& hitNormal,
							  glm::vec2& hitTextureCoordinate) const
	{
	}
private:
	unsigned int m_numOfTriangles;
	std::vector<std::shared_ptr<Triangle>> m_triangles;
	std::vector<std::unique_ptr<unsigned int>> m_index;
	std::vector<std::unique_ptr<glm::vec3>> m_normals;
	std::vector<std::unique_ptr<glm::vec2>> m_textureCoordinates;
};