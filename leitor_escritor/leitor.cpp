#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <random>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <iostream>

#include "../driver_so/driver_so.h"

const int SPEED = 10;
const int MAX_RETANGULOS = 10;
const int X_JANELA = 500;
const int Y_JANELA = 500;

void criarRetangulo(const sf::RectangleShape & submarino, std::vector<std::shared_ptr<sf::RectangleShape>> & vRetangulos, const sf::Texture & texturaLixo)
{
    if (vRetangulos.size() < MAX_RETANGULOS)
    {
        std::shared_ptr<sf::RectangleShape> ptr = std::make_shared<sf::RectangleShape>(sf::Vector2f(30.f, 30.f));

        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> uni(0, X_JANELA - 30);

        auto x = uni(rng);
        auto y = uni(rng);

        ptr->setPosition(x, y);

        sf::FloatRect boundingBox = ptr->getGlobalBounds();

        // verifica se bate no submarino
        if (boundingBox.intersects(submarino.getGlobalBounds()))
            return;

        // verifica se bate em outro retangulo
        auto it = std::find_if(vRetangulos.begin(), vRetangulos.end(), [&boundingBox](auto ptr){
            return boundingBox.intersects(ptr->getGlobalBounds());
        });

        if (it != vRetangulos.end())
            return;

    	ptr->setTexture(&texturaLixo);

        // se nao adiciona
        vRetangulos.push_back(ptr);
    }
}

void verificarColisoes(const sf::RectangleShape & submarino, std::vector<std::shared_ptr<sf::RectangleShape>> & vRetangulos)
{
    sf::FloatRect boundingBox = submarino.getGlobalBounds();

    vRetangulos.erase(
        std::remove_if(vRetangulos.begin(), vRetangulos.end(), 
            [&boundingBox](auto ptr){          
                return boundingBox.intersects(ptr->getGlobalBounds());
            }
        ),vRetangulos.end()
    );
}

void processarEvento(sf::RenderWindow & window)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                window.close();
                break;
            default:
                break;
        }
    }
}

void lerArquivo(sf::RectangleShape & submarino)
{
    int fp = open("/dev/so", O_RDWR);
    if (!(fp < 0))
    {
        char mensagem[BUF_MSG];
        ioctl(fp, IOCTL_GET_POSITION, mensagem);
        int posicao = std::atoi(mensagem);

        if (posicao != POSITION_STOP)
        {
            sf::Vector2f posicao_submarino = submarino.getPosition();
            sf::FloatRect tamanho_submarino = submarino.getGlobalBounds();

            if (posicao == POSITION_RIGHT && posicao_submarino.x + SPEED + tamanho_submarino.width <= X_JANELA)
            {
                submarino.move(sf::Vector2f(SPEED, 0));
            }
            else if (posicao == POSITION_LEFT && posicao_submarino.x - SPEED >= 0) 
            {
                submarino.move(sf::Vector2f(-SPEED, 0));
            }
            else if (posicao == POSITION_UP && posicao_submarino.y - SPEED >= 0) 
            {
                submarino.move(sf::Vector2f(0, -SPEED));
            }
            else if (posicao == POSITION_DOWN && posicao_submarino.y + SPEED + tamanho_submarino.height <= Y_JANELA) 
            {
                submarino.move(sf::Vector2f(0, SPEED));
            }
            posicao = POSITION_STOP;
            ioctl(fp, IOCTL_SET_POSITION, std::to_string(posicao).c_str());
        }
    }
    else
        std::cout << "nao conseguiu abrir o arquivo\n";
    close(fp);
}

template <typename Clock, typename Time, typename Function, typename ... Args>
void proessarEventoRelogio(Clock & relogio, const Time & delay, Function & funcao, Args & ... args)
{
    if (relogio.getElapsedTime() > delay)
    {
        funcao(args...);
        relogio.restart();
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(X_JANELA, Y_JANELA), "Joguito Leitor!");

    sf::RectangleShape submarino(sf::Vector2f(50, 50));

	sf::Texture texturaSubmarino;
	if (!texturaSubmarino.loadFromFile("assets/submarino.png"))
	{
		std::cout << "Não conseguiu abrir textura do submarino\n";
	    submarino.setFillColor(sf::Color::Black);
	}
	else submarino.setTexture(&texturaSubmarino);

	sf::Texture texturaLixo;
	if (!texturaLixo.loadFromFile("assets/lixo.png"))
		std::cout << "Não conseguiu abrir textura do lixo\n";

    std::vector<std::shared_ptr<sf::RectangleShape>> vRetangulos;

    sf::Clock relogioRetangulos, relogioColisoes, relogioMovimento;

    sf::Time delayRetangulos { sf::seconds(3) };
    sf::Time delayColisoes { sf::seconds(1) };
    sf::Time delayMovimento { sf::seconds(0.3f) };

    while (window.isOpen())
    {
        processarEvento(window);
        
        proessarEventoRelogio(relogioRetangulos, delayRetangulos, criarRetangulo, submarino, vRetangulos, texturaLixo);
        proessarEventoRelogio(relogioMovimento, delayMovimento, lerArquivo, submarino);
        proessarEventoRelogio(relogioColisoes, delayColisoes, verificarColisoes, submarino, vRetangulos);

        window.clear(sf::Color::Blue);
        
        window.draw(submarino);

        for_each(vRetangulos.begin(), vRetangulos.end(), [&window](auto ptr){
            window.draw(*ptr);
        });

        window.display();
    }

    return 0;
}
