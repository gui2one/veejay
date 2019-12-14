#include "timer.h"


Timer::Timer()
{
	
}

void Timer::start()
{
	m_start_time = std::chrono::steady_clock::now();
	

}

int Timer::getMillis(){
	return m_millis;
}
void Timer::update()
{
	m_current_time = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration(m_current_time - m_start_time);
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
	m_millis = (int)millis.count();
	//~ std::cout << "milliseconds : " << (millis.count()) << std::endl;
}

