#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(600, 600), "New Window");
    while (window.isOpen()) {

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

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();

        }

        window.clear();

        sf::RectangleShape shape(sf::Vector2f(200, 200));
        window.draw(shape);

        window.display();
    }

    return 0;
}