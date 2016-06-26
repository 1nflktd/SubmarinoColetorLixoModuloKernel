#include <SFML/Graphics.hpp>
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string>

#include "../driver_so/driver_so.h"

void verificarTeclaPressionada()
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

}

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "Joguito Escritor!");
	sf::Clock relogioMovimento;
    sf::Time delayMovimento { sf::seconds(0.3f) };

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
            		    if (relogioMovimento.getElapsedTime() > delayMovimento)
						{
					        verificarTeclaPressionada();
					        relogioMovimento.restart();
					    }
		                break;
	            	}
	            default:
	            	break;
	        }
	    }

        window.clear(sf::Color::White);

        window.display();
    }

    return 0;
}