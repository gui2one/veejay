#include "particle_system.h"


ParticleSystem::ParticleSystem(std::shared_ptr<Timer> timer)
{
	m_timer = timer;
	m_particles.clear();

}

ParticleSystem::ParticleSystem(const ParticleSystem& other)
{
	m_timer = other.m_timer;
	
	m_particles = std::vector<Particle>();
	m_particles.clear();
	
	//~ for(auto particle : other.m_particles)
	//~ {
		//~ Particle p;
		//~ m_particles.push_back(p);
	//~ }
	
	//~ for(auto force : other.getForces())
	//~ {
		//~ DirectionalForce * p_direct = nullptr;
		
		//~ if((p_direct = dynamic_cast<DirectionalForce *>(force.get())))
		//~ {
			//~ std::shared_ptr<DirectionalForce> f = std::make_shared<DirectionalForce>(*p_direct);
			//~ addForce(f);
			//~ printf("just duplicated a DirectionForce\n");
		//~ }
		
	//~ }
	
	//~ for(auto emitter : other.getEmitters())
	//~ {
		//~ PointEmitter * p_point = nullptr;
		//~ RectEmitter * p_rect = nullptr;
		
		//~ if((p_point = dynamic_cast<PointEmitter *>(emitter.get())))
		//~ {
			//~ std::shared_ptr<PointEmitter> copy = std::make_shared<PointEmitter>(*p_point);
			//~ addEmitter(copy);
			//~ printf("just duplicated a PointEmitter\n");
		//~ }		
		//~ else if((p_rect = dynamic_cast<RectEmitter *>(emitter.get())))
		//~ {
			//~ std::shared_ptr<RectEmitter> copy = std::make_shared<RectEmitter>(*p_rect);
			//~ addEmitter(copy);
			//~ printf("just duplicated a RectEmitter\n");
		//~ }				
	//~ }
}

ParticleSystem::~ParticleSystem()
{
	
}

void ParticleSystem::update()
{	
	// delete too old
	for(unsigned int i = m_particles.size(); i>0; --i)
	{
	
		if( m_particles[i-1].age >= m_particles[i-1].life)
		{
			m_particles.erase(m_particles.begin() + (i-1), m_particles.begin() + (i-1) + 1);
		}
	}
	
	float delta_t = (float)m_timer->getDeltaMillis() / 1000;
	for(size_t i=0; i< m_particles.size(); i++){
		for( auto force : m_forces)
		{
			DirectionalForce * p_direct = nullptr;
			
			if( (p_direct = dynamic_cast<DirectionalForce *>(force.get())))
			{
				m_particles[i].velocity.x += p_direct->magnitude.x ;
				m_particles[i].velocity.y += p_direct->magnitude.y;
			}
		}
		
		m_particles[i].age += delta_t;
		
		//~ std::cout << "delta t : " << delta_t << std::endl;
		//~ std::cout << "\tlife : " << m_particles[i].life << ", age : "<< m_particles[i].age <<std::endl;
		
		m_particles[i].position.x += m_particles[i].velocity.x  * delta_t;
		m_particles[i].position.y += m_particles[i].velocity.y  * delta_t;			
	}
}

void ParticleSystem::spawnParticles(unsigned int num)
{	
	for(auto emitter : m_emitters)
	{
		RectEmitter * p_rect = nullptr;
		
		if((p_rect = dynamic_cast<RectEmitter *>(emitter.get())))
		{

			p_rect->internal_counter += m_timer->getDeltaMillis();
			if( p_rect->internal_counter > (1000 / p_rect->amount))
			{
				
				for(size_t i=0; i<num; i++)
				{
					Particle p;
					
					p.position = emitter->position; 
					
					float rand_speed = 1.0f;
					float rand_x = ((float)rng() / rng.max()) * rand_speed;
					float rand_y = ((float)rng() / rng.max()) * rand_speed;
					float rand_z = ((float)rng() / rng.max()) * rand_speed;
					
					p.velocity = glm::vec3(rand_x, rand_y, rand_z);
							
					m_particles.push_back(p);						
				}
				
				p_rect->internal_counter = 0;			
			}
		}
	}
}

void ParticleSystem::emitParticles(float amount_mult) 
{
	if( m_particles.size() < max_particles)
	{
		
		for(auto emitter : m_emitters)
		{		
			RectEmitter * p_rect = nullptr;
			
			if((p_rect = dynamic_cast<RectEmitter *>(emitter.get())))
			{			

				
				
					
					p_rect->internal_counter += m_timer->getDeltaMillis();
				if( p_rect->internal_counter > 200) // greater than 200 milliseconds
				{
					p_rect->internal_counter = 0;
				}
				

				
				if( p_rect->internal_counter > (1000 / (p_rect->amount* amount_mult)))
				{				

					
					float temp_partial = ((float)(p_rect->internal_counter) / (1000.0 / (p_rect->amount * amount_mult)) ); 
					p_rect->partial_counter += temp_partial;
					
					unsigned int num;
					num = (unsigned int)temp_partial;

					
					m_particles.reserve(m_particles.size() + num);
									
					for(size_t i=0; i<num; i++)
					{
						Particle p;
						
						p.position = emitter->position; 
						
						float rand_speed = ((float)rng() / rng.max());
						float rand_x = ((float)rng() / rng.max()) * 2.0 - 1.0;
						float rand_y = ((float)rng() / rng.max()) * 2.0 - 1.0;
						//~ float rand_z = ((float)rng() / rng.max()) * 2.0 - 1.0;
						
						p.velocity = glm::normalize(glm::vec3(rand_x, rand_y, 0.0f)) * rand_speed;
											
						m_particles.emplace_back(p);						
					}
					
					p_rect->internal_counter = 0;			
							
				}
			}
		}
	}
}

void ParticleSystem::addEmitter(PARTICLE_EMITTER_TYPE type)
{
	switch(type)
	{
		case PARTICLE_EMITTER_POINT :
		{
			std::shared_ptr<PointEmitter> point_emitter = std::make_shared<PointEmitter>();
			m_emitters.push_back(point_emitter);
			break;
		}
		case PARTICLE_EMITTER_RECT :
		{
			std::shared_ptr<RectEmitter> rect_emitter = std::make_shared<RectEmitter>();
			m_emitters.push_back(rect_emitter);
			break;		
		}
			
	}
}

void ParticleSystem::addEmitter(std::shared_ptr<Emitter> ptr)
{
	m_emitters.push_back(ptr);
}
