#include "orbiter.h"


Orbiter::Orbiter(std::shared_ptr<Timer> timer)
	: Module(timer)
{
	//~ printf("Building Orbiter\n");
	
	p_radius = std::make_shared<Param<float> >();
	p_radius->setName("Radius");
	p_radius->setValue(1.0);
	param_layout.addParam( p_radius);
	
	p_speed = std::make_shared<Param<float> >();
	p_speed->setName("Speed");
	p_speed->setValue(1.0);
	param_layout.addParam(p_speed);
	
	setType(MODULE_TYPE_ORBITER);
	
	p_color->color = glm::vec3(0.0, 1.0, 0.0);
	
	m_angle = 0.0f;
	//~ setBlendingMode(MODULE_BLENDING_ADD);
}

Orbiter::Orbiter(const Orbiter & other):Module(other)
{

	p_radius = std::make_shared<Param<float> >(*(other.p_radius));
	param_layout.addParam( p_radius);
	
	p_speed = std::make_shared<Param<float> >(*(other.p_speed));
	param_layout.addParam(p_speed);
	
	setType((MODULE_TYPE) other.getType());

	
	m_angle = other.m_angle;	
}

Orbiter::~Orbiter(){
	GLCall(glDeleteBuffers(1, &m_vbo));
}

void Orbiter::init(){
	
	//~ if(m_vbo != 0)
		//~ GLCall(glDeleteBuffers(1, &m_vbo));
		
	GLCall(glGenBuffers(1, &m_vbo));
	
	float positions[9] = {
		-0.5, -0.5, 0.0,
		0.5, -0.5, 0.0,
		0.0, 0.5, 0.0
	};
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER,sizeof(float) * 9, positions, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	
	
	m_shader.loadVertexShaderSource("../src/shaders/basic_shader.vert");
	m_shader.loadFragmentShaderSource("../src/shaders/basic_shader.frag");
	m_shader.createShader();
	//~ printf("orbiter init() \n");
}

void Orbiter::update(float * fft_maximums){
	//~ printf("orbiter update \n");
	m_counter++;
	
	
	m_radius = p_radius->getFilteredValue(fft_maximums);
	m_opacity = p_opacity->getFilteredValue(fft_maximums);
	m_speed = p_speed->getFilteredValue(fft_maximums);
	
	m_angle += m_speed * getTimer()->getDeltaMillis();
	
}

void Orbiter::render(){

	m_shader.useProgram();
	
	GLint loc_color;
	loc_color = glGetUniformLocation(m_shader.m_id, "u_color");
	GLCall(glUniform3fv(loc_color, 1, glm::value_ptr(p_color->color)));
	
	GLint loc_opacity;
	loc_opacity = glGetUniformLocation(m_shader.m_id, "u_opacity");
	GLCall(glUniform1f(loc_opacity, m_opacity));	
	
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0));
		
	GLCall(glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ));

		glPushMatrix();
		
		
		glScalef(m_radius, m_radius, m_radius);
		glRotatef(m_angle, 0.0, 0.0, 1.0);

		GLCall(glDrawArrays(GL_TRIANGLES, 0, 3));
		
		GLCall(glDisableVertexAttribArray(0));
		

		glPopMatrix();		
		
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));		
	
	GLCall(glUseProgram(0));
}


