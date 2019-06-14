#include "Image.h"

#include <iostream>
#include <glm/glm.hpp>

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
	m_texture = new sf::Texture();
	m_sprite = new sf::Sprite();
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

	createImage();
}

void Image::render(Camera * camera, std::shared_ptr<BVH>& bvh, const std::vector<std::shared_ptr<Light>>& lights, std::atomic<sf::RenderWindow*>& window)
{
	float scale = glm::tan(glm::radians(m_imageData.m_fov * 0.5f));

	bool threading = true;
	int cores = std::thread::hardware_concurrency() - 2;
	int max = m_imageData.m_size.x * m_imageData.m_size.y;
	
	glm::mat4 model(1.0f);

	if(threading)
	{
		//Assign pixels to render for each thread
		for(int c = 0; c < cores; ++c)
		{
			m_future_vector.emplace_back(std::async([=]()
			{
				int part = m_imageData.m_size.y / cores;
				for(int i = 0; i < part; i++)
				{
					int y = part * c + i;

					for(int x = 0; x < m_imageData.m_size.x; x++)
					{
						//Un project to get direction of ray
						glm::vec3 p = glm::unProject(glm::vec3(x, y, 0), model,
													 camera->getProjection(),
													 glm::vec4(0, 0, m_imageData.m_size.x, m_imageData.m_size.y));
						Ray ray(camera->getPos(), glm::normalize(p - camera->getPos()), 0);
						putPixel(glm::u64vec2(x, y), glm::vec4(ray.castRay(bvh, lights, m_imageData), 1.0f));
					}
				}
			}));
		}

		////Wait for all threads to be done
		//for(auto& thread : future_vector)
		//{
		//	//thread.wait();
		//}
	}
	else
	{
		for(unsigned int x = 0; x < m_imageData.m_size.x; x++)
		{
			for(unsigned int y = 0; y < m_imageData.m_size.y; y++)
			{
				glm::vec3 p = glm::unProject(glm::vec3(x, y, 0), model,
											 camera->getProjection(),
											 glm::vec4(0, 0, m_imageData.m_size.x, m_imageData.m_size.y));
				Ray ray(camera->getPos(), glm::normalize(p - camera->getPos()), 0);

				//Assign colour to pixel
				putPixel(glm::u64vec2(x, y), glm::vec4(ray.castRay(bvh, lights, m_imageData), 1.0f));
			}
		}
	}

	//createImage();
}

bool Image::createImage()
{
	if (!m_texture.load()->loadFromImage(*m_image.load()))
	{
		std::cout << "Failed to load image" << std::endl;
		return false;
	}

	m_sprite.load()->setTexture(*m_texture);

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

void Image::draw(std::atomic<sf::RenderWindow*>& renderWindow)
{
	createImage();
	m_texture.load()->loadFromImage(*m_image.load());
	m_sprite.load()->setTexture(*m_texture);

	renderWindow.load()->draw(*m_sprite);
}
