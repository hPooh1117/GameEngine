#pragma once
#include "./Utilities/CommonInclude.h"
#include <functional>
#include <atomic>

struct JobArgs
{
	uint32_t jobIndex;       // HLSL��SV_DispatchThreadID�̂悤�ɁA�W���u�̃C���f�b�N�X��ݒ�
	uint32_t groupID;        // HLSL��SV_GroupID�̂悤�ɁA�O���[�v�̃C���f�b�N�X��ݒ�
	uint32_t groupIndex;     // HLSL��SV_GroupIndex�̂悤�ɁA�O���[�v���ł̃W���u�̃C���f�b�N�X��ݒ�
	bool isFirstJobInGroup;  // ���݂̃W���u�̓O���[�v�̍ŏ��̃W���u���H
	bool isLastJobInGroup;   // ���݂̃W���u�̓O���[�v�̍Ō�̃W���u���H
	void* sharedmemory;      // ���݂̃O���[�v���ɂ����鋤�L���������X�^�b�N����
};

namespace JobSystem
{
	void Initialize();

	uint32_t GetThreadCount();

	// ���s or �ҋ@�̃X�e�[�g���`
	struct Context
	{
		std::atomic<uint32_t> counter{ 0 };
	};

	// �񓯊����s����^�X�N��������B���g�p�X���b�h������΂Ɏ��s������
	void Execute(Context& ctx, const std::function<void(JobArgs)>& task);

	// �^�X�N�𑽐��̃W���u�ɕ������A���s���Ď��s������
	void Dispatch(Context& ctx, uint32_t job_count, uint32_t group_size, const std::function<void(JobArgs)>& task, size_t sharedmemory_size = 0);

	// job_size�Agroup_size�ɂ���č��ꂽ�W���u�O���[�v�̗ʂ�Ԃ��B
	uint32_t GetDispatchGroupCount(uint32_t job_count, uint32_t group_size);

	// �X���b�h���g�p�����ǂ����m�F
	bool IsBusy(const Context& ctx);

	// �S�ẴX���b�h�����g�p�ɂȂ�܂ő҂�
	void Wait(const Context& ctx);
}