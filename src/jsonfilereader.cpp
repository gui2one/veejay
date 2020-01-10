#include "jsonfilereader.h"

using json::JSON;
JsonFileReader::JsonFileReader()
{
	std::cout << "JSON READER" << std::endl;
}

VJ_FILE_DATA JsonFileReader::load(std::string& path)
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
	
	std::cout << modules[0]["name"] << std::endl;
	//~ for(auto mod : modules)
	//~ {
		//~ std::cout << mod["name"] << std::endl;
	//~ }
	
	VJ_FILE_DATA data;
	
	
	std::shared_ptr<Orbiter> p_orbiter = std::make_shared<Orbiter>();
	//~ p_orbiter->setName(uniqueName("Orbiter"));
	p_orbiter->p_color->color = glm::vec3(1.0, 1.0, 1.0);
	p_orbiter->init();	
	data.modules.push_back(p_orbiter);
	return data;
}

