#ifndef CIRCLES_H
#define CIRCLES_H

#include "../../pch.h"
#include "module.h"
#include "../shader.h"

class Circles: public Module
{
	public:
		Circles(std::shared_ptr<Timer> timer);
		void init();
		void update(float * fft_maximums);
		void render();		
			
	private:
	
		Shader m_shader;
		float m_opacity;
		/* add your private declarations */
};

#endif /* CIRCLES_H */ 
