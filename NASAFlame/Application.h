#pragma once

#include <SFML/Graphics.hpp>
#include <imgui/imgui.h>

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
	bool isMouseInMenu(const float &x, const float &y) const;

	struct {
		unsigned VAO, VBO, instanceVBO;
	} m_particleRenderable;

	struct {
		unsigned VAO, VBO, EBO;
	} m_sphereRenderable;

	struct {
		unsigned ID;
		unsigned projView, viewRot, minSoot, fogOffset, fogFactor;
	} m_particleShader;

	struct {
		unsigned ID;
		unsigned projView, model, blueBloom, cameraPos;
	} m_firebloomShader;

	unsigned m_screenShader;

	struct {
		unsigned ID;
		unsigned horizontal, intensity;
	} m_blurShader;

	struct {
		bool showMenu = true;
		int particleAmt = 2750;
		float flameRadius = 25.f;
		int sootRatio = 5;
		int blurIntensity = 8;
		float fogOffset = 0.f;
		float fogFactor = 1.f;

		ImVec4 menuBB;
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
		float asc = 0.f, dec = 0.f, rad = 150.f;
	} m_camera;

	sf::RenderWindow m_window;

	ParticleGenerator m_particles;
};