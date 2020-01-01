#include "particles.h"


Particles::Particles(std::shared_ptr<Timer> timer)
	: Module(timer)
{
	init();
	
	
	m_psystem = std::make_shared<ParticleSystem>(timer);
	//~ m_psystem->spawnParticles(50);
	

	
	p_spawn_button = std::make_shared<ParamButton>();
	p_spawn_button->setCallback([this](){
		this->m_psystem->spawnParticles(50);
		std::mt19937 twister(1234);
		
		float delta_t = (float)getTimer()->getDeltaMillis() / 1000;
		float rand_speed = 0.05f;
		for(size_t i=0; i < 50; i++)
		{
			auto p = m_psystem->getParticles()[m_psystem->getParticles().size() - 50 + i];
			float rand_x = ((float)twister() / twister.max()) * rand_speed;
			float rand_y = ((float)twister() / twister.max()) * rand_speed;
			float rand_z = ((float)twister() / twister.max()) * rand_speed;
			
			p->velocity = glm::vec3(rand_x, rand_y, rand_z);
		}		
	});
	param_layout.addParam(p_spawn_button);
	
	
	p_gravity = std::make_shared<Param<float> >();
	p_gravity->setName("Gravity");
	param_layout.addParam(p_gravity);
	
	gravity_force = std::make_shared<DirectionalForce>();
	gravity_force->magnitude = glm::vec2(0.0f, p_gravity->getValue());
	m_psystem->addForce(gravity_force);	

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

	gravity_force->magnitude = glm::vec2(0.0f, p_gravity->getFilteredValue(fft_maximums));
	
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


