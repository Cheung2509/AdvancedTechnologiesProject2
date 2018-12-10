#include "Model.h"

#include "Ray.h"

#include <iostream>

Model::Model(const std::string & filePath, const glm::vec3& colour)
	: Geometry(colour, GeometryType::MESH)
{
	loadOBJ(filePath);

	glm::vec3 min(kInfinity);
	glm::vec3 max(-kInfinity);

	for (auto& triangle : m_triangles)
	{
		min = glm::min(min, triangle->getBox().getMin());
		max = glm::max(max, triangle->getBox().getMax());
	}

	m_boundingBox = AABB(min, max);
}

void Model::loadOBJ(const std::string & filePath)
{
	FILE* file = new FILE();
	fopen_s(&file, filePath.c_str(), "r");

	if (!file)
	{
		std::cout << "Could not open obj file: " << filePath << std::endl;
		return;
	}

	std::vector<unsigned int> vertexIndices;
	std::vector<unsigned int> uvIndices;
	std::vector<unsigned int> normalIndices;

	std::vector<glm::vec3> temp_v;
	std::vector<glm::vec2> temp_uv;
	std::vector<glm::vec3> temp_n;
 
	while (1)
	{
		char lineHeader[128];

		int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_v.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			temp_uv.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_n.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1;
			std::string vertex2;
			std::string vertex3;
			unsigned int vertexIndex[3];
			unsigned int uvIndex[3];
			unsigned int normalIndex[3];

			int matches = fscanf_s(file, "%d %d %d/n",
								   &vertexIndex[0],
								   &vertexIndex[1],
								   &vertexIndex[2]);

			if (matches != 3)
			{
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return;
			}

			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
	glm::vec3 vertex[3];
	int count = 0;
	
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		if (i % 3 == 0 && i != 0)
		{
			m_triangles.emplace_back(std::make_shared<Triangle>(vertex[0], vertex[1], vertex[2],
									 m_diffuseColour));
			count = 0;
		}
		else
		{
			unsigned int index = vertexIndices[i];
			vertex[count] = temp_v[index - 1];
			count++;
		}

	}
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
