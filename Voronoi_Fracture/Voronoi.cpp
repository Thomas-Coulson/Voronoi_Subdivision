#include "Voronoi.h"

Voronoi::Voronoi()
{
    srand(time(NULL));

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
    //update loop
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
                Reset();
                m_startedSim = false;

                for (auto& point : m_featurePoints)
                {
                    RandomiseFeaturePointPosition(point);
                }
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

void Voronoi::Reset()
{
    m_verticies.clear();
    m_edgePoints.clear();
    m_voxelFeaturePoints.clear();
}

void Voronoi::FindVertecies()
{
    std::cout << "Finding Verticies" << std::endl;
    //m_voxelFeaturePoints.clear();

    //find voxels that contain the feature points (Dont need this)
    for (sf::Vector2f voxelPos : m_voxelSubdivision)
    {
        for (FeaturePoint featurePoint : m_featurePoints)
        {
            if (featurePoint.position.x >= (voxelPos.x - (m_voxelSubdivisionSize / 2)) && featurePoint.position.x < voxelPos.x + (m_voxelSubdivisionSize / 2)
                && featurePoint.position.y >= voxelPos.y - (m_voxelSubdivisionSize / 2) && featurePoint.position.y < voxelPos.y + (m_voxelSubdivisionSize / 2))
            {
                m_voxelFeaturePoints.push_back(voxelPos);
            }
        }
    }

    std::cout << "Found Voxel Feature Points" << std::endl;

    //Search through voxels
    //vertex is where each of ITS NEAREST feature points are equi-distant

    std::vector<int> pointDistances;
    sf::Vector2f point1, point2, point3, midpoint;
    int dist1, dist2, dist3, currentDist;
    int xMid, yMid;

    //for each voxel, we find its closet 3 feature points
    for (sf::Vector2f voxelPoint : m_voxelSubdivision)
    {
        pointDistances.clear();
        for (sf::Vector2f voxFeaturePointDists : m_voxelFeaturePoints)
        {
            pointDistances.push_back(DistanceBetweenPoints(voxelPoint, voxFeaturePointDists));
        }

        std::sort(pointDistances.begin(), pointDistances.end());//sorts all feature point positions in ascending order

        for (sf::Vector2f voxFeaturePoint : m_voxelFeaturePoints)//points 1 2 and 3 are this voxel's 3 closest feature points
        {
            currentDist = DistanceBetweenPoints(voxelPoint, voxFeaturePoint);
            if (currentDist == pointDistances[0])
                point1 = voxFeaturePoint;
            else if (currentDist == pointDistances[1])
                point2 = voxFeaturePoint;
            else if (currentDist == pointDistances[2])
                point3 = voxFeaturePoint;
        }

        //find midpoint of trio
        xMid = (point1.x + point2.x + point3.x) / 3;// average x positions
        yMid = (point1.y + point2.y + point3.y) / 3;// average y positions
        midpoint = sf::Vector2f(xMid, yMid);

        //find midpoint in voxel space
        for (sf::Vector2f voxelPos : m_voxelSubdivision)
        {
            if (midpoint.x >= (voxelPos.x - (m_voxelSubdivisionSize / 2)) && midpoint.x < voxelPos.x + (m_voxelSubdivisionSize / 2)
                && midpoint.y >= voxelPos.y - (m_voxelSubdivisionSize / 2) && midpoint.y < voxelPos.y + (m_voxelSubdivisionSize / 2))
            {
                midpoint = voxelPos;
            }
        }

        if (midpoint == voxelPoint)
        {
            m_verticies.push_back(voxelPoint);
        }

        ///////////////////////////////////////////////////////////////////- Testing edge finding
        //find midpoint of trio
        //xMid = (point1.x + point2.x) / 2;// average x positions
        //yMid = (point1.y + point2.y) / 2;// average y positions
        //midpoint = sf::Vector2f(xMid, yMid);

        ////find midpoint in voxel space
        //for (sf::Vector2f voxelPos : m_voxelSubdivision)
        //{
        //    if (midpoint.x >= (voxelPos.x - (m_voxelSubdivisionSize / 2)) && midpoint.x < voxelPos.x + (m_voxelSubdivisionSize / 2)
        //        && midpoint.y >= voxelPos.y - (m_voxelSubdivisionSize / 2) && midpoint.y < voxelPos.y + (m_voxelSubdivisionSize / 2))
        //    {
        //        midpoint = voxelPos;
        //    }
        //}

        //if (midpoint == voxelPoint)
        //{
        //    m_edgePoints.push_back(voxelPoint);
        //}

    }


    //std::cout << "Number of feature points matched to voxel squares = " << m_voxelFeaturePoints.size() << std::endl;
    std::cout << "Number of verticies found = " << m_verticies.size() << std::endl;

}

float Voronoi::DistanceBetweenPoints(sf::Vector2f point1, sf::Vector2f point2)
{
    int xDist = abs(point2.x - point1.x);
    int yDist = abs(point2.y - point1.y);

    //return sqrt(pow(xDist, 2) + pow(yDist, 2));
    return pow(xDist, 2) + pow(yDist, 2);//as this is only ever used for comparrisons, we dont nee
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

    //DrawVoxels(window);

    DrawFeaturePoints(window);

    DrawVerticies(window);
}

void Voronoi::DrawFeaturePoints(sf::RenderWindow& window)
{
    //draw dots
    int radius = 4;
    for (FeaturePoint point : m_featurePoints)
    {
        sf::CircleShape dot(radius);
        dot.setOrigin(sf::Vector2f(radius, radius));//set origin to mid point
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

void Voronoi::DrawVerticies(sf::RenderWindow& window)
{
    //draw vertecies
    int radius = 4;
    for (sf::Vector2f vertex : m_verticies)
    {
        sf::CircleShape dot(radius);
        dot.setOrigin(sf::Vector2f(radius, radius));//set origin to mid point
        dot.setPosition(vertex);
        dot.setFillColor(sf::Color::Red);

        window.draw(dot);
    }

    //draw edges (test)
    //for (sf::Vector2f edge : m_edgePoints)
    //{
    //    sf::CircleShape dot(radius);
    //    dot.setOrigin(sf::Vector2f(radius, radius));//set origin to mid point
    //    dot.setPosition(edge);
    //    dot.setFillColor(sf::Color::Magenta);

    //    window.draw(dot);
    //}
}