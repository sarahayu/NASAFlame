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

	void renderObjects();
	void renderMenu();
	void loadResources();
	void reloadParticleRenderable(const glm::vec3 &cameraPos = glm::vec3());

	struct {
		unsigned VAO, VBO, instanceVBO;
	} m_particleRenderable;

	struct {
		unsigned VAO, VBO;
	} m_bloomRenderable;

	struct {
		unsigned ID;
		unsigned projView, viewRot, minSoot;
	} m_particleShader;

	struct {
		unsigned ID;
		unsigned PVM, viewRot;
	} m_firebloomShader;

	unsigned m_screenShader;

	struct {
		unsigned ID;
		unsigned horizontal, intensity;
	} m_blurShader;

	struct {
		bool showMenu = true;
		int particleAmt = 3000;
		float flameRadius = 26.f;
		int sootRatio = 6;
		int blurIntensity = 4;
	} m_userOptions;

	struct {
		unsigned FBO;
		unsigned colorTexture, depthRBO;
	} m_screenBuffer;

	struct {
		unsigned FBO[2];
		unsigned colorTextures[2];
	} m_blurBuffers;

	struct {
		float asc = 0.f, dec = 0.f, rad = 70.f;
	} m_camera;

	sf::RenderWindow m_window;

	ParticleGenerator m_particles;
};