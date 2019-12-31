#ifndef MODULE_H
#define MODULE_H

#include "../../pch.h"
#include "../../param.h"
#include "../timer.h"

enum MODULE_TYPE
{
	MODULE_TYPE_NULL,
	MODULE_TYPE_ORBITER,
	MODULE_TYPE_CIRCLES,
	MODULE_TYPE_IMAGE,
	MODULE_TYPE_PARTICLES
};

enum MODULE_BLENDING
{
	MODULE_BLENDING_NORMAL,
	MODULE_BLENDING_ADD,
	MODULE_BLENDING_MULTIPLY
	
};

class Module
{
	public:
		Module(std::shared_ptr<Timer> timer = nullptr);
		virtual ~Module();
		virtual void init(){};
		virtual void update(float * fft_maximums){};
		virtual void render(){};
		
		inline void 		setName(std::string _name)
		{ 
			p_name->setValue(_name); 
		}		
		inline const std::string getName(){ return p_name->getValue();	}		
		
		inline void 	   setType(MODULE_TYPE _type){ type = _type; }		
		inline MODULE_TYPE getType(){ return type; }		
		
		inline void 	   setBlendingMode(MODULE_BLENDING _mode){ blending_mode = _mode; }		
		inline MODULE_BLENDING getBlendingMode(){ return blending_mode; }	
				
		ParamLayout param_layout;
		std::shared_ptr<Param<std::string> > p_name;
		std::shared_ptr<ParamColor3 > p_color;
		std::shared_ptr<Param<float> > p_opacity;		
		std::shared_ptr<ParamMenu > p_blending;		
		std::shared_ptr<Param<SignalRange> > p_signal_range;		
	private:
		MODULE_BLENDING blending_mode = MODULE_BLENDING_NORMAL;
		MODULE_TYPE type = MODULE_TYPE_NULL;
		std::string name = "default name";
		std::shared_ptr<Timer> m_timer;
		
		/* add your private declarations */
};

#endif /* MODULE_H */ 
