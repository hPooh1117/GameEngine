#pragma once
#include "./Utilities/CommonInclude.h"
#include <functional>
#include <atomic>

struct JobArgs
{
	uint32_t jobIndex;       // HLSLのSV_DispatchThreadIDのように、ジョブのインデックスを設定
	uint32_t groupID;        // HLSLのSV_GroupIDのように、グループのインデックスを設定
	uint32_t groupIndex;     // HLSLのSV_GroupIndexのように、グループ内でのジョブのインデックスを設定
	bool isFirstJobInGroup;  // 現在のジョブはグループの最初のジョブか？
	bool isLastJobInGroup;   // 現在のジョブはグループの最後のジョブか？
	void* sharedmemory;      // 現在のグループ内における共有メモリをスタックする
};

namespace JobSystem
{
	void Initialize();

	uint32_t GetThreadCount();

	// 実行 or 待機のステートを定義
	struct Context
	{
		std::atomic<uint32_t> counter{ 0 };
	};

	// 非同期実行するタスクを加える。未使用スレッドがあればに実行させる
	void Execute(Context& ctx, const std::function<void(JobArgs)>& task);

	// タスクを多数のジョブに分割し、平行して実行させる
	void Dispatch(Context& ctx, uint32_t job_count, uint32_t group_size, const std::function<void(JobArgs)>& task, size_t sharedmemory_size = 0);

	// job_size、group_sizeによって作られたジョブグループの量を返す。
	uint32_t GetDispatchGroupCount(uint32_t job_count, uint32_t group_size);

	// スレッドが使用中かどうか確認
	bool IsBusy(const Context& ctx);

	// 全てのスレッドが未使用になるまで待つ
	void Wait(const Context& ctx);
}