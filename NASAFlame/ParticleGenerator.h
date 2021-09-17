#pragma once

#include <vector>
#include <glm/glm.hpp>

class ParticleGenerator
{
public:
	struct Particle
	{
		float x, y, z;
		float strength;
		float seed;
	};

	typedef std::vector<Particle> Particles;

	ParticleGenerator(const unsigned &amount, const float &range);

	void update(const float &deltaTime);

	const unsigned getAmount() const;
	const Particles& getParticles() const;
	const Particles& getSortedParticles(const glm::vec3 &cameraPos);

private:

	unsigned m_amount;
	Particles m_particles;
};