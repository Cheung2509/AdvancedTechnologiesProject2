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
	std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>
		(sf::VideoMode(600, 600), "Advanced Tech Block 2");

	std::vector<std::shared_ptr<Geometry>> shapes;
	std::vector<std::shared_ptr<Light>> lights;

	//Create light
	auto light = std::make_shared<Light>();
	light->setIntensity(1);
	light->setPos(glm::vec3(0.0f, 0.0f, 1.0f));
	lights.emplace_back(std::move(light));

	//Initialize objects in scene
	auto model = std::make_shared<Model>("suzanne.obj", glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0));
	model->setMaterialType(MaterialType::PHONG);
	shapes.emplace_back(model);

	auto model2 = std::make_shared<Model>("suzanne.obj", glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 2.5f, 0.0f));
	shapes.emplace_back(model2);

	auto model3 = std::make_shared<Model>("suzanne.obj", glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0, -2.5f, 0.0f));
	shapes.emplace_back(model3);

	auto model4 = std::make_shared<Model>("suzanne.obj", glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(3.0f, 0.0f, 0.0f));
	shapes.emplace_back(model4);

	auto model5 = std::make_shared<Model>("suzanne.obj", glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(-3.0f, 0.0f, 0.0f));
	shapes.emplace_back(model5);

	auto camera = std::make_unique<Camera>(90, float(window->getSize().x) / float(window->getSize().y),
										   glm::vec3(0.0f, -1.0f, 0.0f),
										   glm::vec3(0.0f, 0.0f, 0.0f));

	//Build BVH
	auto sah = std::make_shared<BVH>();
	sah->buildSAH(shapes);
	
	auto image = new Image(camera.get(), window->getSize().x, window->getSize().y);
	
	auto start = std::chrono::steady_clock::now();
	image->render(camera.get(), sah, lights);
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<float> TimeSAH = end - start;

	std::cout << "Time to render with SAH: " << TimeSAH.count() << std::endl;

	image->exportImage();

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