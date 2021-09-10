#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>

namespace
{
	unsigned SCR_WIDTH = 600;
}

Application::Application()
{
	srand(time(0));

	m_window.create(sf::VideoMode(SCR_WIDTH, SCR_WIDTH), "NASA Flame");

	if (!gladLoadGL()) exit(-1);

	glViewport(0, 0, SCR_WIDTH, SCR_WIDTH);
	glClearColor(0.f, 0.f, 0.f, 1.f);

	ImGui::SFML::Init(m_window);
}

void Application::run()
{
	sf::Clock clock;
	sf::Event evnt;

	while (m_window.isOpen())
	{
		const float deltaTime = clock.restart().asSeconds();

		ImGui::SFML::Update(m_window, sf::seconds(deltaTime));

		while (m_window.pollEvent(evnt))
		{
			ImGui::SFML::ProcessEvent(evnt);

			switch (evnt.type)
			{
			case sf::Event::Closed:
				m_window.close();
				break;
			}
		}

		draw();
	}

	glfwTerminate();
	ImGui::SFML::Shutdown();
}

void Application::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw shit

	m_window.pushGLStates();
	m_window.resetGLStates();

	//if (!m_input.hideGUI) m_fps.draw(m_window);

	ImGui::Begin("Options menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);

	static float dummy = 0.f;
	ImGui::PushItemWidth(50.f);
	ImGui::DragFloat("Try this on for size!", &dummy);
	ImGui::DragFloat("Try this on for size!", &dummy);
	ImGui::DragFloat("Try this on for size!", &dummy);

	ImGui::End();

	ImGui::SFML::Render();

	m_window.popGLStates();

	m_window.display();
}
