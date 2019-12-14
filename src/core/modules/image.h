#ifndef IMAGE_MODULE_H
#define IMAGE_MODULE_H

#include "../../pch.h"
#include "../texture.h"
#include "module.h"
#include "../shader.h"
class Image: public Module
{
	public:
		Image();
		//~ ~Image();
		void loadFile(const char * path);
		void init();
		void update();
		void render();				
		
	private:
		/* add your private declarations */
		
		std::shared_ptr<ParamFilePath > p_path;
		//~ std::shared_ptr<Param<float> > p_speed;
				
		Texture m_texture;
		GLuint m_vbo;
		Shader screen_shader;
		
};

#endif /* IMAGE_MODULE_H */ 
