#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <cmath>

struct FeaturePoint
{
	sf::Vector2f position;
};

struct Bisector
{
	sf::Vector2f position;
	sf::Vector2f direction;
	sf::Vector2f startPoint = sf::Vector2f(0, 0);
	sf::Vector2f endPoint = sf::Vector2f(0, 0);
};

struct ParentLine
{
	sf::Vector2f startPoint;
	sf::Vector2f endPoint;
	sf::Vector2f direction;
	float length;
	Bisector bisector;
};

class Voronoi
{
private:
	sf::RenderWindow* m_window;
	std::vector<FeaturePoint> m_featurePoints;

	std::vector<ParentLine> parentLines;
	std::vector<ParentLine> parentLineComparisions;
	std::vector<Bisector> finalBisectors;


	int m_totalFeaturePoints = 3;
	int m_borderWidth = 100;
	int m_cubeSize = 800;
	int m_sampleSize = 1;

	bool m_startedSim = false;

public:
	Voronoi();
	void Run();
	void Update();
	void Reset();

	void CreateBisectors();
	void RandomiseFeaturePointPosition(FeaturePoint& featurePoint);
	float DistanceBetweenPoints(sf::Vector2f point1, sf::Vector2f point2);

	void Draw(sf::RenderWindow& window);
	void DrawFeaturePoints(sf::RenderWindow& window);
	void DrawBisectors(sf::RenderWindow& window);
};

