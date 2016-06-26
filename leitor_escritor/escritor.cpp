#include <SFML/Graphics.hpp>
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string>
#include <map>
#include <cassert>

#include "../driver_so/driver_so.h"

int verificarTeclaPressionada()
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
	return posicao;
}

std::map<int, sf::Texture> carregarTexturasPadrao()
{
    std::map<int, sf::Texture> texturasPadrao;
    texturasPadrao.emplace(POSITION_UP, sf::Texture{});
    texturasPadrao.emplace(POSITION_DOWN, sf::Texture{});
    texturasPadrao.emplace(POSITION_RIGHT, sf::Texture{});
    texturasPadrao.emplace(POSITION_LEFT, sf::Texture{});
	
	assert(texturasPadrao[POSITION_UP].loadFromFile("assets/tecla_para_cima.png"));
	assert(texturasPadrao[POSITION_DOWN].loadFromFile("assets/tecla_para_baixo.png"));
	assert(texturasPadrao[POSITION_RIGHT].loadFromFile("assets/tecla_para_direita.png"));
	assert(texturasPadrao[POSITION_LEFT].loadFromFile("assets/tecla_para_esquerda.png"));

	return texturasPadrao;
}

std::map<int, sf::Texture> carregarTexturasPressed()
{
    std::map<int, sf::Texture> texturasPressed;
    texturasPressed.emplace(POSITION_UP, sf::Texture{});
    texturasPressed.emplace(POSITION_DOWN, sf::Texture{});
    texturasPressed.emplace(POSITION_RIGHT, sf::Texture{});
    texturasPressed.emplace(POSITION_LEFT, sf::Texture{});
	
	assert(texturasPressed[POSITION_UP].loadFromFile("assets/tecla_para_cima_pressed.png"));
	assert(texturasPressed[POSITION_DOWN].loadFromFile("assets/tecla_para_baixo_pressed.png"));
	assert(texturasPressed[POSITION_RIGHT].loadFromFile("assets/tecla_para_direita_pressed.png"));
	assert(texturasPressed[POSITION_LEFT].loadFromFile("assets/tecla_para_esquerda_pressed.png"));

	return texturasPressed;	
}

std::map<int, sf::RectangleShape> carregarRetangulosTeclas(std::map<int, sf::Texture> & texturasPadrao)
{
    std::map<int, sf::RectangleShape> teclas;
    teclas.emplace(POSITION_UP, sf::RectangleShape{ sf::Vector2f(120, 120) });
    teclas.emplace(POSITION_DOWN, sf::RectangleShape{ sf::Vector2f(120, 120) });
    teclas.emplace(POSITION_RIGHT, sf::RectangleShape{ sf::Vector2f(120, 120) });
    teclas.emplace(POSITION_LEFT, sf::RectangleShape{ sf::Vector2f(120, 120) });

    teclas[POSITION_UP].setTexture(&texturasPadrao[POSITION_UP]);
    teclas[POSITION_DOWN].setTexture(&texturasPadrao[POSITION_DOWN]);
    teclas[POSITION_RIGHT].setTexture(&texturasPadrao[POSITION_RIGHT]);
    teclas[POSITION_LEFT].setTexture(&texturasPadrao[POSITION_LEFT]);

    teclas[POSITION_UP].setPosition(125, 5);
    teclas[POSITION_DOWN].setPosition(125, 125);
    teclas[POSITION_RIGHT].setPosition(245, 125);
    teclas[POSITION_LEFT].setPosition(5, 125);

	return teclas;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(370, 250), "Coletor de lixo. Escritor!");

    // clock para ler teclas
	sf::Clock relogioMovimento;
    sf::Time delayMovimento { sf::seconds(0.3f) };

    std::map<int, sf::Texture> texturasPadrao = carregarTexturasPadrao();
    std::map<int, sf::Texture> texturasPressed = carregarTexturasPressed();
    
    std::map<int, sf::RectangleShape> teclas = carregarRetangulosTeclas(texturasPadrao);

    int posicaoAtual = POSITION_STOP;
    int ultimaPosicao = POSITION_STOP;
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
					        posicaoAtual = verificarTeclaPressionada();
					        relogioMovimento.restart();
					    }
		                break;
	            	}
	            default:
	            	posicaoAtual = POSITION_STOP;
	            	break;
	        }
	    }

	    if (posicaoAtual != ultimaPosicao)
	    {
	    	if (ultimaPosicao != POSITION_STOP)
		    	teclas[ultimaPosicao].setTexture(&texturasPadrao[ultimaPosicao]);

	    	if (posicaoAtual != POSITION_STOP)
		    	teclas[posicaoAtual].setTexture(&texturasPressed[posicaoAtual]);
	    	else
		    	teclas[posicaoAtual].setTexture(&texturasPadrao[posicaoAtual]);

	    	ultimaPosicao = posicaoAtual;
	    }

        window.clear(sf::Color::White);

        for_each(teclas.begin(), teclas.end(), [&window](auto tecla){
            window.draw(tecla.second);
        });

        window.display();
    }

    return 0;
}