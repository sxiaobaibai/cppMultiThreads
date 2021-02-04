#Simple C++ realtime multi threads

## basic usaage
This is a header only soft realtime multi thread program.

Include the headers
```
#include "/path/to/multi_thread.h"
#include "/path/to/thread_timer.h"
```
Create an instance of thread_timer object and MultiThread object.
```
thread_timer timer(1);// The basic sample frequency is 1s
Multithread<thread_timer> threads(timer);
```

Add function which you want to call at each sample frequency()
```
void test()
{
	//Do something.
}
threads.add_thread(test, 10);//call test function at each 10s.
```

Finish task.
```
	threads.exec();
	threads.join_all();
```

## Define your own timer
Example of defining different timer will be as follow.
```
struct highResolutionTimer: public thread_timer
{
	highResolutionTimer(float tick):thread_timer(tick){}
	virtual void step()
	{
		// write code to make system wait thick_ second.
		// one example will be usleep(1000); //wait 1ms.
		// if you want to implent a higher frequency system, you should inherit thread_timer class and define your own class.
	}
};
```

## Example
An example code is located in the directory of ./example/.
In the main.cc, five threads are created.
The five threads are called at different frequency.
(Be carefull that this example does not consider mutual exclusion for the std::vector object.)
The way of executing the example program is as follow.
```
cd example/
mkdir build
cd build
cmake ..
make
./example
```
