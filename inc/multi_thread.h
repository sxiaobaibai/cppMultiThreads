#ifndef MULTI_THREAD_H
#define MULTI_THREAD_H

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "thread_timer.h"

template<typename T=thread_timer>
struct MultiThread
{
	std::vector<std::thread> ths_;
	T t_;
	std::mutex mtx_;
	std::vector<std::unique_ptr<std::condition_variable>> cv_vec_;
	std::vector<bool> is_ready_vec_;
	std::vector<int> freq_map_;

	MultiThread(const T &t):t_(t){}

	void realtime_loop(void(*f)(void), size_t idx)
	{
		while(1)
		{
			f();
			{
				std::unique_lock<std::mutex> uniq_lk(mtx_);
				cv_vec_[idx]->wait(uniq_lk, [this, idx](){return is_ready_vec_[idx];});
				is_ready_vec_[idx] = false;
			}
		}
	}
	void async_loop(void(*f)(void), size_t idx)
	{
		(void)idx;
		while(1)
			f();
	}
	void add_thread(void(*f)(void), int freq)
	{
		size_t id = freq_map_.size();

		cv_vec_.push_back(std::make_unique<std::condition_variable>());
		is_ready_vec_.push_back(false);
		freq_map_.push_back(freq);
		if (freq > 0)
			ths_.push_back(std::thread([this, f, id](){realtime_loop(f, id);}));
		else
			ths_.push_back(std::thread([this, f, id](){async_loop(f, id);}));
	}

	void exec()
	{
		static unsigned long long cnt(0);
		while(1)
		{
			t_.step();
			cnt++;
			for (int i = 0; i < freq_map_.size(); i++) {
				if (freq_map_[i] <= 0)
					continue;
				if (cnt % freq_map_[i] == 0)
				{
					{
						std::lock_guard<std::mutex> lock(mtx_);
						is_ready_vec_[i] = true;
					}
					cv_vec_[i]->notify_one();
				}
			}
		}
	}
	void join_all()
	{
		for (auto& th : ths_)
			th.join();
	}
};
#endif
