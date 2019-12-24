#ifndef ORBITER_H
#define ORBITER_H

#include "../../pch.h"
#include "module.h"
#include "../shader.h"

class Orbiter: public Module
{
	public:
		Orbiter();
		void init();
		void update(float * fft_maximums);
		void render();
	private:
		std::shared_ptr<Param<float> > p_radius;
		std::shared_ptr<Param<float> > p_speed;

		unsigned int m_vbo = 0;
		
		Shader m_shader;
		int m_counter = 0;
		float m_radius;
		float m_opacity;
		float m_speed;
		
		/* add your private declarations */
};

#endif /* ORBITER_H */ 
