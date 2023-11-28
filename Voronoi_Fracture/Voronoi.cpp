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

    int voxelWidth = m_cubeSize / m_voxelSubdivisionSize;//200
    int voxelHeight = m_cubeSize / m_voxelSubdivisionSize;//200

    for (int y = 0; y < voxelHeight; y++)
    {
        for (int x = 0; x < voxelWidth; x++)
        {
            int voxelX = x * m_voxelSubdivisionSize + (m_voxelSubdivisionSize / 2) + m_borderWidth; // position of midpoint in subdivision
            int voxelY = y * m_voxelSubdivisionSize + (m_voxelSubdivisionSize / 2) + m_borderWidth; // position of midpoint in subdivision
            m_voxelSubdivision.push_back(sf::Vector2f(voxelX, voxelY));
        }
    }
    
    std::cout << "Number of voxels = " << m_voxelSubdivision.size() << std::endl;;
    
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
                for (auto& point : m_featurePoints)
                {
                    RandomiseFeaturePointPosition(point);
                }
                m_startedSim = false;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                m_sampleSize += 1;
                std::cout << "New Sample Size = " << m_sampleSize << std::endl;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                m_sampleSize -= 1;
                std::cout << "New Sample Size = " << m_sampleSize << std::endl;
            }

            if (sf::Keyboard::isKeyPressed (sf::Keyboard::Enter))
            {
                //start algorithm
                if (!m_startedSim)
                {
                    FindVertecies();
                    std::cout << "started simulation" << std::endl;
                    m_startedSim = true;
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

void Voronoi::FindVertecies()
{
    m_voxelFeaturePoints.clear();

    //find voxels that contain the feature points
    for (auto& voxelPos : m_voxelSubdivision)
    {
        for (auto& featurePoint : m_featurePoints)
        {
            if (featurePoint.position.x >= (voxelPos.x - (m_voxelSubdivisionSize / 2)) && featurePoint.position.x < voxelPos.x + (m_voxelSubdivisionSize / 2)
                && featurePoint.position.y >= voxelPos.y - (m_voxelSubdivisionSize / 2) && featurePoint.position.y < voxelPos.y + (m_voxelSubdivisionSize / 2))
            {
                m_voxelFeaturePoints.push_back(voxelPos);
            }
        }
    }

    //find vertecies (points that are equidistant from 3 feature points)

    std::cout << "Number of feature points matched to voxel squares = " << m_voxelFeaturePoints.size() << std::endl;

}

void Voronoi::RandomiseFeaturePointPosition(FeaturePoint& featurePoint)
{
    if (m_sampleSize != 0)
    {
        int totalX = 0, totalY = 0;
        for (int i = 0; i < m_sampleSize; i++)
        {
            totalX = totalX + ((rand() % (m_cubeSize - 1)) + m_borderWidth);
            totalY = totalY + ((rand() % (m_cubeSize - 1)) + m_borderWidth);
        }
        featurePoint.position.x = totalX / m_sampleSize;
        featurePoint.position.y = totalY / m_sampleSize;
    }
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

    DrawVoxels(window);

    DrawFeaturePoints(window);
}

void Voronoi::DrawFeaturePoints(sf::RenderWindow& window)
{
    //draw dots
    for (FeaturePoint point : m_featurePoints)
    {
        sf::CircleShape dot(2);//radius of 2
        dot.setOrigin(sf::Vector2f(2, 2));
        dot.setPosition(point.position);
        dot.setFillColor(sf::Color::White);

        window.draw(dot);
    }
}

void Voronoi::DrawVoxels(sf::RenderWindow& window)
{
    //draw voxels as squares
    for (sf::Vector2f boxPos : m_voxelSubdivision)
    {
        sf::RectangleShape shape(sf::Vector2f(m_voxelSubdivisionSize, m_voxelSubdivisionSize));
        shape.setOrigin(sf::Vector2f(m_voxelSubdivisionSize / 2, m_voxelSubdivisionSize / 2));
        shape.setPosition(boxPos);
        shape.setFillColor(sf::Color::Black);
        shape.setOutlineColor(sf::Color(189, 189, 189, 255));
        shape.setOutlineThickness(1);
        window.draw(shape);
    }
    
}