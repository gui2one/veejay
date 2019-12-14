#ifndef OPENGL_DEBUG_H
#define OPENGL_DEBUG_H
// opengl debugging
#include <stdio.h>
#include <iostream>
#include <csignal>

#define ASSERT(x) if(!(x)) std::raise(SIGINT);
#define GLCall(x) GLClearError(); \
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))


static void GLClearError()
{
        while(glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
        while( GLenum error = glGetError()){
 
				std::string error_str = "";
				
				switch(error){
					case GL_INVALID_ENUM :
						error_str = "GL_INVALID_ENUM";
						break;
					case GL_INVALID_VALUE :
						error_str = "GL_INVALID_VALUE";
						break;	
					case GL_INVALID_OPERATION :
						error_str = "GL_INVALID_OPERATION";
						break;												
					case GL_INVALID_FRAMEBUFFER_OPERATION :
						error_str = "GL_INVALID_FRAMEBUFFER_OPERATION";
						break;		
					case GL_OUT_OF_MEMORY :
						error_str = "GL_OUT_OF_MEMORY";
						break;																								
				}
                std::cout << "[OpenGL Error] (" << error_str << ") : " << function << " " << file <<  " : " << line << std::endl;
    
                return false;
        }
    
        return true;
}

#endif /* OPENGL_DEBUG_H */
