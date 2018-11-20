#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <glm/glm.hpp>

#include <vector>

class Geometry;
class Light;
class Camera;

struct ImageData
{
	glm::u64vec2 m_size;
	std::uint8_t m_maxDepth = 5;
	glm::vec3 m_backgroundColour = glm::vec3(1.0f, 1.0f, 1.0f);
	float m_bias = (float)0.00001;
	float m_aspectRatio;
	float m_fov;
};

class Image
{
public:
	Image(Camera* camera, unsigned int sizeX = 640, unsigned int sizeY = 480);
	~Image() = default;

	void putPixel(glm::u64vec2 pos, sf::Color colour);
	void putPixel(glm::u64vec2 pos, glm::vec3 colour);

	void render(Camera* camera, const std::vector<std::unique_ptr<Geometry>>& shapes,
				const std::vector<std::unique_ptr<Light>>& lights);

	bool createImage();

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