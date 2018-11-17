#include "Image.h"

#include <fstream>

Image::Image(unsigned int sizeX, unsigned int sizeY) : m_imageSize(sizeX, sizeY)
{
	m_pixels.reserve(m_imageSize.x * m_imageSize.y * 4);
	
	for (int i = 0; i < m_imageSize.x * m_imageSize.y * 4; i++)
	{
		m_pixels.emplace_back(0);
	}
}

void Image::putPixel(sf::Vector2u pos, sf::Color colour)
{
	m_pixels[(m_imageSize.x * pos.x + pos.y) * 4] = colour.r;
	m_pixels[(m_imageSize.x * pos.x + pos.y) * 4 + 1] = colour.g;
	m_pixels[(m_imageSize.x * pos.x + pos.y) * 4 + 2] = colour.b;
}

void Image::putPixel(sf::Vector2u pos, glm::vec3 colour)
{
	m_pixels[(m_imageSize.x * pos.x + pos.y) * 4] = glm::min(1.0f, colour.x) * 255;
	m_pixels[(m_imageSize.x * pos.x + pos.y) * 4 + 1] = glm::min(1.0f, colour.y) * 255;
	m_pixels[(m_imageSize.x * pos.x + pos.y) * 4 + 2] = glm::min(1.0f, colour.z) * 255;
	m_pixels[(m_imageSize.x * pos.x + pos.y) * 4 + 3] = 1 * 255;
}

bool Image::exportImage()
{
	sf::Image image;
	image.create(m_imageSize.x, m_imageSize.y, m_pixels.data());

	if (image.saveToFile("Test.png"))
	{
		return true;
	}

	return false;
}

void Image::draw(sf::RenderWindow* renderWindow)
{

	sf::Image image;
	image.create(m_imageSize.x, m_imageSize.y, m_pixels.data());
	
	sf::Texture texture;
	texture.loadFromImage(image);

	sf::Sprite sprite;
	sprite.setTexture(texture);

	renderWindow->draw(sprite);
}
