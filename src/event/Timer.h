# pragma once

#include "../utils.h"

typedef void (*TIMER_CALLBACK)();

enum class TIMER_TYPE {
	ONCE = 0,
	REPEAT = 1
};

class Timer {

public:
	Timer(const TIMER_TYPE type, const float msec, 
		TIMER_CALLBACK func = NULL);
	~Timer();

	void start();
	void pause();
	void expire(int val);

	void setCallback(TIMER_CALLBACK callback_func);

private:
	TIMER_TYPE type;
	TIMER_CALLBACK callback;
	float interval;	// in msec

private:
	bool enable;
};