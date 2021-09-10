#pragma once
#include <SFML\Graphics.hpp>

class FPS
{
public:
	FPS();

	void update(const float &deltaTime);
	void draw(sf::RenderWindow &window);

private:
	float lastFPS = 0.f;

	sf::Text fpsText;
	sf::Font font;
};