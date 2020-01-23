#include "particles.h"


Particles::Particles(std::shared_ptr<Timer> timer)
	: Module(timer)
{

	
	setType(MODULE_TYPE_PARTICLES);
	m_timer = timer;
	
	m_psystem = std::make_shared<ParticleSystem>(m_timer);	
	m_psystem->addEmitter(PARTICLE_EMITTER_RECT);
	m_psystem->getEmitters()[0]->amount = 1;
	m_psystem->getEmitters()[0]->position = glm::vec3(0.5f, 0.0f, 0.0f);
	
	
	p_gravity = std::make_shared<Param<float> >();
	p_gravity->setName("Gravity");
	p_gravity->setValue(-0.05f);
	param_layout.addParam(p_gravity);

	p_point_force_amount = std::make_shared<Param<float> >();
	p_point_force_amount->setName("Point Force");
	p_point_force_amount->setValue(0.05f);
	param_layout.addParam(p_point_force_amount);
	
	p_emit_amount = std::make_shared<Param<float> >();
	p_emit_amount->setName("Emit Amount");
	p_emit_amount->setValue(1.0f);
	param_layout.addParam(p_emit_amount);	
	
	p_info = std::make_shared<ParamInfo>();
	p_info->setName("Num Particles");
	param_layout.addParam(p_info);
	
	gravity_force = std::make_shared<DirectionalForce>();
	gravity_force->magnitude = glm::vec2(0.0f, p_gravity->getValue());
	m_psystem->addForce(gravity_force);		

	point_force = std::make_shared<PointForce>();
	point_force->magnitude = 1.0f;
	m_psystem->addForce(point_force);		
}

Particles::Particles(const Particles& other) : Module(other)
{
	setType(MODULE_TYPE_PARTICLES);
	
	m_gl_particles.clear();
	
	m_psystem = std::make_shared<ParticleSystem>(*(other.m_psystem));
	
	m_psystem->addEmitter(PARTICLE_EMITTER_RECT);
	m_psystem->getEmitters()[0]->amount = 1;
	m_psystem->getEmitters()[0]->position = glm::vec3(0.5f, 0.0f, 0.0f);
		
	p_gravity = std::make_shared<Param<float> >(*(other.p_gravity));
	param_layout.addParam(p_gravity);
	
	p_point_force_amount = std::make_shared<Param<float> >(*(other.p_point_force_amount));
	param_layout.addParam(p_point_force_amount);
	
	p_emit_amount = std::make_shared<Param<float> >(*(other.p_emit_amount));
	param_layout.addParam(p_emit_amount);
	
	p_info = std::make_shared<ParamInfo>(*(other.p_info));
	param_layout.addParam(p_info);
	
	gravity_force = std::make_shared<DirectionalForce>(*(other.gravity_force));	
	m_psystem->addForce(gravity_force);
	
	point_force = std::make_shared<PointForce>(*(other.point_force));	
	m_psystem->addForce(point_force);	
	
	
	
}

Particles::~Particles()
{
	GLCall(glDeleteBuffers(1, &m_vbo));
}

void Particles::init()
{

	
	
	GLCall(glGenBuffers(1, &m_vbo));
	
	
	
	m_shader.loadVertexShaderSource("../src/shaders/basic_particle_shader.vert");
	m_shader.loadFragmentShaderSource("../src/shaders/basic_particle_shader.frag");
	m_shader.createShader();
	
}

void Particles::update(float * fft_maximums)
{

	
	gravity_force->magnitude = glm::vec2(0.0f, p_gravity->getFilteredValue(fft_maximums));
	point_force->magnitude = p_point_force_amount->getFilteredValue(fft_maximums);
	m_psystem->emitParticles(p_emit_amount->getFilteredValue(fft_maximums));
	
	m_psystem->update();
	
	m_gl_particles.clear();
	
	
	m_gl_particles.reserve( m_psystem->getParticles().size() );
	
	
	for( auto particle : m_psystem->getParticles())
	{
		gl_particle p;
		p.x = particle.position.x;
		p.y = particle.position.y;
		p.z = particle.position.z;
		
		
		//~ m_vertices.emplace_back( particle->position.x);
		//~ m_vertices.emplace_back( particle->position.y);
		//~ m_vertices.emplace_back( particle->position.z);
		
		// opacity
		p.opacity =  1.0 - ( particle.age / particle.life);
		
		
		m_gl_particles.emplace_back(p);
	}
	
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER,sizeof(gl_particle) * m_gl_particles.size(), m_gl_particles.data(), GL_STATIC_DRAW));
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
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(gl_particle), (void *)0));
	GLCall(glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(gl_particle), (void *)(sizeof(float)*3)));
	
	GLCall(glDrawArrays(GL_POINTS, 0, m_gl_particles.size()));
	
	
	GLCall(glDisableVertexAttribArray(0));
	GLCall(glDisableVertexAttribArray(1));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glUseProgram(0));
}


