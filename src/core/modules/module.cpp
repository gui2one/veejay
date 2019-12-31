#include "module.h"

Module::Module(std::shared_ptr<Timer> timer){
	//~ param_layout.params.clear();
	
	m_timer = timer;
	
	p_name = std::make_shared<Param<std::string> >();
	p_name->setName("name");
	p_name->setValue(getName());
	param_layout.addParam(p_name);
	
	
	p_opacity = std::make_shared<Param<float> >();
	p_opacity->setName("Opacity");
	p_opacity->setValue(1.0);
	param_layout.addParam( p_opacity);		
	
	p_color = std::make_shared<ParamColor3 >();
	p_color->setName("Color");
	p_color->color = glm::vec3(1.0,1.0,1.0);
	param_layout.addParam( p_color);	
	
	
	std::vector<const char *> _entries = {"Normal", "Add", "Multiply"};
	p_blending = std::make_shared<ParamMenu>(_entries);
	p_blending->setName("Blending Mode");
	param_layout.addParam(p_blending);
	
	param_layout.addParam( std::make_shared<ParamSeparator>());	
	
	
	p_signal_range = std::make_shared<Param<SignalRange > >();
	p_signal_range->setName("Signal Range");

	
}
Module::~Module()
{
	printf("Deleting Module\n");
}



