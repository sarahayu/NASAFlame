#pragma once

#include <SFML/Graphics.hpp>

class Application
{
public:
	Application();

	void run();

private:
	void draw();

	sf::RenderWindow m_window;
};