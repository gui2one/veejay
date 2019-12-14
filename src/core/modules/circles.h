#ifndef CIRCLES_H
#define CIRCLES_H

#include "../../pch.h"
#include "module.h"
#include "../shader.h"

class Circles: public Module
{
	public:
		Circles();
		void init();
		void update();
		void render();		
			
	private:
	
		Shader m_shader;
		/* add your private declarations */
};

#endif /* CIRCLES_H */ 
