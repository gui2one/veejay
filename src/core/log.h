#ifndef LOG_H
#define LOG_H

#include "../pch.h"

#define SPDLOG_HEADER_ONLY
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class Log
{
	public:
		static void init();
		inline static std::shared_ptr<spdlog::logger>& getLogger(){ return s_logger; }
			
	private:
		static std::shared_ptr<spdlog::logger> s_logger;
	
		/* add your private declarations */
};


#define VJ_LOG_TRACE(...) Log::getLogger()->trace(__VA_ARGS__)
#define VJ_LOG_INFO(...) Log::getLogger()->info(__VA_ARGS__)
#define VJ_LOG_WARN(...) Log::getLogger()->warn(__VA_ARGS__)
#define VJ_LOG_ERROR(...) Log::getLogger()->error(__VA_ARGS__)
//~ #define VJ_LOG_FATAL(...) Log::getLogger()->fatal(__VA_ARGS__)

#endif /* LOG_H */ 
