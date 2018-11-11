#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Image
{
public:
	Image(unsigned int sizeX, unsigned int sizeY);
	~Image();

	void putPixel(sf::Vector2u pos, sf::Color colour);

	void exportImage();
	void draw(sf::RenderWindow* renderWindow);

	const sf::Vector2u& getSize() const { return m_imageSize; }
private:
	sf::Vector2u m_imageSize;
	std::vector<sf::Uint8> m_pixels;
};