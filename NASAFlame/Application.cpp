#include "Application.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>
#include <iostream>
#include <array>

#include "util/ShaderUtil.h"
#include "util/MathUtil.h"

namespace
{
	const unsigned SCR_WIDTH = 600;
	const float FRAMELEN = 1.f / 60;
	const float PAN_SPEED = 60.f;
	const float ZOOM_SPEED = 50.f;
	const float MOUSE_SENSITIVITY = 0.01f;

	const float QUAD_VERTICES[] = {
		-0.5f, -0.5f,
		0.5f, -0.5f,
		-0.5f, 0.5f,
		0.5f, 0.5f,
	};
	const unsigned VERTEX_COUNT = 64;
	const auto CIRCLE_VERTICES = MathUtil::generateCircle<VERTEX_COUNT * 2>();
}

Application::Application()
	: m_particles(m_userOptions.particleAmt, m_userOptions.flameRadius)
{
	srand(time(0));

	sf::ContextSettings settings;
	settings.depthBits = 24;
	m_window.create(sf::VideoMode(SCR_WIDTH, SCR_WIDTH), "NASA Flame", sf::Style::Default, settings);

	if (!gladLoadGL()) exit(-1);

	glViewport(0, 0, SCR_WIDTH, SCR_WIDTH);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glEnable(GL_BLEND);

	ImGui::SFML::Init(m_window);

	loadResources();
}

void Application::run()
{
	sf::Clock clock;
	float accumulated = 0.f;

	while (m_window.isOpen())
	{
		const float deltaTime = clock.restart().asSeconds();
		accumulated += deltaTime;

		input(deltaTime);

		while (accumulated >= FRAMELEN)
		{
			accumulated -= FRAMELEN;
			update(FRAMELEN);
		}

		render();
	}

	glfwTerminate();
	ImGui::SFML::Shutdown();
}

void Application::input(const float & deltaTime)
{
	static int lastX = MAXINT, lastY = MAXINT;

	ImGui::SFML::Update(m_window, sf::seconds(deltaTime));

	sf::Event evnt;

	while (m_window.pollEvent(evnt))
	{
		ImGui::SFML::ProcessEvent(evnt);

		switch (evnt.type)
		{
		case sf::Event::Closed:
			m_window.close();
			break;
		case sf::Event::Resized:
			glViewport(0, 0, evnt.size.width, evnt.size.height);
			break;
		case sf::Event::KeyPressed:
			switch (evnt.key.code)
			{
			case sf::Keyboard::Left:
				m_camera.asc += PAN_SPEED * deltaTime;
				break;
			case sf::Keyboard::Right:
				m_camera.asc -= PAN_SPEED * deltaTime;
				break;
			case sf::Keyboard::Down:
				m_camera.dec -= PAN_SPEED * deltaTime;
				break;
			case sf::Keyboard::Up:
				m_camera.dec += PAN_SPEED * deltaTime;
				break;
			case sf::Keyboard::LBracket:
				m_camera.rad += ZOOM_SPEED * deltaTime;
				break;
			case sf::Keyboard::RBracket:
				m_camera.rad -= ZOOM_SPEED * deltaTime;
				break;
			case sf::Keyboard::F1:
				m_userOptions.showMenu = !m_userOptions.showMenu;
				break;
			}
			m_camera.dec = std::min(std::max(m_camera.dec, -glm::radians(90.f)), glm::radians(90.f));
			break;
		case sf::Event::MouseButtonPressed:
			if (evnt.mouseButton.button == sf::Mouse::Left)
			{
				lastX = evnt.mouseButton.x;
				lastY = evnt.mouseButton.y;
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (evnt.mouseButton.button == sf::Mouse::Left)
			{
				lastX = lastY = MAXINT;
			}
			break;
		case sf::Event::MouseMoved:
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && lastX != MAXINT)
			{
				const int deltaX = lastX - evnt.mouseMove.x, deltaY = lastY - evnt.mouseMove.y;
				if (std::abs(deltaX) > std::abs(deltaY))
				{
					m_camera.asc -= PAN_SPEED * deltaTime * deltaX * MOUSE_SENSITIVITY;
				}
				else
				{
					m_camera.dec -= PAN_SPEED * deltaTime * deltaY * MOUSE_SENSITIVITY;
				}
				lastX = evnt.mouseMove.x;
				lastY = evnt.mouseMove.y;
				m_camera.dec = std::min(std::max(m_camera.dec, -glm::radians(90.f)), glm::radians(90.f));
			}
			break;
		case sf::Event::MouseWheelScrolled:
			m_camera.rad += ZOOM_SPEED * deltaTime * evnt.mouseWheelScroll.delta;
			break;
		}
	}
}

