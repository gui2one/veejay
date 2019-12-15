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
		int getDeltaMillis();

		
	private:
	
		int m_millis;
		int m_old_millis;
		std::chrono::steady_clock::time_point m_start_time, m_current_time;

		
};

#endif /* TIMER_H */ 
