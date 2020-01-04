#include "particles.h"


Particles::Particles(std::shared_ptr<Timer> timer)
	: Module(timer)
{

	
	
	m_timer = timer;
	
	// init is called in main.cpp
	//~ init();

}

Particles::~Particles()
{
	GLCall(glDeleteBuffers(1, &m_vbo));
}

void Particles::init()
{
	m_psystem = std::make_shared<ParticleSystem>(m_timer);
	//~ m_psystem->spawnParticles(50);
	
	m_psystem->addEmitter(PARTICLE_EMITTER_RECT);
	m_psystem->getEmitters()[0]->amount = 200;
	
	p_spawn_button = std::make_shared<ParamButton>();
	p_spawn_button->setCallback([this](){
		this->m_psystem->spawnParticles(50);
		//~ std::mt19937 twister(1234);
		
		
		//~ float rand_speed = 1.0f;
		//~ for(size_t i=0; i < 50; i++)
		//~ {
			//~ auto p = m_psystem->getParticles()[m_psystem->getParticles().size() - 50 + i];
			//~ float rand_x = ((float)twister() / twister.max()) * rand_speed;
			//~ float rand_y = ((float)twister() / twister.max()) * rand_speed;
			//~ float rand_z = ((float)twister() / twister.max()) * rand_speed;
			
			//~ p->velocity = glm::vec3(rand_x, rand_y, rand_z);
		//~ }		
	});
	param_layout.addParam(p_spawn_button);
	
	
	p_gravity = std::make_shared<Param<float> >();
	p_gravity->setName("Gravity");
	p_gravity->setValue(-0.05f);
	param_layout.addParam(p_gravity);
	
	
	p_info = std::make_shared<ParamInfo>();
	p_info->setName("Num Particles");
	param_layout.addParam(p_info);
	
	gravity_force = std::make_shared<DirectionalForce>();
	gravity_force->magnitude = glm::vec2(0.0f, p_gravity->getValue());
	m_psystem->addForce(gravity_force);		
	
	GLCall(glGenBuffers(1, &m_vbo));
	
	m_shader.loadVertexShaderSource("../src/shaders/basic_particle_shader.vert");
	m_shader.loadFragmentShaderSource("../src/shaders/basic_particle_shader.frag");
	m_shader.createShader();
	
	
	//~ m_psystem->addEmitter(PARTICLE_EMITTER_RECT);	
}

void Particles::update(float * fft_maximums)
{

	
	gravity_force->magnitude = glm::vec2(0.0f, p_gravity->getFilteredValue(fft_maximums));
	m_psystem->spawnParticles(0);
	
	m_psystem->update();
	
	m_vertices.clear();
	
	
	m_vertices.reserve( m_psystem->getParticles().size() * 4);
	
	
	for( auto particle : m_psystem->getParticles())
	{
		
		m_vertices.emplace_back( particle->position.x);
		m_vertices.emplace_back( particle->position.y);
		m_vertices.emplace_back( particle->position.z);
		
		// opacity
		m_vertices.emplace_back( 1.0 - ( particle->age / particle->life));
	}
	
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER,sizeof(float) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	
	
	char info_buffer[512];
	sprintf(info_buffer, "%d", (int)m_psystem->getParticles().size()); 
	
	p_info->setValue((const char *)info_buffer);	
	
}



void Particles::render()
{
	
	m_shader.useProgram();
	GLCall(glPointSize(3));
	
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void *)0));
	GLCall(glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void *)(sizeof(float)*3)));
	
	GLCall(glDrawArrays(GL_POINTS, 0, m_vertices.size()/4));
	
	
	GLCall(glDisableVertexAttribArray(0));
	GLCall(glDisableVertexAttribArray(1));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glUseProgram(0));
}


