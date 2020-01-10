#include "jsonfilewriter.h"


using json::JSON;

JsonFileWriter::JsonFileWriter()
{
	json_data = json::Object();
}

JSON JsonFileWriter::encodeModules(std::vector<std::shared_ptr<Module> >& modules)
{
	JSON json_modules = json::Array();
	for(auto module : modules)
	{	
		
			//~ printf("module name : %s -- is an Orbiter \n", module->getName());		

			JSON mod = json::Object();
			JSON json_params = json::Array();
			mod["name"] = module->getName();
			mod["type"] = (int)module->getType();
			for(auto param : module->param_layout.params)
			{
				Param<float> * p_float = nullptr;
				Param<int> * p_int = nullptr;
				ParamMenu * p_menu = nullptr;
				ParamColor3 * p_color3 = nullptr;
				
				JSON json_param = json::Object();
				
				if(( p_float = dynamic_cast<Param<float> *>(param.get())))
				{
					json_param["name"] = p_float->getName();
					json_param["value"] = p_float->getValue();
					json_param["use_signal"] = p_float->getUseSignalRange();
				}
				else if(( p_int = dynamic_cast<Param<int> *>(param.get())))
				{
					json_param["name"] = p_int->getName();
					json_param["value"] = p_int->getValue();
				}	
				else if(( p_menu = dynamic_cast<ParamMenu *>(param.get())))
				{
					json_param["name"] = p_menu->getName();
					json_param["value"] = p_menu->getValue();
				}
				else if(( p_color3 = dynamic_cast<ParamColor3 *>(param.get())))
				{
					JSON json_color = json::Array();
					json_param["name"] = p_color3->getName();
					
					json_color.append(p_color3->getValue().x);
					json_color.append(p_color3->getValue().y);
					json_color.append(p_color3->getValue().z);
					
					json_param["value"] = json_color;
				}
				
				json_params.append(json_param);
				
				
			}	
			
			mod["params"] = json_params;
			json_modules.append(mod);

		
	}
	
	json_data["modules"] = json_modules;
	
	std::cout<< json_data << std::endl;
	
	return json_modules;
	
	
}

