#include <iostream>
#include "multi_thread.h"
#include "thread_timer.h"

struct sleepTimer: public thread_timer
{
	sleepTimer(float tick):thread_timer(tick){}
	virtual void step()
	{
		sleep(tick_);
	}
};

std::vector<int> vec(4,0);

void compute()
{
	vec[0]++;
}

void reading()
{
	vec[1]++;
}

void output()
{
	vec[2]++;
}

void async()
{
	vec[3]++;
}

void display()
{
	std::cout << vec[0] << ",  " << vec[1] << ", " << vec[2] << ", " << vec[3] << std::endl;
}

int main(int argc, char const* argv[])
{
	sleepTimer time(1);
	MultiThread<sleepTimer> threads(time);
	threads.add_thread(compute, 1);
	threads.add_thread(reading, 3);
	threads.add_thread(output, 5);
	threads.add_thread(async, 0);
	threads.add_thread(display, 1);

	threads.exec();
	threads.join_all();
	return 0;
}
