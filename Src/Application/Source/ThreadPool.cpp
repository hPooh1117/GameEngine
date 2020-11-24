#include "ThreadPool.h"

// �n�[�h�E�F�A�̍ő�X���b�h�����擾
const std::size_t ThreadPool::MAX_NUM_THREADS = std::thread::hardware_concurrency();

ThreadPool::ThreadPool(std::size_t number_of_threads)
{
	// �g�p�ł��镪�����X���b�h���N������B
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
