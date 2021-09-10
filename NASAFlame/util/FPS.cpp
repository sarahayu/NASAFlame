#include "FPS.h"
#include <iostream>

FPS::FPS()
{
	font.loadFromFile("resources/proggy-clean.ttf");
	fpsText.setFont(font);
}

void FPS::update(const float & deltaTime)
{
	static float elapsed = 0.f;
	static int frames;

	elapsed += deltaTime;
	frames++;

	if (elapsed >= 0.5f)
	{
		lastFPS = frames / elapsed;
		//std::cout << "\n" << lastFPS;

		elapsed -= 0.5f;
		frames = 0;
	}
}

void FPS::draw(sf::RenderWindow & window)
{
	fpsText.setPosition(5.f, 5.f);
	fpsText.setString("FPS: " + std::to_string(lastFPS));
	window.draw(fpsText);
}