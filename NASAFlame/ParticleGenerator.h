#pragma once

#include <vector>

class ParticleGenerator
{
public:
	struct Particle
	{
		float x, y, z;
		bool fire;
	};

	typedef std::vector<Particle> Particles;

	ParticleGenerator(const unsigned &amount, const float &range);

	void update(const float &deltaTime);

	const unsigned getAmount() const;
	const Particles& getParticles() const;

private:

	unsigned m_amount;
	Particles m_particles;
};