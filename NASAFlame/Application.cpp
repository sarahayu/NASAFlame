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
	const std::array<float, VERTEX_COUNT * 2> CIRCLE_VERTICES = []() {
		std::array<float, VERTEX_COUNT * 2> vertices;
		const float step = glm::two_pi<float>() / (VERTEX_COUNT - 1);
		for (unsigned i = 0; i < VERTEX_COUNT; i++)
		{
			vertices[i * 2] = std::cos(step * i);
			vertices[i * 2 + 1] = std::sin(step * i);
		}

		return vertices;
	}();
}

Application::Application()
	: m_particles(m_userOptions.particleAmt, m_userOptions.flameRadius)
{
	srand(time(0));

	m_window.create(sf::VideoMode(SCR_WIDTH, SCR_WIDTH), "NASA Flame");

	if (!gladLoadGL()) exit(-1);

	glViewport(0, 0, SCR_WIDTH, SCR_WIDTH);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	glClear(GL_COLOR_BUFFER_BIT);

	// draw shit
	auto viewRot = glm::rotate(glm::mat4(1.f), m_camera.dec, { 1.f, 0.f, 0.f })
		* glm::rotate(glm::mat4(1.f), m_camera.asc, { 0.f, 1.f, 0.f });

	auto projView =
		glm::perspective(glm::radians(45.0f), (float)m_window.getSize().x / m_window.getSize().y, 0.1f, 200.f)
		* glm::translate(glm::mat4(1.f), { 0.f, 0.f, -m_camera.rad })
		* viewRot;
	glUseProgram(m_particleShader.ID);
	glUniformMatrix4fv(m_particleShader.projView, 1, GL_FALSE, glm::value_ptr(projView));
	glUniformMatrix4fv(m_particleShader.viewRot, 1, GL_FALSE, glm::value_ptr(viewRot));

	glBindVertexArray(m_particleRenderable.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_particleRenderable.instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleGenerator::Particle) * m_particles.getAmount(), m_particles.getParticles().data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleGenerator::Particle), (void*)0);
	glVertexAttribDivisor(1, 1);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_particles.getAmount());

	projView =
		glm::perspective(glm::radians(45.0f), (float)m_window.getSize().x / m_window.getSize().y, 0.1f, 200.f)
		* glm::translate(glm::mat4(1.f), { 0.f, 0.f, -m_camera.rad })
		* glm::scale(glm::mat4(1.f), glm::vec3(m_userOptions.flameRadius + 3.f))
		* viewRot;

	glUseProgram(m_firebloomShader.ID);
	glUniformMatrix4fv(m_firebloomShader.PVM, 1, GL_FALSE, glm::value_ptr(projView));
	glUniformMatrix4fv(m_firebloomShader.viewRot, 1, GL_FALSE, glm::value_ptr(viewRot));

	glBindVertexArray(m_bloomRenderable.VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_VERTICES.size());

	glBindVertexArray(0);

	m_window.pushGLStates();
	m_window.resetGLStates();

	//if (!m_input.hideGUI) m_fps.draw(m_window);

	ImGui::Begin("Options menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::PushItemWidth(100.f);
	ImGui::DragInt("Particle amount", (int*)(&m_userOptions.particleAmt), 2.f, 1, 50000);
	ImGui::DragFloat("Flame radius", &m_userOptions.flameRadius, 1.f, 1.f, 100.f);
	if (ImGui::Button("Update"))
		m_particles = ParticleGenerator(m_userOptions.particleAmt, m_userOptions.flameRadius);

	ImGui::End();

	ImGui::SFML::Render();

	m_window.popGLStates();

	m_window.display();
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

	glBindBuffer(GL_ARRAY_BUFFER, m_particleRenderable.instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleGenerator::Particle) * m_particles.getAmount(), m_particles.getParticles().data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleGenerator::Particle), (void*)0);
	glVertexAttribDivisor(1, 1);

	glBindVertexArray(m_bloomRenderable.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_bloomRenderable.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * CIRCLE_VERTICES.size(), CIRCLE_VERTICES.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	m_particleShader.ID = ShaderUtil::loadShader("particle-shader");
	glUseProgram(m_particleShader.ID);
	m_particleShader.projView = glGetUniformLocation(m_particleShader.ID, "projView");
	m_particleShader.viewRot = glGetUniformLocation(m_particleShader.ID, "viewRot");

	m_firebloomShader.ID = ShaderUtil::loadShader("firebloom-shader");
	glUseProgram(m_firebloomShader.ID);
	m_firebloomShader.PVM = glGetUniformLocation(m_firebloomShader.ID, "PVM");
	m_firebloomShader.viewRot = glGetUniformLocation(m_firebloomShader.ID, "viewRot");

}
