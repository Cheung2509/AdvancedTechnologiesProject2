#include <SFML/Graphics.hpp>

#include <memory>
#include <iostream>

#include "Image.h"
#include "Camera.h"
#include "Sphere.h"
#include "Light.h"
#include "BoundingVolumeHierachy.h"

int main()
{   
	std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>
		(sf::VideoMode(700, 700), "Advanced Tech Block 2");

	std::vector<std::shared_ptr<Geometry>> shapes;
	std::vector<std::shared_ptr<Light>> lights;

	auto light = std::make_shared<Light>();
	light->setIntensity(1);
	light->setPos(glm::vec3(0.0f, 10.0f, -4.0f));
	lights.emplace_back(std::move(light));

	auto sphere = std::make_shared<Sphere>(1.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -6.0f));
	sphere->setMaterialType(MaterialType::DIFFUSE_AND_GLOSSY);
	shapes.emplace_back(std::move(sphere));
	
	auto sphere2 = std::make_shared<Sphere>(1.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(2.0f, 0.0f, -5.0f));
	sphere2->setMaterialType(MaterialType::DIFFUSE_AND_GLOSSY);
	shapes.emplace_back(std::move(sphere2));

	auto sphere3= std::make_shared<Sphere>(1.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 0.0f, -4.0f));
	sphere3->setMaterialType(MaterialType::DIFFUSE_AND_GLOSSY);
	shapes.emplace_back(std::move(sphere3));

	auto sphere4 = std::make_shared<Sphere>(1.0f, glm::vec3(0.75f, 0.75f, 0.75f), glm::vec3(2.0f, -2.0f, -5.0f));
	sphere4->setMaterialType(MaterialType::DIFFUSE_AND_GLOSSY);
	sphere4->setSpecularExponent(25.0f);
	shapes.emplace_back(std::move(sphere4));

	/*auto sphere5 = std::make_shared<Sphere>(1.0f, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(-2.0f, -2.0f, -5.0f));
	sphere5->setMaterialType(MaterialType::DIFFUSE_AND_GLOSSY);
	shapes.emplace_back(std::move(sphere5));*/

	auto camera = std::make_unique<Camera>(90, window->getSize().x / window->getSize().y,
															  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	camera->setPos(glm::vec3(0.0f, 0.0f, 0.0f));

	BVH bvh;
	bvh.build(shapes);
	
	
	
	auto image = new Image(camera.get(), window->getSize().x, window->getSize().y);

	image->render(camera.get(), shapes, lights);

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
			}
        }

        window->clear();
		image->draw(window.get());
        window->display();
    }

	delete image;

	return 0;
}