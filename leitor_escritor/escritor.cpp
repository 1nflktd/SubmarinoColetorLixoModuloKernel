#include <SFML/Graphics.hpp>
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string>

#include "../driver_so/driver_so.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "Joguito Escritor!");

    while (window.isOpen())
    {
	    sf::Event event;
	    while (window.pollEvent(event))
	    {
	        switch (event.type)
	        {
	            case sf::Event::Closed:
	                window.close();
	                break;
	            case sf::Event::KeyPressed:
	            	{
		            	int posicao = -1;
		                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		                {
							posicao = POSITION_LEFT;
		                }
		                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) 
		                {
		                	posicao = POSITION_RIGHT;
		                }
		                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) 
						{
		                	posicao = POSITION_UP;
		                }
		                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) 
		                {
		                	posicao = POSITION_DOWN;
		                }
		                else
		                {
		                	posicao = POSITION_STOP;
		                }

		                if (posicao > -1)
		                {
		                	int fp = open("/dev/so", O_RDWR);
		                	if (!(fp < 0))
							{
								ioctl(fp, IOCTL_SET_POSITION, std::to_string(posicao).c_str());
							}
							close(fp);
		                }
		                break;
	            	}
	            default:
	            	break;
	        }
	    }

        window.clear(sf::Color::Blue);

        window.display();
    }

    return 0;
}