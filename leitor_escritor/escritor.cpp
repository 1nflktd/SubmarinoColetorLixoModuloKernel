#include <SFML/Graphics.hpp>
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string>
#include <map>
#include <cassert>
#include <memory>

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

std::map<int, std::unique_ptr<sf::Texture>> carregarTexturasPadrao()
{
    std::map<int, std::unique_ptr<sf::Texture>> texturasPadrao;
    texturasPadrao.emplace(POSITION_UP, std::make_unique<sf::Texture>());
    texturasPadrao.emplace(POSITION_DOWN, std::make_unique<sf::Texture>());
    texturasPadrao.emplace(POSITION_RIGHT, std::make_unique<sf::Texture>());
    texturasPadrao.emplace(POSITION_LEFT, std::make_unique<sf::Texture>());
	
	assert(texturasPadrao[POSITION_UP]->loadFromFile("assets/tecla_para_cima.png"));
	assert(texturasPadrao[POSITION_DOWN]->loadFromFile("assets/tecla_para_baixo.png"));
	assert(texturasPadrao[POSITION_RIGHT]->loadFromFile("assets/tecla_para_direita.png"));
	assert(texturasPadrao[POSITION_LEFT]->loadFromFile("assets/tecla_para_esquerda.png"));

	return texturasPadrao;
}

std::map<int, std::unique_ptr<sf::Texture>> carregarTexturasPressed()
{
    std::map<int, std::unique_ptr<sf::Texture>> texturasPressed;
    texturasPressed.emplace(POSITION_UP, std::make_unique<sf::Texture>());
    texturasPressed.emplace(POSITION_DOWN, std::make_unique<sf::Texture>());
    texturasPressed.emplace(POSITION_RIGHT, std::make_unique<sf::Texture>());
    texturasPressed.emplace(POSITION_LEFT, std::make_unique<sf::Texture>());
	
	assert(texturasPressed[POSITION_UP]->loadFromFile("assets/tecla_para_cima_pressed.png"));
	assert(texturasPressed[POSITION_DOWN]->loadFromFile("assets/tecla_para_baixo_pressed.png"));
	assert(texturasPressed[POSITION_RIGHT]->loadFromFile("assets/tecla_para_direita_pressed.png"));
	assert(texturasPressed[POSITION_LEFT]->loadFromFile("assets/tecla_para_esquerda_pressed.png"));

	return texturasPressed;	
}

std::map<int, std::unique_ptr<sf::RectangleShape>> carregarRetangulosTeclas(std::map<int, std::unique_ptr<sf::Texture>> & texturasPadrao)
{
    std::map<int, std::unique_ptr<sf::RectangleShape>> teclas;
    teclas.emplace(POSITION_UP, std::make_unique<sf::RectangleShape>(sf::Vector2f(120, 120)));
    teclas.emplace(POSITION_DOWN, std::make_unique<sf::RectangleShape>(sf::Vector2f(120, 120)));
    teclas.emplace(POSITION_RIGHT, std::make_unique<sf::RectangleShape>(sf::Vector2f(120, 120)));
    teclas.emplace(POSITION_LEFT, std::make_unique<sf::RectangleShape>(sf::Vector2f(120, 120)));

    teclas[POSITION_UP]->setTexture(texturasPadrao[POSITION_UP].get());
    teclas[POSITION_DOWN]->setTexture(texturasPadrao[POSITION_DOWN].get());
    teclas[POSITION_RIGHT]->setTexture(texturasPadrao[POSITION_RIGHT].get());
    teclas[POSITION_LEFT]->setTexture(texturasPadrao[POSITION_LEFT].get());

    teclas[POSITION_UP]->setPosition(125, 5);
    teclas[POSITION_DOWN]->setPosition(125, 125);
    teclas[POSITION_RIGHT]->setPosition(245, 125);
    teclas[POSITION_LEFT]->setPosition(5, 125);

	return teclas;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(370, 250), "Coletor de lixo. Escritor!");

    // clock para ler teclas
	sf::Clock relogioMovimento;
    sf::Time delayMovimento { sf::seconds(0.3f) };

    std::map<int, std::unique_ptr<sf::Texture>> texturasPadrao = carregarTexturasPadrao();
    std::map<int, std::unique_ptr<sf::Texture>> texturasPressed = carregarTexturasPressed();
    
    std::map<int, std::unique_ptr<sf::RectangleShape>> teclas = carregarRetangulosTeclas(texturasPadrao);

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
		    	teclas[ultimaPosicao]->setTexture(texturasPadrao[ultimaPosicao].get());
		    
	    	if (posicaoAtual != POSITION_STOP)
		    	teclas[posicaoAtual]->setTexture(texturasPressed[posicaoAtual].get());
			
	    	ultimaPosicao = posicaoAtual;
	    }

        window.clear(sf::Color::White);

        for_each(teclas.begin(), teclas.end(), [&window](auto & tecla){
            window.draw(*tecla.second);
        });

        window.display();
    }

    return 0;
}