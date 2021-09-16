#include "ParticleGenerator.h"

#include <cmath>
#include <iostream>

ParticleGenerator::ParticleGenerator(const unsigned & amount, const float & range)
	: m_particles(amount), m_amount(amount)
{
	// https://math.stackexchange.com/questions/87230/picking-random-points-in-the-volume-of-sphere-with-uniform-probability

	const int precision = (int)range * 2;
	for (int i = 0; i < amount; i++)
	{
		const float U = (float)(std::rand() % precision + 1) / precision * 0.1f + 0.9f;
		const float X1 = (float)(std::rand() % (precision * 2 + 1) - precision) / precision,
			X2 = (float)(std::rand() % (precision * 2 + 1) - precision) / precision * 0.35f,
			X3 = (float)(std::rand() % (precision * 2 + 1) - precision) / precision;

		const float len = std::sqrt(X1 * X1 + X2 * X2 + X3 * X3);
		const float randdist = range * std::cbrt(U) + (float)(std::rand() % 20) / 10.f - 1.f;

		m_particles[i] = {
			randdist * X1 / len,
			randdist * X2 / len,
			randdist * X3 / len,
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
