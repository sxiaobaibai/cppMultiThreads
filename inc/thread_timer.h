#ifndef THREAD_TIMER_H
#define THREAD_TIMER_H

#include <unistd.h>

struct thread_timer
{
	float tick_;
	thread_timer(float tick): tick_(tick){}
	void step()
	{
		sleep(tick_);
	}
};
#endif
