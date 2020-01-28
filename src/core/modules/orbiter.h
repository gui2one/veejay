#ifndef ORBITER_H
#define ORBITER_H

#include "../../pch.h"
#include "module.h"
#include "../shader.h"

class Planet 
{
public:
	Planet();
	//~ Planet(const Planet & other);
	~Planet();
	
private:

	float m_radius; 
	float m_mass;
	glm::vec3 m_position;
	glm::vec3 m_velocity;
};

class Orbiter: public Module
{
	public:
		Orbiter(std::shared_ptr<Timer> timer = nullptr);
		Orbiter(const Orbiter & other);
		~Orbiter();
		void init();
		void update(float * fft_maximums);
		void render();
	private:
		std::shared_ptr<Param<float> > p_radius;
		std::shared_ptr<Param<float> > p_speed;

		unsigned int m_vbo = 0;
		
		Shader m_shader;
		int m_counter = 0;
		
		float m_angle;
		float m_radius;
		float m_opacity;
		float m_speed;
		
		std::vector<Planet> m_planets;
		
		/* add your private declarations */
};





#endif /* ORBITER_H */ 
