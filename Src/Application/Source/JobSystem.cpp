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

    // jobQueue�̎��̃A�C�e�������s�i�߂�l�Ftrue�Ȃ琬���Afalse�Ȃ���s�\�W���u���Ȃ�
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
        // �V�X�e������n�[�h�E�F�A�X���b�h�����擾
        auto numCores = std::thread::hardware_concurrency();

        // ���[�J�X���b�h�̐����v�Z
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
            // Windows���L�̃X���b�h�ݒ�����s
            // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setthreadaffinitymask
            // https://postd.cc/c11-threads-affinity-and-hyperthreading/
            HANDLE handle = (HANDLE)worker.native_handle();

            // �X���b�h���ƂɃA�t�B�j�e�B��ݒ肷��
            // �P�܂��͎��O�ɒ�`���ꂽ������CPU��ŃX���b�h�����s����悤�AOS�ɃX�P�W���[�����O������B
            DWORD_PTR affinityMask = 1ull << threadID;
            DWORD_PTR affinity_result = SetThreadAffinityMask(handle, affinityMask);
            assert(affinity_result > 0);

            // �X���b�h�v���C�I���e�B���グ��
            BOOL priority_result = SetThreadPriority(handle, THREAD_PRIORITY_HIGHEST);
            assert(priority_result != 0);

            // �X���b�h�ɖ��O��t����
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
        // Context State���X�V
        ctx.counter.fetch_add(1);

        // job�̏�����
        Job job;
        job.ctx = &ctx;
        job.task = task;
        job.groupID = 0;
        job.groupJobOffset = 0;
        job.groupJobEnd = 1;
        job.sharedmemory_size = 0;

        // �v�b�V���ɐ�������܂ŐV�����W���u���v�b�V�����Â���
        while (!jobQueue.PushBack(job))
        {
            wakeCondition.notify_all();
        }

        // �X���[�v���̃X���b�h�����ꂩ�N�����B
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

        // Context State���X�V
        ctx.counter.fetch_add(groupCount);

        Job job;
        job.ctx = &ctx;
        job.task = task;
        job.sharedmemory_size = (uint32_t)sharedmemory_size;

        for (uint32_t groupID = 0; groupID < groupCount; ++groupID)
        {
            // ���ꂼ��̃O���[�v�ɂ��āA���ۂɃW���u�����
            job.groupID = groupID;
            job.groupJobOffset = groupID * group_size;
            job.groupJobEnd = (std::min)(job.groupJobOffset + group_size, job_count);

            // ��������܂ŁA�V�����W���u���v�b�V�����Â���
            while (!jobQueue.PushBack(job))
            {
                wakeCondition.notify_all();
            }

           
            // �X���[�v��Ԃ̃X���b�h���N����
            wakeCondition.notify_all();
        }
    }

    uint32_t GetDispatchGroupCount(uint32_t job_count, uint32_t group_size)
    {
        // Dispatch�̂��߂ɃW���u�O���[�v�̗ʂ��v�Z
        return (job_count + group_size - 1) / group_size;
    }

    bool IsBusy(const Context& ctx)
    {
        // context���x�����O���傫��������A�������Ă��Ȃ��W���u������
        return ctx.counter.load() > 0;
    }

    void Wait(const Context& ctx)
    {
        // �X���[�v��Ԃ̃X���b�h���N����
        wakeCondition.notify_all();

        // �������̃W���u������΁A���s���Â���
        while (IsBusy(ctx)) { Work(); }
    }

}
