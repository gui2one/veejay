#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include "../pch.h"


class Emitter{
public :
	Emitter(){
		
	}
	
	glm::vec3 position;
private :
};

class RectEmitter : public Emitter
{
public :
	RectEmitter() : 		
		Emitter(),
		size(glm::vec2(50.0f,50.0f))
	{
		
	}
	
	glm::vec2 size;
private :
};

class Particle{
public :
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
private :
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
