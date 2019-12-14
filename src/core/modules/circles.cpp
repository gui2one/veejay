#include "circles.h"


Circles::Circles()
	: Module()
{
	setType(MODULE_TYPE_CIRCLES);
	m_shader.loadVertexShaderSource("../src/shaders/basic_shader.vert");
	m_shader.loadFragmentShaderSource("../src/shaders/basic_shader.frag");
	m_shader.createShader();
	
	p_color->color = glm::vec3(1.0,0.0,0.0);
}

void Circles::init()
{
	
}

void Circles::update()
{
	
}

void Circles::render()
{
	m_shader.useProgram();
	
	GLint loc;
	loc = glGetUniformLocation(m_shader.m_id, "u_color");
	glm::vec3 _color = glm::vec3(p_color->color);
	GLCall(glUniform3fv(loc, 1, glm::value_ptr(_color)));	

	loc = glGetUniformLocation(m_shader.m_id, "u_opacity");
	
	GLCall(glUniform1f(loc, p_opacity->getValue()));	
	
	glBegin(GL_TRIANGLES);
	glVertex3f( 0.0, 0.0, 0.0);
	glVertex3f( 0.5, 0.0, 0.0);
	glVertex3f( 0.0, 0.5, 0.0);
	glEnd();
	
	GLCall(glUseProgram(0));
}
