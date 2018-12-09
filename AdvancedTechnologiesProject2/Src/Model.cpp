#include "Model.h"

#include "Ray.h"

Model::Model(const std::string & filePath, const glm::vec3& colour)
	: Geometry(colour, GeometryType::MESH)
{
	
}

bool Model::intersect(Ray * ray, std::uint64_t & index, glm::vec2 & uv, float & t)
{
	bool intersected = false;

	for (auto& triangle : m_triangles)
	{
		if (triangle->getBox().checkRayCollision(ray))
		{
			if (triangle->intersect(ray, index, uv, t))
			{
				if (t < ray->getClosestHit())
				{
					ray->setHit(t);
				}
			}
		}
	}

	return intersected;
}
