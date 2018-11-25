#include <SFML/Graphics.hpp>

#include <memory>
#include <iostream>
#include <random>

#include "Image.h"
#include "Camera.h"
#include "Sphere.h"
#include "Light.h"
#include "BoundingVolumeHierachy.h"

int main()
{   
	std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>
		(sf::VideoMode(600, 600), "Advanced Tech Block 2");

	std::vector<std::shared_ptr<Geometry>> shapes;
	std::vector<std::shared_ptr<Light>> lights;

	auto light = std::make_shared<Light>();
	light->setIntensity(1);
	light->setPos(glm::vec3(0.0f, 10.0f, -4.0f));
	lights.emplace_back(std::move(light));

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_real<float> x(-50, 50);
	std::uniform_real<float> z(-50, 50);
	std::uniform_real<float> y(-50, 50);
	std::uniform_real<float> col(0, 1.0f);

	for (int i = 0; i < 1000; i++)
	{
		auto sphere = std::make_shared<Sphere>(1.0f, glm::vec3(col(rng), col(rng), col(rng)),
											   glm::vec3(x(rng), y(rng), z(rng)));
		sphere->setMaterialType(MaterialType::DIFFUSE_AND_GLOSSY);
		shapes.emplace_back(std::move(sphere));
	}

	auto camera = std::make_unique<Camera>(90, float(window->getSize().x) / float(window->getSize().y),
															  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	camera->setPos(glm::vec3(0.0f, 0.0f, 0.0f));

	auto bvh = std::make_shared<BVH>();
	bvh->build(shapes);
	
	auto image = new Image(camera.get(), window->getSize().x, window->getSize().y);

	image->render(camera.get(), bvh, lights);

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
					image->exportImage();
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				{
					window->close();
				}
			}
        }

        window->clear();
		image->draw(window.get());
        window->display();
    }

	delete image;

	return 0;
}