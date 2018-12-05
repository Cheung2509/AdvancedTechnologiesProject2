#pragma once

#include "Geometry.h"

class  Triangle : public Geometry
{
public:
	Triangle() = default;
	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 pos, glm::vec3 col);
	~Triangle() = default;

	bool intersect(Ray* ray, std::uint64_t& index, glm::vec2& uv, float& t);
	void getSurfaceData(const glm::vec3& pHit, glm::vec3& nHit, glm::vec2& tex) const;
	void getSurfaceProperties(const glm::vec3& hitPoint, const glm::vec3& viewDirection,
							  const uint64_t& index, const glm::vec2& uv, glm::vec3& hitNormal, 
							  glm::vec2& hitTextureCoordinate) const;
private:
	glm::vec3 m_v0;
	glm::vec3 m_v1;
	glm::vec3 m_v2;

	glm::vec3 cols[3];
};