#include "Ray.h"

#include "Shape.h"

#define INFINITY 1e8

const glm::vec4 & Ray::trace(const glm::vec3 & rayorig, const glm::vec3 & rayDir, const std::vector<Shape>& shapes, const int & depth)
{
	float tnear = INFINITY;

	const Shape* shape = nullptr;

	for (auto& obj : shapes)
	{
		float t0 = INFINITY;
		float t1 = INFINITY;

		if (obj.intersect(rayorig, rayDir, t0, t1))
		{
			if (t0 < 0)
			{
				t0 = t1;
			}
			if (t0 < tnear)
			{
				tnear = t0;
				shape = &obj;
			}
		}
	}

	//Return black if nothing hits
	if (!shape)
	{
		return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}


}
