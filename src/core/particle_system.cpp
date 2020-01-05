#include "particle_system.h"


ParticleSystem::ParticleSystem(std::shared_ptr<Timer> timer)
{
	m_timer = timer;
	m_particles.clear();

}

ParticleSystem::~ParticleSystem()
{
	
}

void ParticleSystem::update()
{	
	// delete too old
	for(unsigned int i = getParticles().size(); i>0; --i)
	{
	
		if( m_particles[i-1]->age >= m_particles[i-1]->life)
		{
			m_particles.erase(m_particles.begin() + (i-1), m_particles.begin() + (i-1) + 1);
		}
	}
	
	float delta_t = (float)m_timer->getDeltaMillis() / 1000;
	for(auto particle : m_particles){
		for( auto force : m_forces)
		{
			DirectionalForce * p_direct = nullptr;
			
			if( (p_direct = dynamic_cast<DirectionalForce *>(force.get())))
			{
				particle->velocity.x += p_direct->magnitude.x ;
				particle->velocity.y += p_direct->magnitude.y;
			}
		}
		
		particle->age += delta_t;
		particle->position.x += particle->velocity.x  * delta_t;
		particle->position.y += particle->velocity.y  * delta_t;			
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
					std::shared_ptr<Particle> p = std::make_shared<Particle>();
					
					p->position = emitter->position; 
					
					float rand_speed = 1.0f;
					float rand_x = ((float)rng() / rng.max()) * rand_speed;
					float rand_y = ((float)rng() / rng.max()) * rand_speed;
					float rand_z = ((float)rng() / rng.max()) * rand_speed;
					
					p->velocity = glm::vec3(rand_x, rand_y, rand_z);
							
					m_particles.push_back(p);						
				}
				
				p_rect->internal_counter = 0;			
			}
		}
	}
}

void ParticleSystem::emitParticles(float amount_mult) 
{
	if( getParticles().size() < max_particles)
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
					//~ std::cout << "emit !!!!" << std::endl;
					
					float temp_partial = ((float)(p_rect->internal_counter) / (1000.0 / (p_rect->amount * amount_mult)) ); 
					p_rect->partial_counter += temp_partial;
					
					unsigned int num;
					num = (unsigned int)temp_partial;
					//~ if( p_rect->partial_counter > 1.0f && num == 0)
					//~ {
						//~ num += 1;
						//~ p_rect->partial_counter = 0.0f;
					//~ }	
						
					 				
					
					//~ if( num == 0)	num = 1;
					
					m_particles.reserve(m_particles.size() + num);
									
					for(size_t i=0; i<num; i++)
					{
						std::shared_ptr<Particle> p = std::make_shared<Particle>();
						
						p->position = emitter->position; 
						
						float rand_speed = ((float)rng() / rng.max());
						float rand_x = ((float)rng() / rng.max()) * 2.0 - 1.0;
						float rand_y = ((float)rng() / rng.max()) * 2.0 - 1.0;
						//~ float rand_z = ((float)rng() / rng.max()) * 2.0 - 1.0;
						
						p->velocity = glm::normalize(glm::vec3(rand_x, rand_y, 0.0f)) * rand_speed;
											
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
