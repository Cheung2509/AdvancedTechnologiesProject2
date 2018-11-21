#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Image.h"

class Geometry;
class Light;

class Ray
{
public:
	Ray() = default;
	Ray(const glm::vec3& rayOrig, const glm::vec3& rayDir);
	~Ray() = default;

	const bool trace(const std::vector<std::shared_ptr<Geometry>> &objects,
					 float &tNear, std::uint64_t &index, glm::vec2 &uv, Geometry **hitObject);

	const float mix(const float& a, const float& b, const float& mix);
	const glm::vec3 mix(const glm::vec3& a, const glm::vec3& b, const float & mixValue);

	glm::vec3 castRay(const std::vector<std::shared_ptr<Geometry>>& shapes,
					 const std::vector<std::shared_ptr<Light>>& lights,
					 ImageData data, std::uint64_t depth, bool test = false);

	glm::vec3 reflect(const glm::vec3& i, const glm::vec3& n);
	glm::vec3 refract(const glm::vec3& i, const glm::vec3& n, const float& ior);
	void fresnel(const glm::vec3& i, glm::vec3& n, const float &ior, float& kr);


	const glm::vec3& getOrigin() const { return m_rayOrig; }
	const glm::vec3& getDirection() const { return m_rayDirection; }
	const glm::vec3& getInvDirection() const { return m_invDirection; }
	const int& getSign(const int& i) const { return m_sign[i]; }

private:
	glm::vec3 m_rayOrig;
	glm::vec3 m_rayDirection;
	glm::vec3 m_invDirection;

	int m_sign[3];
};