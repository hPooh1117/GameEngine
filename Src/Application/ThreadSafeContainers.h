#pragma once
#include "SpinLock.h"

namespace Containers
{
	// スレッドセーフなリングバッファ
	template<typename T, size_t capacity>
	class ThreadSafeRingBuffer
	{
	private:
		T mData[capacity];
		size_t mHead = 0;
		size_t mTail = 0;
		SpinLock mLock;
	public:
		// 未使用スペースがあればアイテムを末尾に挿入（戻り値：trueなら成功、falseならスペース不足）
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

		// 使用中スペースならアイテムを取得
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