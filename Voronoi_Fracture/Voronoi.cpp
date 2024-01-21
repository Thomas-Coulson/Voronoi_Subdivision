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
                    CreateBisectors();
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
    parentLines.clear();
    parentLineComparisions.clear();
    finalBisectors.clear();
}

void Voronoi::CreateBisectors()
{
    //finds parent lines
    for (int i = 0; i < m_featurePoints.size(); i++)
    {

        parentLines.clear();
        parentLineComparisions.clear();

        for (int j = 0; j < m_featurePoints.size(); j++)
        {
            if (i != j)//not the same point
            {
                bool sameLine = false;
                ParentLine newParentLine;
                newParentLine.startPoint = m_featurePoints[i].position;
                newParentLine.endPoint = m_featurePoints[j].position;
                newParentLine.length = DistanceBetweenPoints(newParentLine.startPoint, newParentLine.endPoint);
                newParentLine.direction = (newParentLine.endPoint - newParentLine.startPoint) / newParentLine.length;
                parentLines.push_back(newParentLine);
            }
        }

        //find bisectors
        for (int i = 0; i < parentLines.size(); i++)
        {
            parentLines[i].bisector.position = parentLines[i].startPoint + (parentLines[i].direction * (parentLines[i].length / 2));
            parentLines[i].bisector.direction.x = parentLines[i].direction.y * -1;
            parentLines[i].bisector.direction.y = parentLines[i].direction.x;
        }

        //copy parent lines into parentLineComparisions
        for (int p = 0; p < parentLines.size(); p++)
        {
            ParentLine parentLine = parentLines[p];
            parentLineComparisions.push_back(parentLine);
        }

        //add wall lines to parentLineComparisions
        ParentLine topWall;
        topWall.bisector.position = sf::Vector2f(100, 100);//top left
        topWall.bisector.direction = sf::Vector2f(100, 0);// topLeft->topRight vector
        parentLineComparisions.push_back(topWall);

        ParentLine rightWall;
        rightWall.bisector.position = sf::Vector2f(900, 100);//top right
        rightWall.bisector.direction = sf::Vector2f(0, 100);// topRight->bottomRight vector
        parentLineComparisions.push_back(rightWall);

        ParentLine bottomWall;
        bottomWall.bisector.position = sf::Vector2f(900, 900);//bottom left
        bottomWall.bisector.direction = sf::Vector2f(-100, 0);// bottomRight->bottomLeft vector
        parentLineComparisions.push_back(bottomWall);

        ParentLine leftWall;
        leftWall.bisector.position = sf::Vector2f(100, 900);//bottom left
        leftWall.bisector.direction = sf::Vector2f(0, -100);// bottomLeft->topLeft vector
        parentLineComparisions.push_back(leftWall);


        for (int i = 0; i < parentLines.size(); i++)
        {
            int numIntersections = 0;

            sf::Vector2f clostestIntersection1;
            float closestDistance1 = 0;
            sf::Vector2f clostestIntersection2;
            float closestDistance2 = 0;

            float featurePointDist1 = 0;
            float featurePointDist2 = 0;

            for (int j = 0; j < parentLineComparisions.size(); j++)
            {
                if (i != j)//not the same point
                {
                    //check bisectors for intersections.
                    int x1 = parentLines[i].bisector.position.x;
                    int y1 = parentLines[i].bisector.position.y;
                    float mx1 = parentLines[i].bisector.direction.x;
                    float my1 = parentLines[i].bisector.direction.y;

                    int x2 = parentLineComparisions[j].bisector.position.x;
                    int y2 = parentLineComparisions[j].bisector.position.y;
                    float mx2 = parentLineComparisions[j].bisector.direction.x;
                    float my2 = parentLineComparisions[j].bisector.direction.y;

                    double top = (my2 * (x1 - x2)) - (mx2 * (y1 - y2));
                    double bottom = (mx2 * my1) - (my2 * mx1);

                    if (bottom != 0)//only continue if there is an intersection (lines are not parallel)
                    {
                        double t = top / bottom;

                        sf::Vector2f intersectionPos = sf::Vector2f(x1 + (mx1 * t), y1 + (my1 * t));

                        float intersectionDist = DistanceBetweenPoints(parentLines[i].bisector.position, intersectionPos);

                        //start comparing intersections to find closest to parent line

                        if (j >= parentLineComparisions.size() - 4)
                        {
                            //looking at wall connections

                            featurePointDist1 = DistanceBetweenPoints(parentLines[i].startPoint, intersectionPos);
                            featurePointDist2 = DistanceBetweenPoints(parentLines[i].endPoint, intersectionPos);

                            bool badIntersection = false;

                            for (int fp = 0; fp < m_featurePoints.size(); fp++)
                            {
                                float distance = DistanceBetweenPoints(intersectionPos, m_featurePoints[fp].position);

                                if (distance < featurePointDist1 && distance < featurePointDist2)
                                {
                                    //bad intersection, another wall is preffered.
                                    badIntersection = true;
                                }

                            }

                            if (!badIntersection)
                            {
                                if (numIntersections == 0)
                                {
                                    closestDistance1 = intersectionDist;
                                    clostestIntersection1 = intersectionPos;
                                    numIntersections++;
                                }
                                else if (numIntersections == 1)
                                {
                                    if (intersectionDist < closestDistance1)
                                    {
                                        closestDistance2 = closestDistance1;
                                        clostestIntersection2 = clostestIntersection1;
                                        closestDistance1 = intersectionDist;
                                        clostestIntersection1 = intersectionPos;
                                    }
                                    else
                                    {
                                        closestDistance2 = intersectionDist;
                                        clostestIntersection2 = intersectionPos;
                                    }
                                    numIntersections++;
                                }
                                else
                                {
                                    if (intersectionDist < closestDistance2)
                                    {
                                        if (intersectionDist < closestDistance1)
                                        {
                                            closestDistance2 = closestDistance1;
                                            clostestIntersection2 = clostestIntersection1;
                                            closestDistance1 = intersectionDist;
                                            clostestIntersection1 = intersectionPos;
                                        }
                                        else
                                        {
                                            closestDistance2 = intersectionDist;
                                            clostestIntersection2 = intersectionPos;
                                        }
                                    }
                                    numIntersections++;
                                }
                            }

                        }
                        else if (numIntersections == 0)
                        {
                            closestDistance1 = intersectionDist;
                            clostestIntersection1 = intersectionPos;
                            numIntersections++;
                        }
                        else if (numIntersections == 1)
                        {
                            if (intersectionDist < closestDistance1)
                            {
                                closestDistance2 = closestDistance1;
                                clostestIntersection2 = clostestIntersection1;
                                closestDistance1 = intersectionDist;
                                clostestIntersection1 = intersectionPos;
                            }
                            else
                            {
                                closestDistance2 = intersectionDist;
                                clostestIntersection2 = intersectionPos;
                            }
                            numIntersections++;
                        }
                        else
                        {
                            if (intersectionDist < closestDistance2)
                            {
                                if (intersectionDist < closestDistance1)
                                {
                                    closestDistance2 = closestDistance1;
                                    clostestIntersection2 = clostestIntersection1;
                                    closestDistance1 = intersectionDist;
                                    clostestIntersection1 = intersectionPos;
                                }
                                else
                                {
                                    closestDistance2 = intersectionDist;
                                    clostestIntersection2 = intersectionPos;
                                }
                            }
                            numIntersections++;
                        }
                    }
                }
            }

            // set start point and end point to closest two intersections
            parentLines[i].bisector.startPoint = clostestIntersection1;
            parentLines[i].bisector.endPoint = clostestIntersection2;

            finalBisectors.push_back(parentLines[i].bisector);

        }
    }
}

float Voronoi::DistanceBetweenPoints(sf::Vector2f point1, sf::Vector2f point2)
{
    int xDist = abs(point2.x - point1.x);
    int yDist = abs(point2.y - point1.y);

    return sqrt(pow(xDist, 2) + pow(yDist, 2));
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

    DrawFeaturePoints(window);

    DrawBisectors(window);
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

void Voronoi::DrawBisectors(sf::RenderWindow& window)
{
    for (int i = 0; i < finalBisectors.size(); i++)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(finalBisectors[i].startPoint),
            sf::Vertex(finalBisectors[i].endPoint),
        };

        window.draw(line, 2, sf::Lines);
    }

}