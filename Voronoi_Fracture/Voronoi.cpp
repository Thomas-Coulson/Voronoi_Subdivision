#include "Voronoi.h"

Voronoi::Voronoi()
{
    m_window = new sf::RenderWindow(sf::VideoMode(1000, 1000), "Voronoi Subdivision");

    for (int i = 0; i < m_totalFeaturePoints; i++)
    {
        FeaturePoint featurepoint;
        RandomiseFeaturePointPosition(featurepoint);
        m_featurePoints.push_back(featurepoint);
    }
    
}

void Voronoi::Run()
{
    srand(time(NULL));

    while (m_window->isOpen()) {

        sf::Event event;
        while (m_window->pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                m_window->close();
                break;
            default:
                break;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                for (FeaturePoint point : m_featurePoints)
                {
                    RandomiseFeaturePointPosition(point);
                }
            }
        }

        //update
        Update();

        //clear
        m_window->clear();
        //draw
        Draw(*m_window);

        //display
        m_window->display();
    }
}

void Voronoi::Update()
{

}

void Voronoi::RandomiseFeaturePointPosition(FeaturePoint& featurePoint)
{
    featurePoint.position.x = rand() % 780 + m_borderWidth + 10;
    featurePoint.position.y = rand() % 780 + m_borderWidth + 10;
}

void Voronoi::Draw(sf::RenderWindow& window)
{
    //draw bounding square
    sf::RectangleShape shape(sf::Vector2f(m_cubeSize, m_cubeSize));
    shape.setPosition(sf::Vector2f(m_borderWidth, m_borderWidth));
    shape.setFillColor(sf::Color::Black);
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2);
    window.draw(shape);

    //draw dots
    for (FeaturePoint point : m_featurePoints)
    {
        sf::CircleShape dot(5);
        dot.setPosition(point.position);
        dot.setFillColor(sf::Color::White);

        window.draw(dot);
    }

}