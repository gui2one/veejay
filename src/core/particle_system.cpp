#include "particle_system.h"


ParticleSystem::ParticleSystem(std::shared_ptr<Timer> timer)
{
	m_timer = timer;
	m_particles.clear();
	RectEmitter emitter; 
	m_emitters.push_back(emitter);
}

ParticleSystem::~ParticleSystem()
{
	
}

void ParticleSystem::update()
{
	//~ printf("num particles %d \n", m_particles.size());
	unsigned int num_p = getParticles().size();
	// delete too old
	for(size_t j = num_p-1; j>=0; j--)
	{
		std::cout<< "deleting particle " << j << std::endl;
		//~ if(m_particles[i]->age >= m_particles[i]->life)
		//~ {
			//~ printf("deleting particle %d \n", i);
			
			//~ m_particles.erase(m_particles.begin() + i , m_particles.begin() + i + 1);
		//~ }
	}	
	
	float delta_t = (float)m_timer->getDeltaMillis() / 1000;
	for(auto particle : m_particles){
		for( auto force : m_forces)
		{
			DirectionalForce * p_direct = nullptr;
			
			if( (p_direct = dynamic_cast<DirectionalForce *>(force.get())))
			{
				particle->velocity.x += p_direct->magnitude.x * delta_t;
				particle->velocity.y += p_direct->magnitude.y * delta_t;
			}
		}
		
		particle->age += delta_t;
		particle->position.x += particle->velocity.x;
		particle->position.y += particle->velocity.y;			
	}
	

}

void ParticleSystem::spawnParticles(unsigned int n)
{
	
	//~ m_particles.reserve(n);
	for(size_t i = 0; i < n; i++)
	{
		std::shared_ptr<Particle> p = std::make_shared<Particle>();
		m_particles.push_back(p);
	}
	printf("spawning %d particles\n", n);
	//~ std::cout<<"spawned " << n << " particles" << std::endl;
	
}

