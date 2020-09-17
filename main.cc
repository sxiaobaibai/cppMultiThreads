#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <atomic>

struct timer
{
	float clock_;
	timer(float clock): clock_(clock){}
	void step()
	{
		sleep(1);
	}
};


void compute()
{
	std::cout << "compute" << std::endl;
}

void reading()
{
	while(1)
	{
		std::cout << "reading" << std::endl;
	}
}

void output()
{
	while(1)
	{
		std::cout << "output" << std::endl;
	}
}

struct pool
{
	std::vector<std::thread> ths_;
	timer t_;
	std::mutex mtx_;
	std::vector<std::unique_ptr<std::condition_variable>> cv_vec_;
	//std::unique_ptr<std::condition_variable> cv_ =std::make_unique<std::condition_variable>();
	std::vector<bool> is_ready_vec_;


	pool(const timer &t):t_(t){}
	void each_loop(void(*f)(void))
	{
		while(1)
		{
			f();
			std::unique_lock<std::mutex> uniq_lk(mtx_);
			cv_vec_[0]->wait(uniq_lk, [this](){return is_ready_vec_[0];});
			is_ready_vec_[0] = false;
		}
	}
	void add(void(*f)(void))
	{
		cv_vec_.push_back(std::make_unique<std::condition_variable>());
		is_ready_vec_.push_back(false);

		ths_.push_back(std::thread([this, f](){each_loop(f);}));
	}
	void exec()
	{
		while(1)
		{
			t_.step();
			{
				std::lock_guard<std::mutex> lock(mtx_);
				is_ready_vec_[0] = true;
			}
			cv_vec_[0]->notify_one();
		}
	}
};

int main(int argc, char const* argv[])
{
	timer time(1);
	pool threads(time);
	threads.add(compute);
	threads.exec();
	return 0;
}
