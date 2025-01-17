#pragma once

#include <SFML/System.hpp>

#include <glm/glm.hpp>

#include "Geometry.h"

class Sphere : public Geometry
{
public:
	Sphere() {};
	Sphere(const float& r, const glm::vec3& colour, const glm::vec3& pos = glm::vec3(0));
	~Sphere() {};

	bool intersect(Ray* ray, std::uint64_t &index, glm::vec2 &uv, float& t);
	void getSurfaceData(const glm::vec3& pHit, glm::vec3& nHit, glm::vec2& tex) const;
	void getSurface(const glm::vec3& p, const glm::vec3& i, const uint64_t& index, const glm::vec2& uv, glm::vec3& n, glm::vec2& st) const;
private:
	float m_radius;
	float m_radius2;
};