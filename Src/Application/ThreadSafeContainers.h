#pragma once
#include "SpinLock.h"

namespace Containers
{
	// �X���b�h�Z�[�t�ȃ����O�o�b�t�@
	template<typename T, size_t capacity>
	class ThreadSafeRingBuffer
	{
	private:
		T mData[capacity];
		size_t mHead = 0;
		size_t mTail = 0;
		SpinLock mLock;
	public:
		// ���g�p�X�y�[�X������΃A�C�e���𖖔��ɑ}���i�߂�l�Ftrue�Ȃ琬���Afalse�Ȃ�X�y�[�X�s���j
		inline bool PushBack(const T& item)
		{
			bool result = false;
			mLock.Lock();
			size_t next = (mHead + 1) % capacity;
			if (next != mTail)
			{
				mData[mHead] = item;
				mHead = next;
				result = true;
			}
			mLock.Unlock();
			return result;
		}

		// �g�p���X�y�[�X�Ȃ�A�C�e�����擾
		inline bool PopFront(T& item)
		{
			bool result = false;
			mLock.Lock();
			if (mTail != mHead)
			{
				item = mData[mTail];
				mTail = (mTail + 1) % capacity;
				result = true;
			}
			mLock.Unlock();
			return result;
		}
	};
}