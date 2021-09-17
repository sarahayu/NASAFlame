#pragma once

#include <array>
#include <glm/gtc/matrix_transform.hpp>

namespace MathUtil
{
	template<size_t S>
	std::array<float, S> generateCircle()
	{
		std::array<float, S> vertices;
		const float step = glm::two_pi<float>() / (S / 2 - 1);
		for (unsigned i = 0; i < S / 2; i++)
		{
			vertices[i * 2] = std::cos(step * i);
			vertices[i * 2 + 1] = std::sin(step * i);
		}

		return vertices;
	}

	glm::vec3 getCameraPos(const glm::mat4 &viewModel);
}