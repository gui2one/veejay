#ifndef SHADER_H
#define SHADER_H
#include <GL/glew.h>

#include <string>

#include "../../pch.h"

class Shader
{
	public:
		Shader();
		virtual ~Shader();
		unsigned int m_id;
		void loadVertexShaderSource(std::string filePath);
		void loadFragmentShaderSource(std::string filePath);

		unsigned int compileShader(unsigned int type, const std::string& source);	
		unsigned int createShader();
		//~ unsigned int createShader( const std::string& vertexShader, const std::string& fragmentShader);
		std::string getVertexShaderSource();
		std::string getFragmentShaderSource();
		
		void useProgram();
		
	private:
		std::string vertexShaderSource;
		std::string fragmentShaderSource;
		/* add your private declarations */
};

#endif /* SHADER_H */ 
