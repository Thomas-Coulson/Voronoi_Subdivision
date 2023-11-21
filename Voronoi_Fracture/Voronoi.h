#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <stdlib.h>
#include <ctime>

struct FeaturePoint
{
	sf::Vector2f position;
};

class Voronoi
{
private:
	sf::RenderWindow* m_window;
	sf::Vector2f m_dotPos = sf::Vector2f(500, 500);//first dot position
	std::vector<FeaturePoint> m_featurePoints;

	int m_totalFeaturePoints = 5;
	int m_borderWidth = 100;
	int m_cubeSize = 800;

public:
	Voronoi();
	void Run();
	void Update();
	void RandomiseFeaturePointPosition(FeaturePoint& featurePoint);
	void Draw(sf::RenderWindow& window);
};

