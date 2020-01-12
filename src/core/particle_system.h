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
	Force(const Force& other){};
	
	virtual void funcA() = 0;
	
private:
};

class DirectionalForce : public Force
{
public:
	DirectionalForce() : Force()
	{
	
		
	}
	
	DirectionalForce(const DirectionalForce& other) : Force(other)
	{
		magnitude = other.magnitude;
	}
	
	void funcA(){}
	
	glm::vec2 magnitude;
private:

};

class Emitter{
public :
	Emitter(){
		
	}
	Emitter(const Emitter& other)
	{
		position = other.position;
		amount = other.amount;
		internal_counter = other.internal_counter;
		partial_counter = other.partial_counter;
	}
	virtual ~Emitter(){};
	virtual void funcA() = 0;
	
	glm::vec3 position;
	int amount = 1;
	long int internal_counter = 0;
	float partial_counter = 0.0f;
private :
};

class PointEmitter : public Emitter
{
public :
	PointEmitter() : Emitter()
	{
		
	}
	PointEmitter(const PointEmitter& other) : Emitter(other)
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
	
	RectEmitter(const RectEmitter& other) : Emitter(other)
	{
		size = other.size;
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
	
	Particle(const Particle& other)
	{
		id = other.id;
		position = other.position;
		velocity = other.velocity;
		acceleration = other.acceleration;
		age = other.age;
		life = other.life;
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
		ParticleSystem(const ParticleSystem& other);
		virtual ~ParticleSystem();
		
		void spawnParticles(unsigned int n);
		void emitParticles(float amout_mult);
		void update();
		
		inline std::vector<Particle> getParticles() const {
			return m_particles;
		}
		
		inline std::vector< std::shared_ptr<Force> > getForces() const{
			return m_forces;
		}
		
		inline void addForce(std::shared_ptr<Force> force)
		{
			m_forces.push_back(force);
		}
		
		void addEmitter(PARTICLE_EMITTER_TYPE type);
		void addEmitter(std::shared_ptr<Emitter> ptr);
		
		std::vector<std::shared_ptr<Emitter> > getEmitters() const 
		{
			return m_emitters;
		}
			
		inline void setTimer(std::shared_ptr<Timer> timer_ptr)
		{
			m_timer = timer_ptr;
		}
		inline std::shared_ptr<Timer> getTimer() const {
			return m_timer;
		}		
		
	private:
		/* add your private declarations */
		std::vector<Particle> m_particles;
		std::vector<std::shared_ptr<Emitter> > m_emitters;
		std::shared_ptr<Timer> m_timer;
		
		std::vector< std::shared_ptr<Force> > m_forces;
		
		std::mt19937 rng;
		
		unsigned int max_particles = 1000;
		
};

#endif /* PARTICLE_SYSTEM_H */ 
