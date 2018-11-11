#include <SFML/Graphics.hpp>

#include <memory>

#include "Image.h"

int main()
{
    std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>
		(sf::VideoMode(400, 400), "SFML works!");

	std::unique_ptr<Image> image = std::make_unique<Image>(window->getSize().x , window->getSize().y);

	for (int x = 0; x < image->getSize().x; x++)
	{
		for (int y = 0; y < image->getSize().y; y++)
		{
			if (x < (image->getSize().x / 3))
			{
				image->putPixel(sf::Vector2u(x, y), sf::Color::Red);
			}
			else if (x < (image->getSize().x / 3) * 2)
			{
				image->putPixel(sf::Vector2u(x, y), sf::Color::Yellow);
			}
			else
			{
				image->putPixel(sf::Vector2u(x, y), sf::Color::Green);
			}
			
		}
	}

	

    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();
        }

        window->clear();
		image->draw(window.get());
        window->display();
    }

	image->exportImage();
    
	return 0;
}