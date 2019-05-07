#pragma once

#include "Geometry.h"
#include "Vertex.h"

class  Triangle : public Geometry
{
public:
	Triangle() = default;
	Triangle(std::shared_ptr<glm::vec3> v0, std::shared_ptr<glm::vec3> v1, std::shared_ptr<glm::vec3> v2, 
			 std::shared_ptr<glm::vec3> n0, std::shared_ptr<glm::vec3> n1, std::shared_ptr<glm::vec3> n2,
			 std::shared_ptr<glm::vec2> uv0, std::shared_ptr<glm::vec2> uv1, std::shared_ptr<glm::vec2> uv2,
			 glm::vec3 col);
	Triangle(std::shared_ptr<glm::vec3> v0, std::shared_ptr<glm::vec3> v1, std::shared_ptr<glm::vec3> v2, glm::vec3 col);
	~Triangle() = default;

	bool intersect(Ray* ray, std::uint64_t& index, glm::vec2& uv, float& t) override;
	void getSurfaceData(const glm::vec3& pHit, glm::vec3& nHit, glm::vec2& tex) const;
	void getSurface(const glm::vec3& hitPoint, const glm::vec3& viewDirection,
							  const uint64_t& index, const glm::vec2& uv, glm::vec3& hitNormal,
							  glm::vec2& hitTextureCoordinate) const;
private:
	Vertex m_v0;
	Vertex m_v1;
	Vertex m_v2;

};