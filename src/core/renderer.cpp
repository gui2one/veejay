#include "renderer.h"


Renderer::Renderer()
{

}

void Renderer::initTexture()
{

		
	if( m_texture != 0)
	{
		glDeleteTextures(1, &m_texture);
		glGenTextures(1, &m_texture); // Generate one texture
	}else{
		glGenTextures(1, &m_texture); // Generate one texture
	}
	glBindTexture(GL_TEXTURE_2D, m_texture); // Bind the texture fbo_texture

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); // Create a standard texture with the width and height of our window

	// Setup the basic texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);
	

}


void Renderer::initFBO(int w, int h)
{
	texture_width = w;
	texture_height = h;
	
	initTexture();
	
	fbo_shader.loadVertexShaderSource("../src/shaders/fbo_shader.vert");
	fbo_shader.loadFragmentShaderSource("../src/shaders/fbo_shader.frag");
	fbo_shader.createShader();
	
	if(m_fbo != 0)
	{
		glDeleteFramebuffersEXT(1, &m_fbo); 
	}
	
	glGenFramebuffersEXT(1, &m_fbo); // Generate one frame buffer and store the ID in fbo
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo); // Bind our frame buffer

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_texture, 0); // Attach the texture fbo_texture to the color buffer in our frame buffer

	//~ glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, viewport_FBO_depth); // Attach the depth buffer fbo_depth to our frame buffer

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); // Check that status of our generated frame buffer

	if (status != GL_FRAMEBUFFER_COMPLETE_EXT) // If the frame buffer does not report back as complete
	{
		std::cout << "Couldn't create frame buffer, haaaaaa ?!" << std::endl; // Output an error to the console
		exit(0); // Exit the application
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // Unbind our frame buffer		
}




void Renderer::updateModules()
{
	for(auto module : m_modules)
	{
		module->update();
	}	
}

void Renderer::render()
{




	GLCall(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo)); // Bind our frame buffer for rendering 
	GLCall(glPushAttrib(GL_VIEWPORT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT | GL_COLOR_BUFFER_BIT)); // Push our glEnable and glViewport states 
	


		
	float aspect = (float)texture_width / (float)texture_height;
	GLCall(glViewport(0, 0, (float)texture_width, (float)texture_height));
	

	//~ GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		
	GLCall(glClearColor (0.0f, 0.0f, 0.0f, 0.0f)); // Set the clear colour 
	GLCall(glClear (GL_COLOR_BUFFER_BIT)); // Clear the depth and colour buffers  		
	
	std::vector<std::shared_ptr<Module> > copy = m_modules;
	std::reverse(std::begin(copy), std::end(copy));
	for(auto module : copy)
	{	
		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glEnable(GL_BLEND));			
		switch(module->p_blending->getValue()){
			case MODULE_BLENDING_NORMAL :
				GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
				break;
			case MODULE_BLENDING_ADD :
				GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE));
				break;
			case MODULE_BLENDING_MULTIPLY :
				GLCall(glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA));
				break;				
			default:
				break;
			
			
			
		}
		
		glPushMatrix();
		GLCall(glOrtho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, 1.0, -1.0));	
		
		module->render();
		glPopMatrix();

	}
	
	GLCall(glPopAttrib()); // Restore our glEnable and glViewport states 
	GLCall(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0)); // Unbind our texture	
	

}


void Renderer::displayScreen()
{
	GLCall(glDisable(GL_CULL_FACE));
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	fbo_shader.useProgram();

		
		
	GLCall(glClearColor (0.0f, 0.0f, 0.0f, 0.0f)); // Set the clear colour 
	GLCall(glClear (GL_COLOR_BUFFER_BIT)); // Clear the depth and colour buffers  
				
	GLCall(glActiveTexture(GL_TEXTURE0));

	glEnable( GL_TEXTURE_2D );
	glBindTexture (GL_TEXTURE_2D, m_texture);
	
	glPushMatrix();
	
	glBegin(GL_QUADS);
		// Front Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad
	glEnd();
	
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	
	
	GLCall(glUseProgram(0));
	
}
