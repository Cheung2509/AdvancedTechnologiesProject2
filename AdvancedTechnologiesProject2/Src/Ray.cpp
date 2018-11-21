#include "Ray.h"

#include "Geometry.h"
#include "Light.h"

#include <iostream>

constexpr float kInfinity = std::numeric_limits<float>::max();

Ray::Ray(const glm::vec3 & rayOrig, const glm::vec3 & rayDir)
	: m_rayOrig(rayOrig), m_rayDirection(rayDir)
{
	m_invDirection = 1.0f / m_rayDirection;

	m_sign[0] = (m_invDirection.x < 0);
	m_sign[1] = (m_invDirection.y < 0);
	m_sign[2] = (m_invDirection.z < 0);
}

const bool Ray::trace(const std::vector<std::shared_ptr<Geometry>> &objects,
						float &tNear, std::uint64_t &index, glm::vec2 &uv, Geometry **hitObject)
{
	*hitObject = nullptr;

	for (auto& obj : objects)
	{
		float tNearK = kInfinity;
		std::uint64_t indexK;
		glm::vec2 uvK;

		if (obj->getBox().checkRayCollision(this))
		{
			if (obj->intersect(this, tNearK, indexK, uvK) && tNearK < tNear)
			{
				*hitObject = obj.get();
				tNear = tNearK;
				index = indexK;
				uv = uvK;
			}
		}
	}

	return (*hitObject != nullptr);
}

const float Ray::mix(const float & a, const float & b, const float & mix)
{
	return b * mix + a * (1 - mix);
}

const glm::vec3 Ray::mix(const glm::vec3 & a, const glm::vec3 & b, const float & mixValue)
{
	return a * (1 - mixValue) + b * mixValue;
}

glm::vec3 Ray::castRay(const std::vector<std::shared_ptr<Geometry>>& shapes,
					   const std::vector<std::shared_ptr<Light>>& lights,
					   ImageData data, std::uint64_t depth, bool test)
{
	if (depth > data.m_maxDepth)
	{
		return data.m_backgroundColour;
	}

	glm::vec3 hitColour = data.m_backgroundColour;
	float tNear = kInfinity;
	glm::vec2 uv;
	std::uint64_t index = 0;
	Geometry* hitObject = nullptr;

	if (this->trace(shapes, tNear, index, uv, &hitObject))
	{
		glm::vec3 hitPoint = m_rayOrig  + m_rayDirection * tNear;
		glm::vec3 n;
		glm::vec2 st;

		hitObject->getSurfaceProperties(hitPoint, m_rayDirection, index, uv, n, st);
		glm::vec3 tmp = hitPoint;

		switch (hitObject->getMaterialType())
		{
		case MaterialType::REFLECTION_AND_REFRACTION:
		{
			glm::vec3 reflctionDir = glm::normalize(reflect(m_rayDirection, n));
			glm::vec3 refractionDir = glm::normalize(refract(m_rayDirection, n, hitObject->getIOR()));
			glm::vec3 reflectionRayOrig = (glm::dot(reflctionDir, n) < 0) ?
				hitPoint - n * data.m_bias :
				hitPoint + n * data.m_bias;
			glm::vec3 refractionRayOrig = (glm::dot(refractionDir, n) < 0) ?
				hitPoint - n * data.m_bias :
				hitPoint + n * data.m_bias;

			glm::vec3 reflectionColour = this->castRay(shapes, lights, data, depth + 1, 1);
			glm::vec3 refractionColour = this->castRay(shapes, lights, data, depth + 1, 1);

			float kr;
			fresnel(m_rayDirection, n, hitObject->getIOR(), kr);
			hitColour = reflectionColour * kr + refractionColour * (1 - kr);
			break;
		}
		case MaterialType::REFLECTION:
		{
			float kr;
			fresnel(m_rayDirection, n, hitObject->getIOR(), kr);
			glm::vec3 reflctionDir = glm::normalize(this->reflect(m_rayDirection, n));
			glm::vec3 reflectionRayOrig = (glm::dot(reflctionDir, n) < 0) ?
				hitPoint - n * data.m_bias :
				hitPoint + n * data.m_bias;
			hitColour = this->castRay(shapes, lights, data, depth + 1) * kr;

			break;
		}
		default:
		{
			glm::vec3 lightAmt = glm::vec3(0);
			glm::vec3 specularColour = glm::vec3(0);


			auto shadowRay = std::make_unique<Ray>((glm::dot(m_rayDirection, n) < 0) ?
				hitPoint - n * data.m_bias :
				hitPoint + n * data.m_bias, glm::vec3(0));

			for (auto& obj : lights)
			{
				glm::vec3 lightDir = glm::normalize(obj->getPos() - hitPoint);

				float lightDistance2 = glm::dot(lightDir, lightDir);
				shadowRay->m_rayDirection = lightDir;
				float lDotN = glm::max(0.0f, glm::dot(lightDir, n));
				Geometry* shadowHitObject = nullptr;
				float tNearShadow = kInfinity;

				bool inShadow = shadowRay->trace(shapes, tNearShadow, index, uv, &shadowHitObject)
					&& tNearShadow * tNearShadow < lightDistance2;

				lightAmt += (1 - inShadow) * obj->getIntensity() * lDotN;
				glm::vec3 reflectionDir = reflect(-lightDir, n);
				specularColour += glm::pow(glm::max(0.0f, -glm::dot(reflectionDir, m_rayDirection)), hitObject->getSpecularExponent()) * obj->getIntensity();
			}
			hitColour = lightAmt * hitObject->evalDiffuseColour(st) * hitObject->getKD() + specularColour * hitObject->getKS();
			break;
		}
		}
	}

	return hitColour;
}

glm::vec3 Ray::reflect(const glm::vec3& i, const glm::vec3& n)
{
	return i - 2 * glm::dot(i, n) * n;
}

glm::vec3 Ray::refract(const glm::vec3 & i, const glm::vec3 & n, const float & ior)
{
	float cosi = glm::clamp(glm::dot(i, n), -1.0f, 1.0f);
	float etai = 1;
	float etat = ior;

	glm::vec3 normal = n;

	if (cosi < 0)
	{
		cosi = -cosi;
	}
	else
	{
		std::swap(etai, etat);
		normal = -n;
	}

	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	glm::vec3 temp;
	if (k < 0)
	{
		temp = glm::vec3(0);
	}
	else
	{
		temp = eta * i + (eta * cosi - glm::sqrt(k)) * n;
	}

	return temp;
}

void Ray::fresnel(const glm::vec3 & i, glm::vec3 & n, const float & ior, float & kr)
{
	float cosi = glm::clamp(glm::dot(i, n), -1.0f, 1.0f);
	float etai = 1;
	float etat = ior;

	if (cosi > 0)
	{
		std::swap(etai, etat);
	}

	float sint = etai / etat * glm::sqrt(glm::max(0.0f, 1 - cosi * cosi));

	if (sint >= 1)
	{
		kr = 1;
	}
	else
	{
		float cost = glm::sqrt(std::max(0.0f, 1 - sint * sint));
		cosi = fabsf(cosi);
		float rs = ((etat*cosi) - (etai * cost)) / ((etat * cosi) + (etai* cost));
		float rp = ((etai* cosi) - (etat * cost)) / ((etai * cosi) + (etat*cost));
		kr = (rs * rs + rp * rp) / 2;
	}
}
