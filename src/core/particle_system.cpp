#include "particle_system.h"


ParticleSystem::ParticleSystem()
{
	m_particles.clear();
}

ParticleSystem::~ParticleSystem()
{
	
}

void ParticleSystem::update()
{
	for(auto particle : getParticles()){
		particle->velocity.y -=0.01;
		particle->position.y += particle->velocity.y;
	}
}

void ParticleSystem::spawnParticles(unsigned int n)
{
	
	m_particles.reserve(n);
	for(size_t i = 0; i < n; i++)
	{
		std::shared_ptr<Particle> p = std::make_shared<Particle>();
		m_particles.emplace_back(p);
	}
	printf("spawning %d particles\n", n);
	//~ std::cout<<"spawned " << n << " particles" << std::endl;
	
}

