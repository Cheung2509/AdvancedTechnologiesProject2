#include <SFML/Graphics.hpp>

#include <memory>

#include "Image.h"
#include "Camera.h"
#include "Sphere.h"

int main()
{
    std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>
		(sf::VideoMode(400, 400), "Advanced Tech Block 2");

	std::vector<Shape> shapes;
	Sphere sphere = Sphere(1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	sphere.setPos(glm::vec3(2.0f, 0.0f, 0.0f));
	
		
	std::shared_ptr<Camera> camera = std::make_shared<Camera>(30, 
															  window->getSize().x/window->getSize().y,
															  glm::vec3(0.0f,1.0f,0.0f), sphere.getPos());
	shapes.emplace_back(sphere);
	


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
					//image->exportImage();
				}
			}
        }

        window->clear();
        window->display();
    }
    
	return 0;
}