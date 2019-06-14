#include <SFML/Graphics.hpp>

#include <memory>
#include <iostream>
#include <random>
#include <chrono>

#include "Image.h"
#include "Camera.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Model.h"
#include "Light.h"
#include "BoundingVolumeHierachy.h"

int main()
{   
	std::atomic<sf::RenderWindow*> window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Advanced Tech Block 2");

	std::vector<std::shared_ptr<Geometry>> shapes;
	std::vector<std::shared_ptr<Light>> lights;

	//Create light
	auto light = std::make_shared<Light>();
	light->setIntensity(1);
	light->setPos(glm::vec3(0.0f, 0.0f, 1.0f));
	lights.emplace_back(std::move(light));

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_real<float> col(0, 1.0f);

	//Initialize objects in scene
	for(int x = -2; x < 2; x++)
	{
		for(int y = -2; y < 2; y++)
		{
				auto model = std::make_shared<Model>("suzanne.obj", glm::vec3(col(rng), col(rng), col(rng)),
													 glm::vec3(x* 3, y * 3, 0.0f));
				shapes.emplace_back(model);
		}
	}

	auto camera = std::make_unique<Camera>(90, float(window.load()->getSize().x) / float(window.load()->getSize().y),
										   glm::vec3(0.0f, -1.0f, 0.0f),
										   glm::vec3(0.0f, 0.0f, 0.0f));

	//Build BVH
	auto sah = std::make_shared<BVH>();
	sah->buildSAH(shapes);
	
	auto image = new Image(camera.get(), window.load()->getSize().x, window.load()->getSize().y);
	image->render(camera.get(), sah, lights, window);


	//image->exportImage();

    while (window.load()->isOpen())
    {
        sf::Event event;
        while (window.load()->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.load()->close();

			if (event.type == sf::Event::KeyPressed)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				{
					image->exportImage();
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				{
					window.load()->close();
				}
			}
        }

        window.load()->clear();
		image->draw(window);
        window.load()->display();
    }

	delete image;
	delete window;

	return 0;
}