void Application::update(const float & deltaTime)
{
}

void Application::render()
{
	renderObjects();

	m_window.pushGLStates();
	m_window.resetGLStates();

	renderMenu();

	m_window.popGLStates();

	m_window.display();
}

void Application::renderObjects()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_screenBuffer.FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// draw shit
	const auto viewRot = glm::rotate(glm::mat4(1.f), m_camera.dec, { 1.f, 0.f, 0.f })
		* glm::rotate(glm::mat4(1.f), m_camera.asc, { 0.f, 1.f, 0.f });

	const auto translation = glm::translate(glm::mat4(1.f), { 0.f, 0.f, -m_camera.rad });

	auto projView =
		glm::perspective(glm::radians(45.0f), (float)m_window.getSize().x / m_window.getSize().y, 0.1f, 200.f)
		* translation
		* viewRot;

	glUseProgram(m_particleShader.ID);
	glUniformMatrix4fv(m_particleShader.projView, 1, GL_FALSE, glm::value_ptr(projView));
	glUniformMatrix4fv(m_particleShader.viewRot, 1, GL_FALSE, glm::value_ptr(viewRot));
	glUniform1f(m_particleShader.minSoot, 1.f / m_userOptions.sootRatio);

	const auto cameraPos = MathUtil::getCameraPos(-translation * viewRot);

	glBindVertexArray(m_particleRenderable.VAO);
	reloadParticleRenderable(cameraPos);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_particles.getAmount());

	projView =
		glm::perspective(glm::radians(45.0f), (float)m_window.getSize().x / m_window.getSize().y, 0.1f, 200.f)
		* glm::translate(glm::mat4(1.f), { 0.f, 0.f, -m_camera.rad })
		* glm::scale(glm::mat4(1.f), glm::vec3(m_userOptions.flameRadius + 3.f))
		* viewRot;

	glDisable(GL_DEPTH_TEST);
	glUseProgram(m_firebloomShader.ID);
	glUniformMatrix4fv(m_firebloomShader.PVM, 1, GL_FALSE, glm::value_ptr(projView));
	glUniformMatrix4fv(m_firebloomShader.viewRot, 1, GL_FALSE, glm::value_ptr(viewRot));

	glBindVertexArray(m_bloomRenderable.VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_VERTICES.size());

	bool horizontal = true, first_iteration = true;
	glUseProgram(m_blurShader.ID);
	for (unsigned int i = 0; i < m_userOptions.blurIntensity * 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_blurBuffers.FBO[horizontal]);
		glUniform1i(m_blurShader.horizontal, horizontal);
		glBindTexture(
			GL_TEXTURE_2D, first_iteration ? m_screenBuffer.colorTexture : m_blurBuffers.colorTextures[!horizontal]
		);
		glBindVertexArray(m_particleRenderable.VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(m_screenShader);
	glBindVertexArray(m_particleRenderable.VAO);
	glBindTexture(GL_TEXTURE_2D, m_screenBuffer.colorTexture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
	glBindTexture(GL_TEXTURE_2D, m_blurBuffers.colorTextures[!horizontal]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

void Application::renderMenu()
{
	if (m_userOptions.showMenu)
	{
		ImGui::Begin("Options menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::PushItemWidth(100.f);
		ImGui::DragInt("Particle amount", &m_userOptions.particleAmt, 2.f, 1, 50000);
		ImGui::DragFloat("Flame radius", &m_userOptions.flameRadius, 1.f, 1.f, 100.f);
		ImGui::DragInt("Soot to spark ratio", &m_userOptions.sootRatio, 1.f, 1, 50, "1:%d");
		ImGui::DragInt("Blur intensity", &m_userOptions.blurIntensity, 1.f, 1, 8);
		if (ImGui::Button("Update"))
			m_particles = ParticleGenerator(m_userOptions.particleAmt, m_userOptions.flameRadius);

		ImGui::End();
	}

	ImGui::SFML::Render();
}

void Application::loadResources()
{
	glGenVertexArrays(1, &m_particleRenderable.VAO);
	glGenVertexArrays(1, &m_bloomRenderable.VAO);
	glGenBuffers(1, &m_particleRenderable.VBO);
	glGenBuffers(1, &m_particleRenderable.instanceVBO);
	glGenBuffers(1, &m_bloomRenderable.VBO);

	glBindVertexArray(m_particleRenderable.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_particleRenderable.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTICES), QUAD_VERTICES, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	reloadParticleRenderable();

	glBindVertexArray(m_bloomRenderable.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_bloomRenderable.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * CIRCLE_VERTICES.size(), CIRCLE_VERTICES.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glGenFramebuffers(1, &m_screenBuffer.FBO);
	glGenTextures(1, &m_screenBuffer.colorTexture);
	glGenRenderbuffers(1, &m_screenBuffer.depthRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_screenBuffer.FBO);
	glBindTexture(GL_TEXTURE_2D, m_screenBuffer.colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_WIDTH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_screenBuffer.colorTexture, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, m_screenBuffer.depthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_WIDTH);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_screenBuffer.depthRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glGenFramebuffers(2, m_blurBuffers.FBO);
	glGenTextures(2, m_blurBuffers.colorTextures);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_blurBuffers.FBO[i]);
		glBindTexture(GL_TEXTURE_2D, m_blurBuffers.colorTextures[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_WIDTH, 0, GL_RGBA, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurBuffers.colorTextures[i], 0
		);
	}

	m_particleShader.ID = ShaderUtil::loadShader("particle-shader");
	glUseProgram(m_particleShader.ID);
	m_particleShader.projView = glGetUniformLocation(m_particleShader.ID, "projView");
	m_particleShader.viewRot = glGetUniformLocation(m_particleShader.ID, "viewRot");
	m_particleShader.minSoot = glGetUniformLocation(m_particleShader.ID, "minSoot");

	m_firebloomShader.ID = ShaderUtil::loadShader("firebloom-shader");
	glUseProgram(m_firebloomShader.ID);
	m_firebloomShader.PVM = glGetUniformLocation(m_firebloomShader.ID, "PVM");
	m_firebloomShader.viewRot = glGetUniformLocation(m_firebloomShader.ID, "viewRot");

	m_screenShader = ShaderUtil::loadShader("screen-shader");
	glUseProgram(m_screenShader);
	glUniform1i(glGetUniformLocation(m_screenShader, "screenTexture"), 0);

	m_blurShader.ID = ShaderUtil::loadShader("blur-shader");
	glUseProgram(m_blurShader.ID);
	glUniform1i(glGetUniformLocation(m_blurShader.ID, "screenTexture"), 0);
	m_blurShader.horizontal = glGetUniformLocation(m_blurShader.ID, "horizontal");
	m_blurShader.intensity = glGetUniformLocation(m_blurShader.ID, "intensity");
}

void Application::reloadParticleRenderable(const glm::vec3 & cameraPos)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_particleRenderable.instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleGenerator::Particle) * m_particles.getAmount(), m_particles.getSortedParticles(cameraPos).data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleGenerator::Particle), (void*)0);
	glVertexAttribDivisor(1, 1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGenerator::Particle), (void*)(offsetof(ParticleGenerator::Particle, strength)));
	glVertexAttribDivisor(2, 1);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGenerator::Particle), (void*)(offsetof(ParticleGenerator::Particle, seed)));
	glVertexAttribDivisor(3, 1);
}
