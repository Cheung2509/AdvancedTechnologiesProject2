#include "Image.h"

#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

#include "Geometry.h"
#include "Camera.h"
#include "Ray.h"

Image::Image(Camera* camera, unsigned int sizeX, unsigned int sizeY)
{
	m_imageData.m_size = glm::uvec2(sizeX, sizeY);
	m_imageData.m_aspectRatio = camera->getAspectRatio();
	m_imageData.m_fov = camera->getFOV();

	m_pixels.reserve(m_imageData.m_size.x * m_imageData.m_size.y * 4);
	
	for (int i = 0; i < m_imageData.m_size.x * m_imageData.m_size.y * 4; i++)
	{
		m_pixels.emplace_back(new float(0.0f));
	}
}

void Image::putPixel(glm::u64vec2 pos, sf::Color colour)
{
	*m_pixels[(m_imageData.m_size.x * pos.x + pos.y) * 3] = colour.r;
	*m_pixels[(m_imageData.m_size.x * pos.x + pos.y) * 3 + 1] = colour.g;
	*m_pixels[(m_imageData.m_size.x * pos.x + pos.y) * 3 + 2] = colour.b;
}

void Image::putPixel(glm::u64vec2 pos, glm::vec3 colour)
{
	*m_pixels[(m_imageData.m_size.x * pos.x + pos.y) * 4] = colour.x;
	*m_pixels[(m_imageData.m_size.x * pos.x + pos.y) * 4 + 1] = colour.y;
	*m_pixels[(m_imageData.m_size.x * pos.x + pos.y) * 4 + 2] = colour.z;
	*m_pixels[(m_imageData.m_size.x * pos.x + pos.y) * 4 + 3] = 1.0f;				 
}

//void Image::createImage(Camera* camera, const std::vector<std::unique_ptr<Shape>>& shapes, 
//						const std::vector<std::unique_ptr<Light>>& light)
//{
//	float invWidth = 1 / float(m_imageData.m_size.x);
//	float invHeight = 1 / float(m_imageData.m_size.y);
//	float angle = glm::radians(glm::angle(camera->getRotation()));
//
//	for (int j = 0; j < m_imageData.m_size.x; j++)
//	{
//		for (int i = 0; i < m_imageData.m_size.y; i++)
//		{
//			float x = (2 * (i + 0.5f) / (float)m_imageData.m_size.x - 1) * camera->getAspectRatio() * angle;
//			float y = (1 - 2 * (j + 0.5f) / (float)m_imageData.m_size.y) * angle;
//			glm::vec3 dir = glm::normalize(glm::vec3(x, y, -1) * dir);
//			
//
//			putPixel(sf::Vector2u(j, i), Ray::castRay(camera->getPos(), dir, shapes));
//		}
//	}
//
//	std::vector<sf::Uint8> pixelData;
//
//	for (auto& pixel : m_pixels)
//	{
//		pixelData.push_back(glm::min(float(1), *pixel) * 255);
//	}
//
//	m_image.create(m_imageData.m_size.x, m_imageData.m_size.y, pixelData.data());
//
//	m_texture.loadFromImage(m_image);
//
//	m_sprite.setTexture(m_texture);
//}

void Image::render(Camera * camera, const std::vector<std::unique_ptr<Geometry>>& shapes, const std::vector<std::unique_ptr<Light>>& lights)
{
	float scale = glm::tan(glm::radians(m_imageData.m_fov * 0.5f));

	for (int j = 0; j < m_imageData.m_size.x; ++j)
	{
		for (int i = 0; i < m_imageData.m_size.y; ++i)
		{
			//Calculate the direction of the ray
			float y = (2 * (i + 0.5) / (float)m_imageData.m_size.x - 1) * m_imageData.m_aspectRatio * scale;
			float x = (1 - 2 * (j + 0.5) / (float)m_imageData.m_size.y) * scale;
			glm::vec3 dir = glm::normalize(glm::vec3(x, y, -1));

			//Assign colour to pixel
			putPixel(glm::u64vec2(i, j), Ray::castRay(camera->getPos(), dir, shapes, lights, m_imageData, 0));
		}
	}

	//Create image to render
	std::vector<sf::Uint8> pixels;

	for (auto& pixel : m_pixels)
	{
		pixels.push_back(sf::Uint8(glm::min(1.0f, *pixel) * 255));
	}

	m_image.create(m_imageData.m_size.x, m_imageData.m_size.y, pixels.data());

	m_texture.loadFromImage(m_image);

	m_sprite.setTexture(m_texture);
}

bool Image::exportImage()
{
	if (m_image.saveToFile("Test.png"))
	{
		return true;
	}

	return false;
}

void Image::draw(sf::RenderWindow* renderWindow)
{
	renderWindow->draw(m_sprite);
}
