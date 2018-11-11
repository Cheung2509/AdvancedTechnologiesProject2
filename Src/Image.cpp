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

Image::~Image()
{
}

void Image::putPixel(sf::Vector2u pos, sf::Color colour)
{
	m_pixels[(m_imageSize.x * pos.x + pos.y) * 4] = colour.r;
	m_pixels[(m_imageSize.x * pos.x + pos.y) * 4 + 1] = colour.g;
	m_pixels[(m_imageSize.x * pos.x + pos.y) * 4 + 2] = colour.b;
	m_pixels[(m_imageSize.x * pos.x + pos.y) * 4 + 3] = colour.a;
}

void Image::exportImage()
{
	sf::Image image;
	image.create(m_imageSize.x, m_imageSize.y, m_pixels.data());

	image.saveToFile("Test.png");
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
