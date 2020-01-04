#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include "../pch.h"
#include "timer.h"

enum PARTICLE_EMITTER_TYPE
{
	PARTICLE_EMITTER_POINT,
	PARTICLE_EMITTER_RECT
};

class Force
{
public:
	Force(){};
	virtual void funcA() = 0;
	
private:
};

class DirectionalForce : public Force
{
public:
	DirectionalForce() : Force()
	{
	
		
	}
	void funcA(){}
	
	glm::vec2 magnitude;
private:

};

class Emitter{
public :
	Emitter(){
		
	}
	virtual ~Emitter(){};
	virtual void funcA() = 0;
	
	glm::vec3 position;
	int amount = 10;
	long int internal_counter = 0;
private :
};

class PointEmitter : public Emitter
{
public :
	PointEmitter() : Emitter()
	{
		
	}
	~PointEmitter(){};
	
	void funcA(){};
	
	
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
	~RectEmitter(){};
	
	void funcA(){};
	
	glm::vec2 size;
private :
};

class Particle{
public :
	Particle() : 
		position(glm::vec3(0.0,0.0,0.0)), 
		velocity(glm::vec3(0.0,0.0,0.0)), 
		acceleration(glm::vec3(0.0,0.0,0.0)),
		age(0.0f),
		life(1.0f)
	{
		
	}
	
	unsigned int id;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	float age, life;
private :
};

class ParticleSystem
{
	public:
		ParticleSystem(std::shared_ptr<Timer> timer);
		virtual ~ParticleSystem();
		
		void spawnParticles(unsigned int n);
		void emitParticles(float amout_mult);
		void update();
		
		inline std::vector<std::shared_ptr<Particle> >& getParticles(){
			return m_particles;
		}
		
		inline std::vector< std::shared_ptr<Force> >& getForces(){
			return m_forces;
		}
		
		inline void addForce(std::shared_ptr<Force> force)
		{
			m_forces.push_back(force);
		}
		
		void addEmitter(PARTICLE_EMITTER_TYPE type);
		std::vector<std::shared_ptr<Emitter> >& getEmitters()
		{
			return m_emitters;
		}
			
		
		
	private:
		/* add your private declarations */
		std::vector<std::shared_ptr<Particle> > m_particles;
		std::vector<std::shared_ptr<Emitter> > m_emitters;
		std::shared_ptr<Timer> m_timer;
		
		std::vector< std::shared_ptr<Force> > m_forces;
		
		std::mt19937 rng;
		
};

#endif /* PARTICLE_SYSTEM_H */ 
