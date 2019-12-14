#ifndef JSONFILEWRITER_H
#define JSONFILEWRITER_H

#include "pch.h"
#include "core.h"

#include "vendor/SimpleJSON/json.hpp"
//~ #include <iostream>

#include "param.h"

class JsonFileWriter
{
	public:
		JsonFileWriter();
		
		json::JSON encodeModules(std::vector<std::shared_ptr<Module> >& modules);
		
		json::JSON json_data;
	private:
		/* add your private declarations */
};

#endif /* JSONFILEWRITER_H */ 
