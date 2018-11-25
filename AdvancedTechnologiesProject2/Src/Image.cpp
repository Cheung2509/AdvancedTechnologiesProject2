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

	m_pixels.reserve(m_imageData.m_size.x * m_imageData.m_size.y * 4);

	auto start = std::chrono::steady_clock::now();

	for (int i = 0; i < m_imageData.m_size.x * m_imageData.m_size.y * 4; i++)
	{
		m_pixels.push_back(std::make_shared<float>(0.0f));
	}

	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<float> time = end - start;

	std::cout << "Time to push back all pixels:" << time.count() << std::endl;

	m_image = new sf::Image();
	m_image.load()->create(m_imageData.m_size.x, m_imageData.m_size.y);
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
				if (index > max)
					break;

				size_t j = index % m_imageData.m_size.x;
				size_t i = index / m_imageData.m_size.x;

				float x = (float)(2 * (i + 0.5) / (float)m_imageData.m_size.x - 1) * m_imageData.m_aspectRatio * scale;
				float y = (float)(1 - 2 * (j + 0.5) / (float)m_imageData.m_size.y) * scale;
				Ray ray(camera->getPos(), glm::normalize(glm::vec3(x, y, -1)), 0);

				//Assign colour to pixel
				putPixel(glm::u64vec2(j, i), ray.castRay(shapes, lights, m_imageData));
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
				if (index > max)
					break;

				size_t j = index % m_imageData.m_size.x;
				size_t i = index / m_imageData.m_size.x;

				float x = (float)(2 * (i + 0.5) / (float)m_imageData.m_size.x - 1) * m_imageData.m_aspectRatio * scale;
				float y = (float)(1 - 2 * (j + 0.5) / (float)m_imageData.m_size.y) * scale;
				Ray ray(camera->getPos(), glm::normalize(glm::vec3(x, y, -1)), 0);

				//Assign colour to pixel
				putPixel(glm::u64vec2(j, i), ray.castRay(bvh, lights, m_imageData));
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

	//Create image to render
	std::vector<sf::Uint8> pixels;

	for (auto& pixel : m_pixels)
	{
		pixels.emplace_back(sf::Uint8(glm::min(1.0f, *pixel) * 255));
	}

	m_image.load()->create(m_imageData.m_size.x, m_imageData.m_size.y, pixels.data());

	if (!m_texture.loadFromImage(*m_image.load()))
	{
		std::cout << "Failed to load image" << std::endl;
		return false;
	}

	m_sprite.setTexture(m_texture);

	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<float> time = end - start;

	std::cout << "Time to create image:" << time.count() << std::endl;

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
