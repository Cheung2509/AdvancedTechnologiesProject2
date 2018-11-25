#include "Image.h"

#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include <chrono>
#include <atomic>
#include <future>

#include "Geometry.h"
#include "Camera.h"
#include "Ray.h"
#include "Light.h"

Image::Image(Camera* camera, unsigned int sizeX, unsigned int sizeY)
{
	m_imageData.m_size = glm::uvec2(sizeX, sizeY);
	m_imageData.m_aspectRatio = camera->getAspectRatio();
	m_imageData.m_fov = camera->getFOV();

	m_pixels.reserve(m_imageData.m_size.x * m_imageData.m_size.y);

	for (int i = 0; i < m_imageData.m_size.x * m_imageData.m_size.y; i++)
	{
		m_pixels.push_back(std::make_shared<glm::vec4>(0.0f));
	}

	m_image = new sf::Image();
	m_image.load()->create(m_imageData.m_size.x, m_imageData.m_size.y);
}

void Image::putPixel(glm::u64vec2 pos, glm::vec4 colour)
{
	*m_pixels[pos.x * m_imageData.m_size.y + pos.y] = colour;

	m_image.load()->setPixel(pos.x, pos.y,
					 sf::Color(glm::min(1.0f, colour.r) * 255.0f,
					 glm::min(1.0f, colour.g) * 255.0f,
					 glm::min(1.0f, colour.b) * 255.0f,
					 1.0f * 255.0f));
}

void Image::render(Camera* camera, const std::vector<std::shared_ptr<Geometry>>& shapes, const std::vector<std::shared_ptr<Light>>& lights)
{
	bool threading = true;

	float scale = glm::tan(glm::radians(m_imageData.m_fov * 0.5f));

	size_t cores = std::thread::hardware_concurrency();
	size_t max = m_imageData.m_size.x * m_imageData.m_size.y;
	std::vector<std::future<void>> future_vector;

	auto start = std::chrono::steady_clock::now();

	//Assign pixels to render for each thread
	for (size_t i = 0; i < cores; ++i)
	{
		future_vector.emplace_back(std::async([=]()
		{
			for (size_t index = i; i < max; index += cores)
			{
				if (index >= max)
					break;

				int xScreenPos = index % m_imageData.m_size.x;
				int yScreenPos = index / m_imageData.m_size.x;

				float x = (float)(2 * (xScreenPos + 0.5) / (float)m_imageData.m_size.x - 1) * m_imageData.m_aspectRatio * scale;
				float y = (float)(1 - 2 * (yScreenPos + 0.5) / (float)m_imageData.m_size.y) * scale;
				Ray ray(camera->getPos(), glm::normalize(glm::vec3(x, y, -1)), 0);

				//Assign colour to pixel
				putPixel(glm::u64vec2(xScreenPos, yScreenPos), glm::vec4(ray.castRay(shapes, lights, m_imageData),1.0f));
			}
		}));
	}

	//Wait for all threads to be done
	for (auto& thread : future_vector)
	{
		thread.wait();
	}

	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<float> time = end - start;

	std::cout << "Time to render:" << time.count() << std::endl;

	//createImage();
}

void Image::render(Camera * camera, std::shared_ptr<BVH>& bvh, const std::vector<std::shared_ptr<Light>>& lights)
{
	float scale = glm::tan(glm::radians(m_imageData.m_fov * 0.5f));

	int cores = std::thread::hardware_concurrency();
	int max = m_imageData.m_size.x * m_imageData.m_size.y;
	std::vector<std::future<void>> future_vector;

	auto start = std::chrono::steady_clock::now();

	//Assign pixels to render for each thread
	for (int c = 0; c < cores; ++c)
	{
		future_vector.emplace_back(std::async([=]()
		{
			for (int index = c; index < max; index += cores)
			{
				if (index >= max)
					break;
			
				int xScreenPos = index % m_imageData.m_size.x;
				int yScreenPos = index / m_imageData.m_size.x;

				float x = (2 * (xScreenPos + 0.5) / (float)m_imageData.m_size.x - 1) * m_imageData.m_aspectRatio * scale;
				float y = (1 - 2 * (yScreenPos + 0.5) / (float)m_imageData.m_size.y) * scale;
				Ray ray(camera->getPos(), glm::normalize(glm::vec3(x, y, -1)), 0);

				//Assign colour to pixel
				putPixel(glm::u64vec2(xScreenPos, yScreenPos), glm::vec4(ray.castRay(bvh, lights, m_imageData), 1.0f));
			}
		}));
	}

	//Wait for all threads to be done
	for (auto& thread : future_vector)
	{
		thread.wait();
	}

	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<float> time = end - start;

	std::cout << "Time to render:" << time.count() << std::endl;

	createImage();
}

bool Image::createImage()
{
	auto start = std::chrono::steady_clock::now();

	////Create image to render
	//std::vector<sf::Uint8> pixels;

	//for (auto& pixel : m_pixels)
	//{
	//	pixels.emplace_back(sf::Uint8(glm::min(1.0f, pixel->r) * 255));
	//	pixels.emplace_back(sf::Uint8(glm::min(1.0f, pixel->g) * 255));
	//	pixels.emplace_back(sf::Uint8(glm::min(1.0f, pixel->b) * 255));
	//	pixels.emplace_back(sf::Uint8(glm::min(1.0f, pixel->a) * 255));
	//}

	if (!m_texture.loadFromImage(*m_image.load()))
	{
		std::cout << "Failed to load image" << std::endl;
		return false;
	}

	m_sprite.setTexture(m_texture);

	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<float> time = end - start;

	//std::cout << "Time to create image:" << time.count() << std::endl;

	return true;
}

bool Image::exportImage()
{
	if (m_image.load()->saveToFile("Test.png"))
	{
		return true;
	}

	return false;
}

void Image::draw(sf::RenderWindow* renderWindow)
{
	m_texture.loadFromImage(*m_image.load());
	m_sprite.setTexture(m_texture);

	renderWindow->draw(m_sprite);
}
