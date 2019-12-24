#ifndef RENDERER_H
#define RENDERER_H

#include "../pch.h"
#include "modules/module.h"
#include "modules/orbiter.h"
#include "shader.h"
class Renderer
{
	public:
		Renderer(float * _fft_maximums = nullptr);
		
		void initTexture();
		void initFBO(int w, int h);
		void updateModules();
		
		void render();
		void displayScreen();
		
		
		unsigned int m_texture, m_fbo;
		Shader fbo_shader;
		std::vector<std::shared_ptr<Module>> m_modules;
		
		int texture_width, texture_height;
		
		float * fft_maximums;
		
	private:

		/* add your private declarations */
};

#endif /* RENDERER_H */ 
