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

	auto light = std::make_shared<Light>();
	light->setIntensity(1);
	light->setPos(glm::vec3(0.0f, 10.0f, 5.0f));
	lights.emplace_back(std::move(light));

	/*std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_real<float> x(-20, 20);
	std::uniform_real<float> z(-30, -20);
	std::uniform_real<float> y(-20, 20);
	std::uniform_real<float> col(0, 1.0f);

	for (int i = 0; i < 100; i++)
	{
		auto sphere = std::make_shared<Sphere>(0.5f, glm::vec3(col(rng), col(rng), col(rng)),
											   glm::vec3(x(rng), y(rng), -10));
		sphere->setMaterialType(MaterialType::DIFFUSE_AND_GLOSSY);
		shapes.emplace_back(std::move(sphere));
	}
*/
	auto model = std::make_shared<Model>("teapot.obj", glm::vec3(1.0f, 0.0f, 0.0f));
	shapes.push_back(model);

	auto camera = std::make_unique<Camera>(90, float(window->getSize().x) / float(window->getSize().y),
										   glm::vec3(0.0f, 1.0f, 0.0f),
										   glm::vec3(0.0f, 0.0f, 1.0f));
	camera->setPos(glm::vec3(0.0f, 0.0f, 5.0f));

	auto bvh = std::make_shared<BVH>();
	bvh->buildSAH(shapes);
	
	auto image = new Image(camera.get(), window->getSize().x, window->getSize().y);
	

	auto start = std::chrono::steady_clock::now();
	image->render(camera.get(), bvh, lights);
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<float> TimeBVH = end - start;

	std::cout << "Time to render with BVH: " << TimeBVH.count() << "Seconds." << std::endl;

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