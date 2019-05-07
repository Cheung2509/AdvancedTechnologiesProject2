#include "Model.h"

#include "Ray.h"

#include <iostream>

Model::Model(const std::string & filePath, const glm::vec3& colour, const glm::vec3& pos = glm::vec3(0.0f))
	: Geometry(colour, GeometryType::MESH)
{
	m_pos = pos;
	loadOBJ(filePath);
	

	glm::vec3 min(kInfinity);
	glm::vec3 max(-kInfinity);

	for (auto& triangle : m_triangles)
	{
		min = glm::min(min, triangle->getBox().getMin());
		max = glm::max(max, triangle->getBox().getMax());
		triangle->setMaterialType(m_matType);
	}


	m_boundingBox = AABB(min, max);
}

void Model::setMaterialType(MaterialType type)
{
	for(auto& triangle : m_triangles)
	{
		triangle->setMaterialType(type);
	}
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

	std::vector<std::shared_ptr<glm::vec3>> temp_v;
	std::vector<std::shared_ptr<glm::vec2>> temp_uv;
	std::vector<std::shared_ptr<glm::vec3>> temp_n;
 
	while (1)
	{
		char lineHeader[128];

		int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			std::shared_ptr<glm::vec3> vertex = std::make_shared<glm::vec3>();
			fscanf_s(file, "%f %f %f\n", &vertex->x, &vertex->y, &vertex->z);
			*vertex += m_pos;
			temp_v.emplace_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			std::shared_ptr<glm::vec2> uv = std::make_shared<glm::vec2>();
			fscanf_s(file, "%f %f\n", &uv->x, &uv->y);
			temp_uv.emplace_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			std::shared_ptr<glm::vec3> normal = std::make_shared<glm::vec3>();
			fscanf_s(file, "%f %f %f\n", &normal->x, &normal->y, &normal->z);
			temp_n.emplace_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1;
			std::string vertex2;
			std::string vertex3;
			unsigned int vertexIndex[3];
			unsigned int uvIndex[3];
			unsigned int normalIndex[3];

			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", 
								 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
								 &vertexIndex[1], &uvIndex[1], &normalIndex[1], 
								 &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

			if (matches != 9)
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
	std::shared_ptr<glm::vec3> vertex[3];
	std::shared_ptr<glm::vec3> normals[3];
	std::shared_ptr<glm::vec2> uvs[3];
	int count = 0;
	
	for (unsigned int i = 0; i < vertexIndices.size(); ++i)
	{
		if (i % 3 == 0 && count != 0)
		{
			m_triangles.emplace_back(std::make_shared<Triangle>(vertex[0], vertex[1], vertex[2],
									 normals[0], normals[1], normals[2],
									 uvs[0],uvs[1], uvs[2],
									 m_diffuseColour));
			count = 0;
		}
		unsigned int index = vertexIndices[i] - 1;
		unsigned int nIndex = normalIndices[i] - 1;
		unsigned int uvIndex = uvIndices[i] - 1;

		normals[count] = temp_n[index];
		vertex[count] = temp_v[index];
		uvs[count] = temp_uv[index];
		count++;
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
				intersected = true;
			}
		}
	}

	return intersected;
}
