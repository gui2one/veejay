#ifndef JSONFILEREADER_HP
#define JSONFILEREADER_HP

#include "core.h"

#include "vendor/SimpleJSON/json.hpp"


#include "param.h"

struct VJ_FILE_DATA{
	std::vector<std::shared_ptr<Module> > modules;
};

class JsonFileReader
{
	public:
		JsonFileReader();
		
		VJ_FILE_DATA load(std::string& path, std::shared_ptr<Timer> timer_ptr);
			
	private:
	
		json::JSON json_data;
		/* add your private declarations */
};

#endif /* JSONFILEREADER_HP */ 
