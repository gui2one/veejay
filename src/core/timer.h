#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <ctime>
#include "../pch.h"
class Timer
{
	public:
	
		Timer();
		void start();
		void update();
		int getMillis();

		
	private:
	
		int m_millis;
		std::chrono::steady_clock::time_point m_start_time, m_current_time;

		
};

#endif /* TIMER_H */ 
