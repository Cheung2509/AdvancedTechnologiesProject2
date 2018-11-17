#include <SFML/Graphics.hpp>

#include <memory>
#include <iostream>

#include "Image.h"
#include "Camera.h"
#include "Sphere.h"
#include "Ray.h"

#define M_PI 3.141592653589793

int main()
{
    std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>
		(sf::VideoMode(700, 700), "Advanced Tech Block 2");

	std::vector<Shape*> shapes;
	Sphere* sphere = new Sphere(2.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.0f);
	sphere->setPos(glm::vec3(0.0f, 0.0f, -5.0f));
	shapes.emplace_back(sphere);
	
	Sphere* sphere2 = new Sphere(2.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.0f);
	sphere2->setPos(glm::vec3(-5.0f, 0.0f, -5.0f));
	shapes.push_back(sphere2);

	Sphere* sphere3 = new Sphere(2.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.0f);
	sphere3->setPos(glm::vec3(0.0f, -4.0f, -5.0f));
	shapes.push_back(sphere3);
	
	Sphere* light = new Sphere(3.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	light->setPos(glm::vec3(20.0f, 20.0f, 30.0f));
	shapes.emplace_back(light);
		
	std::shared_ptr<Camera> camera = std::make_shared<Camera>(30,
															  window->getSize().x / window->getSize().y,
															  glm::vec3(0.0f, 1.0f, 0.0f), sphere->getPos());

	
	
	Image image = Image(window->getSize().x, window->getSize().y);

	float invWidth = 1 / float(image.getSize().x);
	float invHeight = 1 / float(image.getSize().y);
	float angle = glm::angle(camera->getRotation());
	
	for (int x = 0; x < image.getSize().x; x++)
	{
		for (int y = 0; y < image.getSize().y; y++)
		{
			float xx = (2 * ((x + 0.5f) * invWidth) - 1) * angle* camera->getAspectRatio();
			float yy = (1 - 2 * ((y + 0.5f)* invHeight)) * angle;
			glm::vec3 rayDir = glm::normalize(glm::vec3(xx, yy, -1.0f));

			image.putPixel(sf::Vector2u(x, y), Ray::trace(camera->getPos(), rayDir, shapes, 0));
		}
	}

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
    
	for (auto& obj : shapes)
	{
		delete obj;
	}

	return 0;
}