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
	~Ray() = default;

	static const bool& trace(const glm::vec3 &orig, const glm::vec3 &dir,
							 const std::vector<std::unique_ptr<Geometry>> &objects,
							 float &tNear, std::uint64_t &index, glm::vec2 &uv, Geometry **hitObject);

	static const float& mix(const float& a, const float& b, const float& mix);
	static const glm::vec3 mix(const glm::vec3& a, const glm::vec3& b, const float & mixValue);

	static glm::vec3 castRay(const glm::vec3& orig, const glm::vec3& dir,
							 const std::vector<std::unique_ptr<Geometry>>& shapes,
							 const std::vector<std::unique_ptr<Light>>& lights,
							 ImageData data, std::uint64_t depth, bool test = false);

	static glm::vec3 reflect(const glm::vec3& i, const glm::vec3& n);
	static glm::vec3 refract(const glm::vec3& i, const glm::vec3& n, const float& ior);
	static void fresnel(const glm::vec3& i, glm::vec3& n, const float &ior, float& kr);
private:
};