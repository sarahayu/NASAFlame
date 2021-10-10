#include "ParticleGenerator.h"

#include <cmath>
#include <iostream>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\norm.hpp>

ParticleGenerator::ParticleGenerator(const unsigned & amount, const float & range)
	: m_particles(amount), m_amount(amount)
{
	// https://math.stackexchange.com/questions/87230/picking-random-points-in-the-volume-of-sphere-with-uniform-probability

	const int precision = (int)range * 2;
	for (int i = 0; i < amount; i++)
	{
		const float U = (float)(std::rand() % precision + 1) / precision * 0.1f + 0.9f;
		const float X1 = (float)(std::rand() % (precision * 2 + 1) - precision) / precision,
			X2 = (float)(std::rand() % (precision * 2 + 1) - precision) * 0.35f / precision,
			X3 = (float)(std::rand() % (precision * 2 + 1) - precision) / precision;

		const float len = std::sqrt(X1 * X1 + X2 * X2 + X3 * X3);
		float randOffset = (float)(std::rand() % 20) / 10.f - 1.f;
		float randOffsetSq = std::pow(randOffset, 5);
		if (randOffset < 0.f) randOffsetSq = -randOffsetSq;
		const float randdist = range * std::cbrt(U) + randOffsetSq * 4.f;

		m_particles[i] = {
			randdist * X1 / len,						// x
			randdist * X2 * 0.9f / len,					// y
			randdist * X3 / len,						// z
			(float)(std::rand() % 1000) / 1000,			// strength
			(float)(std::rand() % 1000) / 1000			// seed
		};
	}
}

void ParticleGenerator::update(const float & deltaTime)
{
}

const unsigned ParticleGenerator::getAmount() const
{
	return m_amount;
}

const ParticleGenerator::Particles & ParticleGenerator::getParticles() const
{
	return m_particles;
}

const ParticleGenerator::Particles & ParticleGenerator::getSortedParticles(const glm::vec3 & cameraPos)
{
	std::sort(m_particles.begin(), m_particles.end(), [&cameraPos](const Particle &first, const Particle &second) {
		return glm::distance2(cameraPos, { first.x, first.y, first.z }) < glm::distance2(cameraPos, { second.x, second.y, second.z });
	});

	return m_particles;
}