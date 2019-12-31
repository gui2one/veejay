#ifndef PARTICLES_H
#define PARTICLES_H

#include "../../pch.h"
#include "module.h"
#include "../shader.h"
#include "../particle_system.h"

class Particles: public Module
{
	public:
		Particles(std::shared_ptr<Timer> timer);
		~Particles();
		
		void init();
		void update(float * fft_maximums);
		
		void render();		
		
		inline std::shared_ptr<ParticleSystem>& getParticleSystem()
		{
			return m_psystem;
		}
	private:
	
	
		std::shared_ptr<ParamButton> p_spawn_button;
		unsigned int m_vbo = 0;
		std::vector<float> m_positions;
		std::shared_ptr<ParticleSystem> m_psystem;
		/* add your private declarations */
};

#endif /* PARTICLES_H */ 
