#pragma once

#include <SFML/Graphics.hpp>

#include "ParticleGenerator.h"

class Application
{
public:
	Application();

	void run();

private:
	void input(const float &deltaTime);
	void update(const float &deltaTime);
	void render();

	void loadResources();

	struct {
		unsigned VAO, VBO, instanceVBO;
	} m_particleRenderable;

	struct {
		unsigned VAO, VBO;
	} m_bloomRenderable;

	struct {
		unsigned ID;
		unsigned projView, viewRot;
	} m_particleShader;

	struct {
		unsigned ID;
		unsigned PVM, viewRot;
	} m_firebloomShader;

	struct {
		unsigned particleAmt = 1300;
		float flameRadius = 18.f;
	} m_userOptions;

	struct {
		float asc = 0.f, dec = 0.f, rad = 50.f;
	} m_camera;

	sf::RenderWindow m_window;

	ParticleGenerator m_particles;
};