#include "image.h"

Image::Image(std::shared_ptr<Timer> timer)
	: Module(timer)
{
	setType(MODULE_TYPE_IMAGE);
	
	p_path = std::make_shared<ParamFilePath>();
	p_path->setName("File Path");
	p_path->setValue("/home/pi/Pictures/duckling.jpeg");	
	p_path->setCallback( [this](){
		this->loadFile(this->p_path->getValue().c_str());
	});
	param_layout.addParam(p_path);

	
}

Image::Image(const Image& other) : Module(other)
{
	p_path = std::make_shared<ParamFilePath>(*(other.p_path));
	p_path->setCallback( [this](){
		this->loadFile(this->p_path->getValue().c_str());
	});	
	param_layout.addParam(p_path);
	//~ m_texture = Texture(other.m_texture);
}

void Image::loadFile(const char * path)
{
	m_texture.load(std::string(path));
}

void Image::init()
{
	loadFile(p_path->getValue().c_str());
	
	screen_shader.loadVertexShaderSource("../src/shaders/screen_shader.vert");
	screen_shader.loadFragmentShaderSource("../src/shaders/screen_shader.frag");
	screen_shader.createShader();
	
	
	GLCall(glGenBuffers(1, &m_vbo));
	float vertices[4 * 3 * 2] = {
			-1.0, -1.0, 0.0,	0.0, 0.0,
			 1.0, -1.0, 0.0,	1.0, 0.0,
			 1.0,  1.0, 0.0,	1.0, 1.0,
			-1.0,  1.0, 0.0,	0.0, 1.0		
	};
	
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	
	GLCall(glBufferData(GL_ARRAY_BUFFER,sizeof(float) * 4 * 3 * 2, vertices, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));	
}

void Image::update(float * fft_maximums)
{
	m_opacity = p_opacity->getFilteredValue(fft_maximums);
}

void Image::render()
{
	GLCall(glEnable(GL_TEXTURE_2D));
	
	screen_shader.useProgram();
	
	GLint loc;
	loc = glGetUniformLocation(screen_shader.m_id, "u_color");
	glm::vec3 _color = glm::vec3(p_color->color);
	GLCall(glUniform3fv(loc, 1, glm::value_ptr(_color)));	

	loc = glGetUniformLocation(screen_shader.m_id, "u_opacity");
	
	GLCall(glUniform1f(loc, m_opacity));	
		
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texture.getID()));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float)*5, 0));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(float)*5, (void *)(sizeof(float) * 3)));
	
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glEnableVertexAttribArray(1));
	
	GLCall(glDrawArrays(GL_QUADS, 0, 4));
	
	GLCall(glDisableVertexAttribArray(0));
	GLCall(glDisableVertexAttribArray(1));
	
	GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	
	GLCall(glUseProgram(0));
}



