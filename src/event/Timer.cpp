#include "Timer.h"
#include "freeGLUT/freeglut.h"
#include <iostream>
#include "../global.h"


static void callback_trampoline(int id) 
{ 
	if (id == 0) {
		global::travelTimer->expire(id); 
	}
	else if (id == 1) {
		global::fpsTimer->expire(id);
	}
}


Timer::Timer(const TIMER_TYPE type, const float msec,
	TIMER_CALLBACK func, const int id) : type(type), interval(msec),
	callback(func), enable(false), id(id)
{

}

Timer::~Timer()
{
	enable = false;
}

void Timer::start()
{
	std::cout << "timer start" << std::endl;

	glutTimerFunc(interval, callback_trampoline, id);
	enable = true;
}

void Timer::pause()
{
	std::cout << "timer pause" << std::endl;

	enable = false;
}

void Timer::expire(int val)
{
	if (enable) {

		if (callback != NULL) {
			std::cout << "call callback" << std::endl;
			callback();
		}
		
		if (type == TIMER_TYPE::ONCE) {
			enable = false;
		}
		else if (type == TIMER_TYPE::REPEAT) {
			glutTimerFunc(interval, callback_trampoline, id);
		}
	}
}