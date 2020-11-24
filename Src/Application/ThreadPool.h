#pragma once
#include <vector>
#include <thread>
#include <condition_variable>
#include <future>
#include <functional>
#include <queue>


// src: https://www.youtube.com/watch?v=eWTGtp3HXiw


class ThreadPool
{
public:
	using Task = std::function<void()>;
	struct TaskQueue
	{
		std::queue<Task> queue;
		std::mutex       mutex;
	};
	const static std::size_t MAX_NUM_THREADS;

private:

	std::vector<std::thread> mThreads;
	std::condition_variable  mEventVar;
	std::mutex               mEventMutex;
	bool                     mbIsStopping = false;

	TaskQueue                mTaskQueue;

public:
	ThreadPool(std::size_t number_of_threads);
	~ThreadPool();

	void Execute();

	template<class T>
	auto Enqueue(T task) -> std::future<decltype(task())>
	{
		auto wrapper = std::make_shared<std::packaged_task<decltype(task())()> >(std::move(task));

		{
			std::unique_lock<std::mutex> lock(mEventMutex);
			mTaskQueue.queue.emplace([=]()
			{
				(*wrapper)(); 
			});

			mEventVar.notify_one();
			return wrapper->get_future();
		}
	}
};

// -------------------------------------------------------------------------------------------------------
// NOTE: Thread Pool
//
// ------------------------------------------------------------------------------------
		// 
		// To get a return value from an executing thread, we use std::packaged_task<> together
		// with std::future to access the results later.
		//
		// e.g. http://en.cppreference.com/w/cpp/thread/packaged_task
		//
		// int f(int x, int y) { return std::pow(x,y); }
		//
		// 	std::packaged_task<int(int, int)> task([](int a, int b) {
		// 		return std::pow(a, b);
		// 	});
		// 	std::future<int> result = task.get_future();
		// 	task(2, 9);
		//
		// *****
		// 
		// 	std::packaged_task<int()> task(std::bind(f, 2, 11));
		// 	std::future<int> result = task.get_future();
		// 	task();
		// 
		// *****
		//
		// 	std::packaged_task<int(int, int)> task(f);
		// 	std::future<int> result = task.get_future(); 
		// 	std::thread task_td(std::move(task), 2, 10);
		// 
		// ------------------------------------------------------------------------------------

