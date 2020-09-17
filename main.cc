#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <time.h>

void WaitUntilNextSample()
{
	static struct timespec present_timespec_;
	static long long int unit_time_(0);
	static long long int last_unit_time_(0);
	static long long int kTimeSampleFrequencyToSec = 10;
	static long long int kNanoToSec = 1000000000;
	while (1) {
		clock_gettime(CLOCK_MONOTONIC_RAW, &present_timespec_);
		unit_time_ = present_timespec_.tv_nsec*kTimeSampleFrequencyToSec/kNanoToSec;
		if((unit_time_)%1==0 && last_unit_time_ != unit_time_){
			last_unit_time_ = unit_time_;
			break;
		}
	}
}

struct timer
{
	float clock_;
	timer(float clock): clock_(clock){}
	void step()
	{
		//sleep(1);
		WaitUntilNextSample();
	}
};

void my_putstr(const char *str)
{
	while(*str)
		write(1, str++, 1);
}

std::vector<int> vec(3,0);


void compute()
{
	//std::cout << "compute" << std::endl;
	//my_putstr("compute\n");
	vec[0]++;
}

void reading()
{
	//std::cout << "reading" << std::endl;
	//my_putstr("reading\n");
	vec[1]++;
}

void output()
{
	//std::cout << "output" << std::endl;
	//my_putstr("output\n");
	vec[2]++;
}

void display()
{
	//std::cout << "output" << std::endl;
	///my_putstr("output\n");
	std::cout << vec[0] << ",  " << vec[1] << ", " << vec[2] << std::endl;
}

struct pool
{
	std::vector<std::thread> ths_;
	timer t_;
	//std::mutex mtx_;
	std::vector<std::unique_ptr<std::mutex>> mtx_vec_;
	std::vector<std::unique_ptr<std::condition_variable>> cv_vec_;
	//std::unique_ptr<std::condition_variable> cv_ =std::make_unique<std::condition_variable>();
	std::vector<bool> is_ready_vec_;
	std::vector<uint> freq_map_;


	pool(const timer &t):t_(t){}
	void each_loop(void(*f)(void), size_t idx)
	{
		while(1)
		{
			f();
			//std::unique_lock<std::mutex> uniq_lk(mtx_);
			std::unique_lock<std::mutex> uniq_lk(*mtx_vec_[idx]);
			cv_vec_[idx]->wait(uniq_lk, [this, idx](){return is_ready_vec_[idx];});
			is_ready_vec_[idx] = false;
		}
	}
	void add(void(*f)(void), uint freq)
	{
		mtx_vec_.push_back(std::make_unique<std::mutex>());
		cv_vec_.push_back(std::make_unique<std::condition_variable>());
		is_ready_vec_.push_back(false);
		freq_map_.push_back(freq);

		ths_.push_back(std::thread([this, f](){each_loop(f, freq_map_.size() - 1);}));
	}
	void exec()
	{
		static unsigned long long cnt(0);
		while(1)
		{
			t_.step();
			//std::cout << "debug" << std::endl;
			cnt++;
			//if (cnt % 3 == 0 )
			//{
				{
					//std::lock_guard<std::mutex> lock(mtx_);
					for (int i = 0; i < freq_map_.size(); i++) {
						std::lock_guard<std::mutex> lock(*mtx_vec_[i]);
						is_ready_vec_[i] = true;
						//if (cnt % freq_map_[i] == 0)
						//{
						//	is_ready_vec_[i] = true;
						//}
					}
				}
				for (int i = 0; i < freq_map_.size(); i++) {
					cv_vec_[i]->notify_one();
					//if (cnt % freq_map_[i] == 0)
					//{
					//	cv_vec_[i]->notify_one();
					//}
				}
			//}
		}
	}
};

int main(int argc, char const* argv[])
{
	timer time(1);
	pool threads(time);
	threads.add(compute, 1);
	threads.add(reading, 1);
	threads.add(output, 1);
	threads.add(display, 1);
	//sleep(1);
	threads.exec();
	return 0;
}
