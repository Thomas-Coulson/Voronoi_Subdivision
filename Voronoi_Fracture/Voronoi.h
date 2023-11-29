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

class Voronoi
{
private:
	sf::RenderWindow* m_window;
	//sf::Vector2f m_dotPos = sf::Vector2f(500, 500);//first dot position
	std::vector<FeaturePoint> m_featurePoints;

	int m_voxelSubdivisionSize = 10; //needs to be a factor of cube size
	std::vector<sf::Vector2f> m_voxelSubdivision;//stores midpoint positions of subdivisions
	std::vector<sf::Vector2f> m_voxelFeaturePoints;//stores midpoint positions of subdivisions
	std::vector<sf::Vector2f> m_verticies;
	std::vector<sf::Vector2f> m_edgePoints;

	int m_totalFeaturePoints = 5;
	int m_borderWidth = 100;
	int m_cubeSize = 800;
	int m_sampleSize = 1;

	bool m_startedSim = false;

public:
	Voronoi();
	void Run();
	void Update();
	void Reset();

	void FindVertecies();
	void RandomiseFeaturePointPosition(FeaturePoint& featurePoint);
	float DistanceBetweenPoints(sf::Vector2f point1, sf::Vector2f point2); //might need to return int

	void Draw(sf::RenderWindow& window);
	void DrawFeaturePoints(sf::RenderWindow& window);
	void DrawVoxels(sf::RenderWindow& window);
	void DrawVerticies(sf::RenderWindow& window);
};

