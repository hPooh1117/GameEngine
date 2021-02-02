#include "JobSystem.h"

#include "ThreadSafeContainers.h"
#include "./Utilities/Log.h"

#include <thread>
#include <condition_variable>
#include <sstream>
#include <algorithm>
#include <Windows.h>
#include <cassert>

namespace JobSystem
{
    struct Job
    {
        Context* ctx = nullptr;
        std::function<void(JobArgs)> task = [](JobArgs){};
        uint32_t groupID = 0;
        uint32_t groupJobOffset = 0;
        uint32_t groupJobEnd = 0;
        uint32_t sharedmemory_size = 0;
    };

    uint32_t numThreads = 0;
    Containers::ThreadSafeRingBuffer<Job, 256> jobQueue;
    std::condition_variable wakeCondition;
    std::mutex wakeMutex;

    // jobQueueの次のアイテムを実行（戻り値：trueなら成功、falseなら実行可能ジョブがない
    inline bool Work()
    {
        Job job;
        if (jobQueue.PopFront(job))
        {
            JobArgs args;
            args.groupID = job.groupID;
            if (job.sharedmemory_size > 0)
            {
                // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/malloca?view=msvc-160
                args.sharedmemory = _malloca(job.sharedmemory_size);
            }
            else
            {
                args.sharedmemory = nullptr;
            }

            for (uint32_t i = job.groupJobOffset; i < job.groupJobEnd; ++i)
            {
                args.jobIndex = i;
                args.groupIndex = i - job.groupJobOffset;
                args.isFirstJobInGroup = (i == job.groupJobOffset);
                args.isLastJobInGroup = (i == job.groupJobEnd - 1);
                job.task(args);
            }
            job.ctx->counter.fetch_sub(1);
            return true;
        }
        return false;
    }

    void Initialize()
    {
        // システムからハードウェアスレッド数を取得
        auto numCores = std::thread::hardware_concurrency();

        // ワーカスレッドの数を計算
        numThreads = (std::max)(1u, numCores - 1);

        for (uint32_t threadID = 0; threadID < numThreads; ++threadID)
        {
            std::thread worker([] {
                // memory leak!!
                while (true)
                {
                    if (!Work())
                    {
                        std::unique_lock<std::mutex> lock(wakeMutex);
                        wakeCondition.wait(lock);
                    }
                }
            });

#ifdef WIN32
            // Windows特有のスレッド設定を実行
            // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setthreadaffinitymask
            // https://postd.cc/c11-threads-affinity-and-hyperthreading/
            HANDLE handle = (HANDLE)worker.native_handle();

            // スレッドごとにアフィニティを設定する
            // １つまたは事前に定義された複数のCPU上でスレッドを実行するよう、OSにスケジューリングさせる。
            DWORD_PTR affinityMask = 1ull << threadID;
            DWORD_PTR affinity_result = SetThreadAffinityMask(handle, affinityMask);
            assert(affinity_result > 0);

            // スレッドプライオリティを上げる
            BOOL priority_result = SetThreadPriority(handle, THREAD_PRIORITY_HIGHEST);
            assert(priority_result != 0);

            // スレッドに名前を付ける
            std::wstringstream wss;
            wss << "JobSystem_" << threadID;
            HRESULT hr = SetThreadDescription(handle, wss.str().c_str());
            assert(SUCCEEDED(hr));
#endif // WIN32

            worker.detach();
        }

        Log::Info("JobSystem Initilaized With [%d cores] [%d threads]", numCores, numThreads);

    }

    uint32_t GetThreadCount()
    {
        return numThreads;
    }

    void Execute(Context& ctx, const std::function<void(JobArgs)>& task)
    {
        // Context Stateを更新
        ctx.counter.fetch_add(1);

        // jobの初期化
        Job job;
        job.ctx = &ctx;
        job.task = task;
        job.groupID = 0;
        job.groupJobOffset = 0;
        job.groupJobEnd = 1;
        job.sharedmemory_size = 0;

        // プッシュに成功するまで新しいジョブをプッシュしつづける
        while (!jobQueue.PushBack(job))
        {
            wakeCondition.notify_all();
        }

        // スリープ中のスレッドをそれか起こす。
        wakeCondition.notify_one();
    }

    void Dispatch(
        Context& ctx,
        uint32_t job_count, 
        uint32_t group_size,
        const std::function<void(JobArgs)>& task, 
        size_t sharedmemory_size)
    {
        if (job_count == 0 || group_size == 0)
        {
            return;
        }

        const uint32_t groupCount = GetDispatchGroupCount(job_count, group_size);

        // Context Stateを更新
        ctx.counter.fetch_add(groupCount);

        Job job;
        job.ctx = &ctx;
        job.task = task;
        job.sharedmemory_size = (uint32_t)sharedmemory_size;

        for (uint32_t groupID = 0; groupID < groupCount; ++groupID)
        {
            // それぞれのグループについて、実際にジョブを作る
            job.groupID = groupID;
            job.groupJobOffset = groupID * group_size;
            job.groupJobEnd = (std::min)(job.groupJobOffset + group_size, job_count);

            // 成功するまで、新しいジョブをプッシュしつづける
            while (!jobQueue.PushBack(job))
            {
                wakeCondition.notify_all();
            }

           
            // スリープ状態のスレッドを起こす
            wakeCondition.notify_all();
        }
    }

    uint32_t GetDispatchGroupCount(uint32_t job_count, uint32_t group_size)
    {
        // Dispatchのためにジョブグループの量を計算
        return (job_count + group_size - 1) / group_size;
    }

    bool IsBusy(const Context& ctx)
    {
        // contextラベルが０より大きかったら、完了していないジョブがある
        return ctx.counter.load() > 0;
    }

    void Wait(const Context& ctx)
    {
        // スリープ状態のスレッドを起こす
        wakeCondition.notify_all();

        // 未完了のジョブがあれば、実行しつづける
        while (IsBusy(ctx)) { Work(); }
    }

}
