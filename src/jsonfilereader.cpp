#include "jsonfilereader.h"

using json::JSON;
JsonFileReader::JsonFileReader()
{
	//~ std::cout << "JSON READER" << std::endl;
}

static void set_param_layout_values(std::shared_ptr<Module> module_ptr, JSON json_params)
{
	unsigned int inc = 0;
	for(auto param : module_ptr->param_layout.params)
	{
		Param<float> * p_float = nullptr;
		Param<int> * p_int = nullptr;
		ParamMenu * p_menu = nullptr;
		ParamColor3 * p_color3 = nullptr;	
		ParamFilePath * p_file_path = nullptr;	
		
		if((p_float = dynamic_cast<Param<float> *>(param.get())))
		{
			p_float->setValue(json_params[inc]["value"].ToFloat());
			p_float->setUseSignalRange(json_params[inc]["use_signal"].ToBool());
			
			SignalRange range;
			range.min = json_params[inc]["signal_range"]["min"].ToInt();
			range.max = json_params[inc]["signal_range"]["max"].ToInt();
			range.mode = (SignalRangeMode)json_params[inc]["signal_range"]["mode"].ToInt();
			range.multiplier = (SignalRangeMode)json_params[inc]["signal_range"]["multiplier"].ToFloat();
			
			p_float->setSignalRange(range);
			
		}else if((p_int = dynamic_cast<Param<int> *>(param.get())))
		{
			p_int->setValue(json_params[inc]["value"].ToInt());			
		}else if((p_menu = dynamic_cast<ParamMenu *>(param.get())))
		{
			p_menu->setValue(json_params[inc]["value"].ToInt());
			
		}else if((p_color3 = dynamic_cast<ParamColor3 *>(param.get())))
		{
			JSON json_color = json_params[inc]["value"];
			p_color3->setValue(glm::vec3(json_color[0].ToFloat(), json_color[1].ToFloat(), json_color[2].ToFloat()));
		}else if((p_file_path = dynamic_cast<ParamFilePath *>(param.get())))
		{
			JSON json_path = json_params[inc]["value"];
			p_file_path->setValue(json_path.ToString());
		}
		
		inc++;
	}
}

VJ_FILE_DATA JsonFileReader::load(std::string& path, std::shared_ptr<Timer> timer_ptr)
{
	std::string line;
	std::ifstream in_file(path);
	std::string json_source = "";
	
	if(in_file.is_open())
	{
		while( std::getline(in_file, line))
		{
			json_source += line;
			
		}
		
		in_file.close();
	}
	
	json_data = json::JSON::Load(json_source);
	//~ std::cout << "loaded file : " << path << std::endl;
	//~ std::cout << "loaded source : " << json_source << std::endl;
	
	
	JSON modules = json_data["modules"];
	
	//~ std::cout << modules[0]["name"] << std::endl;
	//~ std::cout << modules.size() << std::endl;
	
	VJ_FILE_DATA data;	
	
	for(int i = 0; i< modules.size(); i++)
	{
		JSON j_int = modules[i]["type"];
		
		switch((MODULE_TYPE)(j_int.ToInt()))
		{
			case MODULE_TYPE_NULL :
			{
				
				break;
			}
			case MODULE_TYPE_PARTICLES : 
			{
				std::shared_ptr<Particles> mod = std::make_shared<Particles>(timer_ptr);
				mod->setName(modules[i]["name"].ToString());
				mod->init();	
				set_param_layout_values(mod, modules[i]["params"]);
				data.modules.push_back(mod);
				break;
			}	
			case MODULE_TYPE_ORBITER :
			{ 
				std::shared_ptr<Orbiter> mod = std::make_shared<Orbiter>(timer_ptr);
				mod->setName(modules[i]["name"].ToString());
				mod->init();	
				set_param_layout_values(mod, modules[i]["params"]);
				data.modules.push_back(mod);
				break;				
			}	
			case MODULE_TYPE_CIRCLES : 
			{
				std::shared_ptr<Circles> mod = std::make_shared<Circles>(timer_ptr);
				mod->setName(modules[i]["name"].ToString());
				mod->init();	
				set_param_layout_values(mod, modules[i]["params"]);
				data.modules.push_back(mod);
				break;		
			}
			case MODULE_TYPE_IMAGE : 
			{
				std::shared_ptr<Image> mod = std::make_shared<Image>(timer_ptr);
				mod->setName(modules[i]["name"].ToString());
				set_param_layout_values(mod, modules[i]["params"]);
				mod->init();	
				
				data.modules.push_back(mod);
				break;											
			}
		}
		
		
	}
	
	//~ std::shared_ptr<Particles> mod = std::make_shared<Particles>(timer_ptr);
	//~ mod->setName("Particles");
	//~ mod->init();	
	//~ data.modules.push_back(mod);	
	
	

	return data;
}

