#include <SFML/Graphics.hpp>

#include <memory>
#include <iostream>

#include "Image.h"
#include "Camera.h"
#include "Sphere.h"
#include "Light.h"

int main()
{   
	std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>
		(sf::VideoMode(700, 700), "Advanced Tech Block 2");

	std::vector<std::unique_ptr<Geometry>> shapes;
	std::vector<std::unique_ptr<Light>> lights;

	auto light = std::make_unique<Light>();
	light->setIntensity(1);
	light->setPos(glm::vec3(0.0f, 20.0f, 0.0f));
	lights.push_back(std::move(light));

	auto light2 = std::make_unique<Light>();
	light2->setIntensity(0.5f);
	light2->setPos(glm::vec3(0.0f, -20.0f, 0.0f));
	lights.push_back(std::move(light2));

	auto sphere = std::make_unique<Sphere>(1.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -5.0f));
	sphere->setMaterialType(MaterialType::DIFFUSE_AND_GLOSSY);

	auto sphere2 = std::make_unique<Sphere>(1.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(2.0f, 0.0f, -5.0f));
	sphere2->setMaterialType(MaterialType::DIFFUSE_AND_GLOSSY);
	shapes.push_back(std::move(sphere2));

	auto sphere3= std::make_unique<Sphere>(1.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-2.0f, 0.0f, -5.0f));
	sphere3->setMaterialType(MaterialType::DIFFUSE_AND_GLOSSY);
	shapes.push_back(std::move(sphere3));

	auto mirror = std::make_unique<Sphere>(1.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 2.0f, -5.0f));
	mirror->setMaterialType(MaterialType::REFLECTION);
	shapes.push_back(std::move(mirror));
	
	auto sphere4 = std::make_unique<Sphere>(1.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -2.0f, -5.0f));
	sphere4->setMaterialType(MaterialType::REFLECTION_AND_REFRACTION);
	sphere4->setSpecularExponent(25.0f);
	shapes.push_back(std::move(sphere4));

	auto camera = std::make_unique<Camera>(90, window->getSize().x / window->getSize().y,
															  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	camera->setPos(glm::vec3(0.0f, 0.0f, 0.0f));
	
	shapes.push_back(std::move(sphere));
	
	Image image = Image(camera.get(), window->getSize().x, window->getSize().y);

	image.render(camera.get(), shapes, lights);

    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();

			if (event.type == sf::Event::KeyPressed)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				{
					image.exportImage();
				}
			}
        }

        window->clear();
		image.draw(window.get());
        window->display();
    }

	return 0;
}