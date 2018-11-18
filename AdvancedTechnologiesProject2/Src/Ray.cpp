#include "Ray.h"

#include "Shape.h"
#include "Light.h"

#include <iostream>

constexpr float kInfinity = std::numeric_limits<float>::max();

const bool & Ray::trace(const glm::vec3 &orig, const glm::vec3 &dir,
						const std::vector<std::unique_ptr<Shape>> &objects,
						float &tNear, std::uint64_t &index, glm::vec2 &uv, Shape **hitObject)
{
	*hitObject = nullptr;

	for (auto& obj : objects)
	{
		float tNearK = kInfinity;
		std::uint64_t indexK;
		glm::vec2 uvK;

		if (obj->intersect(orig, dir, tNearK, indexK, uvK) && tNearK < tNear)
		{
			*hitObject = obj.get();
			tNear = tNearK;
			index = indexK;
			uv = uvK;
		}
	}

	return (*hitObject != nullptr);
}

const float & Ray::mix(const float & a, const float & b, const float & mix)
{
	return b * mix + a * (1 - mix);
}

const glm::vec3 Ray::mix(const glm::vec3 & a, const glm::vec3 & b, const float & mixValue)
{
	return a * (1 - mixValue) + b * mixValue;
}

glm::vec3 Ray::castRay(const glm::vec3 & orig, const glm::vec3 & dir, 
					   const std::vector<std::unique_ptr<Shape>>& shapes,
					   const std::vector<std::unique_ptr<Light>>& lights, 
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
	Shape* hitObject = nullptr;

	if (trace(orig, dir, shapes, tNear, index, uv, &hitObject))
	{
		glm::vec3 hitPoint = orig + dir * tNear;
		glm::vec3 n;
		glm::vec2 st;

		hitObject->getSurfaceProperties(hitPoint, dir, index, uv, n, st);
		glm::vec3 tmp = hitPoint;

		switch (hitObject->getMaterialType())
		{
		case MaterialType::DIFFUSE_AND_GLOSSY:
		{
			glm::vec3 reflctionDir = glm::normalize(reflect(dir, n));
			glm::vec3 refractionDir = glm::normalize(refract(dir, n, hitObject->getIOR()));
			glm::vec3 reflectionRayOrig = (glm::dot(reflctionDir, n) < 0) ?
				hitPoint - n * data.m_bias :
				hitPoint + n * data.m_bias;
			glm::vec3 refractionRayOrig = (glm::dot(refractionDir, n) < 0) ?
				hitPoint - n * data.m_bias :
				hitPoint + n * data.m_bias;

			glm::vec3 reflectionColour = castRay(reflectionRayOrig, reflctionDir,
												 shapes, lights, data, depth + 1, 1);
			glm::vec3 refractionColour = castRay(refractionRayOrig, refractionRayOrig,
												 shapes, lights, data, depth + 1, 1);

			float kr;
			fresnel(dir, n, hitObject->getIOR(), kr);
			hitColour = reflectionColour * kr + refractionColour * (1 - kr);
			break;
		}
		case MaterialType::REFLECTION:
		{
			float kr;
			fresnel(dir, n, hitObject->getIOR(), kr);
			glm::vec3 reflctionDir = glm::normalize(reflect(dir, n));
			glm::vec3 reflectionRayOrig = (glm::dot(reflctionDir, n) < 0) ?
				hitPoint - n * data.m_bias :
				hitPoint + n * data.m_bias;
			hitColour = castRay(reflectionRayOrig, reflctionDir, shapes, lights, data, depth + 1) * kr;

			break;
		}
		default:
		{
			glm::vec3 lightAmt = glm::vec3(0);
			glm::vec3 specularColour = glm::vec3(0);

			glm::vec3 shadowPointOrig = (glm::dot(dir, n) < 0) ?
				hitPoint - n * data.m_bias :
				hitPoint + n * data.m_bias;

			for (auto& obj : lights)
			{
				glm::vec3 lightDir = obj->getPos() - hitPoint;

				float lightDistance2 = glm::dot(lightDir, lightDir);
				lightDir = glm::normalize(lightDir);
				float lDotN = glm::max(0.0f, glm::dot(lightDir, n));
				Shape* shadowHitObject = nullptr;
				float tNearShadow = kInfinity;

				bool inShadow = trace(shadowPointOrig, lightDir, shapes, tNearShadow, index, uv, &shadowHitObject)
					&& tNearShadow * tNearShadow < lightDistance2;

				lightAmt += (1 - inShadow) * obj->getIntensity() * lDotN;
				glm::vec3 reflectionDir = reflect(-lightDir, n);
				specularColour += glm::pow(glm::max(0.0f, -glm::dot(reflectionDir, dir)), hitObject->getSpecularExponent()) * obj->getIntensity();
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
