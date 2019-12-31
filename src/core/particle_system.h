#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include "../pch.h"


struct Emitter{
	
	Emitter(){
		
	}
	
	glm::vec3 position;
};

struct RectEmitter : public Emitter
{
	RectEmitter() : Emitter(){
		
	}
	
	
};

struct Particle{
	
	Particle() : 
		position(glm::vec3(0.0,0.0,0.0)), 
		velocity(glm::vec3(0.0,0.0,0.0)), 
		acceleration(glm::vec3(0.0,0.0,0.0))
	{
		
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
		std::vector<Emitter > m_emitters;
		
};

#endif /* PARTICLE_SYSTEM_H */ 
