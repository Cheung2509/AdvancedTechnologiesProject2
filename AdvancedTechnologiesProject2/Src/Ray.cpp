#include "Ray.h"

#include "Shape.h"

#define MAX_RAY_DEPTH 5

const glm::vec3& Ray::trace(const glm::vec3 & rayorig, const glm::vec3 & rayDir, const std::vector<Shape*>& shapes, const int & depth)
{
	float tnear = INFINITY;

	const Shape* shape = nullptr;

	for (auto& obj : shapes)
	{
		float t0 = INFINITY;
		float t1 = INFINITY;

		if (obj->intersect(rayorig, rayDir, t0, t1))
		{
			if (t0 < 0)
			{
				t0 = t1;
			}
			if (t0 < tnear)
			{
				tnear = t0;
				shape = obj;
			}
		}
	}

	//Return black if nothing hits
	if (!shape)
	{
		return glm::vec3(1.0f, 1.0f, 1.0f);
	}

	glm::vec3 surfaceCol = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 pHit = rayorig + rayDir * tnear;
	glm::vec3 nHit = glm::normalize(pHit - shape->getPos());

	float bias = 1e-4;
	bool inside = false;

	if (glm::dot(rayDir, nHit) > 0)
		nHit = -nHit, inside = true;

	if ((shape->getTransparency() > 0 || shape->getReflecation() > 0) && depth < MAX_RAY_DEPTH)
	{
		float facingRatio = -glm::dot(rayDir, nHit);

		float fresnelEffect = mix(glm::pow(1 - facingRatio, 3), 1, 0.1);

		glm::vec3 reflDir = glm::normalize(rayDir - nHit * 2.0f * glm::dot(rayDir, nHit));

		glm::vec3 reflection = trace(pHit + nHit * bias, reflDir, shapes, depth + 1);
		glm::vec3 refraction = glm::vec3(0.0f, 0.0f, 0.0f);

		if (shape->getTransparency())
		{
			float ior = 1.1;
			float eta = inside ? ior : 1 / ior;
			float cosi = -glm::dot(nHit, rayDir);
			float k = 1 - eta * eta*(1 - cosi * cosi);
			glm::vec3 refrDir = glm::normalize(rayDir * eta + nHit * (eta * cosi - glm::sqrt(k)));
			refraction = trace(pHit - nHit * bias, refrDir, shapes, depth + 1);
		}

		surfaceCol = (reflection * fresnelEffect + refraction * 
			(1 - fresnelEffect)* shape->getTransparency()) * shape->getSurfaceColour();
	}
	else
	{
		for (auto& obj : shapes)
		{
			if (obj->getEmissionColour().x > 0)
			{
				glm::vec3 transmission = glm::vec3(1.0f, 1.0f, 1.0f);
				glm::vec3 lightDir = glm::normalize(obj->getPos() - pHit);

				for (auto& obj2 : shapes)
				{
					if (obj != obj2)
					{
						float t0;
						float t1;
						if (obj2->intersect(pHit + nHit * bias, lightDir, t0, t1))
						{
							transmission = glm::vec3(0.0f, 0.0f, 0.0f);
							break;
						}
					}
				}

				surfaceCol += shape->getSurfaceColour() * transmission * 
					glm::max(0.0f, glm::dot(nHit, lightDir)) * obj->getEmissionColour();
			}
		}
	}

	return surfaceCol + shape->getEmissionColour();
}

const float & Ray::mix(const float & a, const float & b, const float & mix)
{
	return b * mix + a * (1 - mix);
}
