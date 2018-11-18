#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <glm/glm.hpp>

#include <vector>

class Shape;
class Light;
class Camera;

struct ImageData
{
	glm::u64vec2 m_size;
	std::uint8_t m_maxDepth = 5;
	glm::vec3 m_backgroundColour = glm::vec3(0.0f, 0.0f, 0.0f);
	float m_bias = 0.00001;
	float m_aspectRatio;
	float m_fov;
};

class Image
{
public:
	Image(Camera* camera, unsigned int sizeX = 640, unsigned int sizeY = 480);
	~Image() = default;

	void putPixel(sf::Vector2u pos, sf::Color colour);
	void putPixel(sf::Vector2u pos, glm::vec3 colour);

	//void createImage(Camera* camera, const std::vector<std::unique_ptr<Shape>>& shapes);
	void render(Camera* camera, const std::vector<std::unique_ptr<Shape>>& shapes,
				const std::vector<std::unique_ptr<Light>>& lights);

	bool exportImage();
	void draw(sf::RenderWindow* renderWindow);

	const glm::u64vec2& getSize() const { return m_imageData.m_size; }
private:
	ImageData m_imageData;
	std::vector<float*> m_pixels;

	sf::Image m_image;
	sf::Texture m_texture;
	sf::Sprite m_sprite;
};