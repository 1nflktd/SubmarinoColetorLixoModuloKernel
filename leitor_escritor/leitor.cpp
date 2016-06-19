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

void criarRetangulo(const sf::CircleShape & circulo, std::vector<std::shared_ptr<sf::RectangleShape>> & vRetangulos)
{
    if (vRetangulos.size() < MAX_RETANGULOS)
    {
        std::shared_ptr<sf::RectangleShape> ptr = std::make_shared<sf::RectangleShape>(sf::Vector2f(30.f, 30.f));
        ptr->setFillColor(sf::Color::Green);

        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> uni(0, X_JANELA - 30);

        auto x = uni(rng);
        auto y = uni(rng);

        ptr->setPosition(x, y);

        sf::FloatRect boundingBox = ptr->getGlobalBounds();

        // verifica se bate no circulo
        if (boundingBox.intersects(circulo.getGlobalBounds()))
            return;

        // verifica se bate em outro retangulo
        auto it = std::find_if(vRetangulos.begin(), vRetangulos.end(), [&boundingBox](auto ptr){
            return boundingBox.intersects(ptr->getGlobalBounds());
        });

        if (it != vRetangulos.end())
            return;

        // se nao adiciona
        vRetangulos.push_back(ptr);
    }
}

void verificarColisoes(const sf::CircleShape & circulo, std::vector<std::shared_ptr<sf::RectangleShape>> & vRetangulos)
{
    sf::FloatRect boundingBox = circulo.getGlobalBounds();

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

void lerArquivo(sf::CircleShape & circulo)
{
    int fp = open("/dev/so", O_RDWR);
    if (!(fp < 0))
    {
        char mensagem[BUF_MSG];
        ioctl(fp, IOCTL_GET_POSITION, mensagem);
        int posicao = std::atoi(mensagem);

        if (posicao != POSITION_STOP)
        {
            sf::Vector2f posicao_circulo = circulo.getPosition();
            sf::FloatRect tamanho_circulo = circulo.getGlobalBounds();

            if (posicao == POSITION_RIGHT && posicao_circulo.x + SPEED + tamanho_circulo.width <= X_JANELA)
            {
                circulo.move(sf::Vector2f(SPEED, 0));
            }
            else if (posicao == POSITION_LEFT && posicao_circulo.x - SPEED >= 0) 
            {
                circulo.move(sf::Vector2f(-SPEED, 0));
            }
            else if (posicao == POSITION_UP && posicao_circulo.y - SPEED >= 0) 
            {
                circulo.move(sf::Vector2f(0, -SPEED));
            }
            else if (posicao == POSITION_DOWN && posicao_circulo.y + SPEED + tamanho_circulo.height <= Y_JANELA) 
            {
                circulo.move(sf::Vector2f(0, SPEED));
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

    sf::CircleShape circulo(30.f);
    circulo.setFillColor(sf::Color::Black);

    std::vector<std::shared_ptr<sf::RectangleShape>> vRetangulos;

    sf::Clock relogioRetangulos, relogioColisoes, relogioMovimento;

    sf::Time delayRetangulos { sf::seconds(3) };
    sf::Time delayColisoes { sf::seconds(1) };
    sf::Time delayMovimento { sf::seconds(0.1f) };

    while (window.isOpen())
    {
        processarEvento(window);
        
        proessarEventoRelogio(relogioRetangulos, delayRetangulos, criarRetangulo, circulo, vRetangulos);
        proessarEventoRelogio(relogioMovimento, delayMovimento, lerArquivo, circulo);
        proessarEventoRelogio(relogioColisoes, delayColisoes, verificarColisoes, circulo, vRetangulos);

        window.clear(sf::Color::Blue);
        
        window.draw(circulo);

        for_each(vRetangulos.begin(), vRetangulos.end(), [&window](auto ptr){
            window.draw(*ptr);
        });

        window.display();
    }

    return 0;
}
