#ifndef PARTICLES_H
#define PARTICLES_H

#include "../../pch.h"
#include "module.h"
#include "../shader.h"
#include "../particle_system.h"

struct gl_particle{
	float x, y, z;
	float opacity;
};

class Particles: public Module
{
	public:
		Particles(std::shared_ptr<Timer> timer = nullptr);
		Particles(const Particles& other);
		~Particles();
		
		void init();
		void update(float * fft_maximums);
		
		void render();		
		
		inline std::shared_ptr<ParticleSystem>& getParticleSystem()
		{
			return m_psystem;
		}
	private:
	
		
		std::shared_ptr<Param<float> > p_emit_amount;
		std::shared_ptr<Param<float> > p_gravity;
		std::shared_ptr<ParamInfo> p_info;
		
		std::shared_ptr<DirectionalForce> gravity_force;
		unsigned int m_vbo = 0;
		std::vector<float> m_vertices;
		std::vector<gl_particle> m_gl_particles;
		
		std::shared_ptr<ParticleSystem> m_psystem;
		
		Shader m_shader;
		std::shared_ptr<Timer> m_timer;
		/* add your private declarations */
};

#endif /* PARTICLES_H */ 
