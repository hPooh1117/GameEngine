#include "ThreadPool.h"

// ハードウェアの最大スレッド数を取得
const std::size_t ThreadPool::MAX_NUM_THREADS = std::thread::hardware_concurrency();

ThreadPool::ThreadPool(std::size_t number_of_threads)
{
	// 使用できる分だけスレッドを起動する。
	for (auto i = 0; i < number_of_threads; ++i)
	{
		mThreads.emplace_back(std::thread(&ThreadPool::Execute, this));
	}


}

ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(mEventMutex);
		mbIsStopping = true;
	}

	mEventVar.notify_all();

	for (auto& thread : mThreads)
	{
		thread.join();
	}
}

void ThreadPool::Execute()
{
	while (true)
	{
		Task task;
		{
			std::unique_lock <std::mutex> lock(mEventMutex);

			mEventVar.wait(lock, [=]() { return mbIsStopping || !mTaskQueue.queue.empty(); });

			if (mbIsStopping) break;

			task = std::move(mTaskQueue.queue.front());
			mTaskQueue.queue.pop();
		}

		task();
	}
}
