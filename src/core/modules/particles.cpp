#include "particles.h"


Particles::Particles(std::shared_ptr<Timer> timer)
	: Module(timer)
{
	init();
	
	
	m_psystem = std::make_shared<ParticleSystem>();
	m_psystem->spawnParticles(50);
	
	p_spawn_button = std::make_shared<ParamButton>();
	p_spawn_button->setCallback([this](){
		this->m_psystem->spawnParticles(10);
		std::mt19937 twister(1234);
		
		float rand_speed = 0.05f;
		for(auto p : this->m_psystem->getParticles())
		{
			float rand_x = ((float)twister() / twister.max()) * rand_speed;
			float rand_y = ((float)twister() / twister.max()) * rand_speed;
			float rand_z = ((float)twister() / twister.max()) * rand_speed;
			
			p->velocity = glm::vec3(rand_x, rand_y, rand_z);
		}		
	});
	param_layout.addParam(p_spawn_button);
	
	

}

Particles::~Particles()
{
	GLCall(glDeleteBuffers(1, &m_vbo));
}

void Particles::init()
{
	GLCall(glGenBuffers(1, &m_vbo));
}

void Particles::update(float * fft_maximums)
{

	m_psystem->update();
	m_positions.clear();
	
	m_positions.reserve( m_psystem->getParticles().size() * 3);
	
	for( auto particle : m_psystem->getParticles())
	{
		
		m_positions.emplace_back( particle->position.x);
		m_positions.emplace_back( particle->position.y);
		m_positions.emplace_back( particle->position.z);
	}
	
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER,sizeof(float) * m_positions.size(), m_positions.data(), GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	
}



void Particles::render()
{
	GLCall(glPointSize(3));
	
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0));
	
	GLCall(glDrawArrays(GL_POINTS, 0, m_positions.size()/3));
	
	
	GLCall(glDisableVertexAttribArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}


