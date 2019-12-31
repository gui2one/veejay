#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include "../pch.h"


struct Emitter{
	
	Emitter(){
		
	}
	
	glm::vec3 position;
};

struct Particle{
	
	Particle(){
		
	}
	
	unsigned int id;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
};

class ParticleSystem
{
	public:
		ParticleSystem();
		virtual ~ParticleSystem();
		
		void spawnParticles(unsigned int n);
		void update();
		
		inline std::vector<std::shared_ptr<Particle> > getParticles(){
			return m_particles;
		}
	private:
		/* add your private declarations */
		std::vector<std::shared_ptr<Particle> > m_particles;
		std::vector<std::shared_ptr<Emitter> > m_emitters;
		
};

#endif /* PARTICLE_SYSTEM_H */ 
