#pragma once
#include <memory>
#include <vector>
#include <list>
#include <array>
#include <unordered_map>
#include <Windows.h>

#include "./Utilities/Vector.h"

template<class T>
class Cell;

template<class T>
class ObjectForTree : public std::enable_shared_from_this<ObjectForTree<T>>
{
public:
	std::shared_ptr<Cell<T>> m_pCell;
	//Cell<T>* m_pCell = nullptr;
	std::shared_ptr<T> m_pObject;
	//T* m_pObject = nullptr;
	std::weak_ptr<ObjectForTree<T>> m_pPreObject;
	std::weak_ptr<ObjectForTree<T>> m_pNextObject;
	


private:
	ObjectForTree() {}
public:
	std::shared_ptr<ObjectForTree<T>> Initialize()
	{
		return std::shared_ptr<ObjectForTree<T>>(new ObjectForTree<T>());
	}

	virtual ~ObjectForTree() = default;

public:
	bool Remove()
	{
		if (!m_pCell) return false;
		if (!m_pCell->OnRemove(this->shared_from_this())) return false;

		if (!m_pPreObject.expired())
		{
			m_pPreObject.lock()->m_pNextObject = m_pNextObject;
			m_pPreObject.reset();
		}

		if (!m_pNextObject.expired())
		{
			m_pNextObject.lock()->m_pPreObject = m_pPreObject;
			m_pNextObject.reset();
		}
		m_pCell = nullptr;
		return true;
	}

	void RegistCell(const std::shared_ptr<Cell<T>>& pCell)
	{
		m_pCell = pCell;
	}

	inline auto& GetNextObject() { return m_pNextObject; }
};


template<class T>
class Cell : public std::enable_shared_from_this<Cell<T>>
{
protected:
	std::weak_ptr<ObjectForTree<T>> m_pLatest;
private:
	Cell()
	{
	}

public:
	std::shared_ptr<Cell<T>> Initialize()
	{
		return std::shared_ptr<Cell<T>>(new Cell<T>());
	}

	virtual ~Cell()
	{
		if (!m_pLatest.expired()) ResetLink(m_pLatest);
	}

	// リンクを全てリセットする
	void ResetLink(std::weak_ptr<ObjectForTree<T>>& obj)
	{
		if (!obj.lock()->m_pNextObject.expired())
			ResetLink(obj.lock()->m_pNextObject);

		obj.reset(); // リセット
	}

	// ObjectForTreeをプッシュ
	bool Push(const std::shared_ptr<ObjectForTree<T>>& obj)
	{
		if (obj == nullptr) return false; // 無効オブジェクトは登録しない
		if (obj->m_pCell == this->shared_from_this()) return false; // 二重登録チェック

		if (m_pLatest.expired()) m_pLatest = obj; // 空間に新規登録
		else
		{
			// 最新ObjectForTreeオブジェクトを更新
			obj->m_pNextObject = m_pLatest;
			m_pLatest.lock()->m_pPreObject = obj;
			m_pLatest = obj;
		}

		obj->RegistCell(this->shared_from_this());
		return true;
	}

	std::weak_ptr<ObjectForTree<T>>& GetFirstObj()
	{
		return m_pLatest;
	}

	bool OnRemove(const std::shared_ptr<ObjectForTree<T>>& pRemoveObj)
	{
		if (m_pLatest.lock() == pRemoveObj)
		{
			if (m_pLatest.lock() != nullptr)
				m_pLatest = m_pLatest.lock()->GetNextObject();

		}
		return true;
	}
};



template<class T>
class LinearQuadTree
{
public:
	static constexpr int MAX_LEVEL = 8;

protected:
	unsigned int mDimention = 0;
	std::unordered_map<unsigned int, std::shared_ptr<Cell<T>>> m_pCellmap;  // 線形空間ポインタ配列
	std::array<unsigned int, MAX_LEVEL + 1> mPowArray;           // べき乗数値配列

	Vector3 mWidth = Vector3(1.0f, 1.0f, 1.0f);        // 領域の幅
	Vector3 mRegionMin = Vector3(0.0f, 0.0f, 0.0f);    // 領域の最小値
	Vector3 mRegionMax = Vector3(1.0f, 1.0f, 1.0f);    // 領域の最大値
	Vector3 mUnit = Vector3(1.0f, 1.0f, 1.0f);         // 最小領域の辺の長さ
	unsigned int mCellNumber = 0;                      // 空間の数
	unsigned int mMinimumLevel = 0;                    // 最下位レベル

public:
	LinearQuadTree()
	{
	}

	virtual ~LinearQuadTree()
	{
	}


	bool Init(unsigned int level, const Vector3& min, const Vector3& max)
	{
		// 最大レベル以上の空間は作らない
		if (level >= MAX_LEVEL) return false;

		// 各レベルでの空間数を算出
		mPowArray.at(0) = 1;
		for (auto i = 1; i < MAX_LEVEL + 1; ++i)
		{
			mPowArray.at(i) = mPowArray.at(i - 1) * 8;
		}

		// Levelの配列作成
		mCellNumber = (mPowArray.at(level + 1) - 1) / 7;
		//for (auto j = 0; j < mCellNumber; ++j)
		//{
		//	m_pCellmap.emplace_back(new Cell<T>());
		//}
		//ZeroMemory(ppCellArray, sizeof(Cell<T>*) * mCellNumber);

		// 領域の登録
		mRegionMin = min;
		mRegionMax = max;
		mWidth = mRegionMax - mRegionMin;
		mUnit = mWidth / static_cast<float>(1 << level);

		mMinimumLevel = level;

		return true;
	}

	bool Regist(Vector3* min, Vector3* max, const std::shared_ptr<ObjectForTree<T>>& obj)
	{
		// オブジェクトの境界範囲から登録モートン番号を算出
		unsigned long element = GetMortonNumber(min, max);
		if (element < mCellNumber)
		{
			// 空間がない場合は新規作成
			auto it = m_pCellmap.find(element);
			if (it == m_pCellmap.end())
			{
				CreateNewCell(element);
				std::shared_ptr<Cell<T>> cell = m_pCellmap.at(element);
			}
			return m_pCellmap.at(element)->Push(obj);

		}
		return false; // 登録失敗
	}

	unsigned int GetAllCollisionList(
		std::vector<std::shared_ptr<T>>& colList)
	{
		// リスト初期化
		colList.clear();

		// ルート空間の存在をチェック
		if (m_pCellmap[0] == NULL) return 0; // 空間が存在していない

		// ルート空間を処理
		std::list<std::shared_ptr<T>> colStac;
		GetCollisionList(0, colList, colStac);

		return static_cast<unsigned int>(colList.size());
	}

protected:

	// 空間内で衝突リストを作成する
	bool GetCollisionList(
		unsigned int element,
		std::vector<std::shared_ptr<T>>& colList,
		std::list<std::shared_ptr<T>>& colStac)
	{
		// 空間内のオブジェクト動詞の衝突リスト作成	
		std::weak_ptr<ObjectForTree<T>> pObject1 = m_pCellmap.at(element)->GetFirstObj();


		while (pObject1.lock() != nullptr)
		{
			std::weak_ptr<ObjectForTree<T>> pObject2 = pObject1.lock()->m_pNextObject;

			while (pObject2.lock() != nullptr)
			{
				// 衝突リスト作成
				colList.push_back(pObject1.lock()->m_pObject);
				colList.push_back(pObject2.lock()->m_pObject);
				pObject2 = pObject2.lock()->m_pNextObject;
			}

			// 衝突スタックとの衝突リスト作成
			for (auto it = colStac.begin(); it != colStac.end(); ++it)
			{
				colList.push_back(pObject1.lock()->m_pObject);
				colList.push_back(*it);
			}
			pObject1 = pObject1.lock()->m_pNextObject;
		}

		bool childFlag = false;

		// 子空間に移動
		unsigned int objNumber = 0;
		unsigned int nextElement;

		for (auto i = 0; i < 8; ++i)
		{
			nextElement = element * 8 + 1 + i;
			if (nextElement < mCellNumber &&
				(m_pCellmap.find(element * 8 + 1 + i) != m_pCellmap.end())
				)
			{
				if (!childFlag)
				{
					// 登録オブジェクトをスタックに追加
					pObject1 = m_pCellmap.at(element)->GetFirstObj();

					while (pObject1.lock() != nullptr)
					{
						colStac.push_back(pObject1.lock()->m_pObject);
						objNumber++;
						pObject1 = pObject1.lock()->m_pNextObject;
					}
				}

				childFlag = true;

				GetCollisionList(element * 8 + 1 + i, colList, colStac); // 子空間へ
			}
		}

		// スタックからオブジェクトを外す
		if (childFlag)
		{
			for (auto i = 0u; i < objNumber; ++i)
			{
				colStac.pop_back();
			}
		}
		return true;
	}

	// 空間を生成
	bool CreateNewCell(unsigned int element)
	{
		// 引数の要素番号
		while (m_pCellmap.find(element) == m_pCellmap.end())
		{
			// 指定の要素番号に空間を新規作成
			std::shared_ptr<Cell<T>> cell;
			m_pCellmap.emplace(element, cell->Initialize());

			// 親空間にジャンプ
			element = (element - 1) >> 3;
			if (element >= mCellNumber) break;
		}

		return true;
	}

	// 座標から空間番号を算出
	unsigned long GetMortonNumber(Vector3* min, Vector3* max)
	{
		// 最小レベルにおける各軸位置を算出
		unsigned long LeftTop = GetPointElement(*min);
		unsigned long RightBottom = GetPointElement(*max);

		// 空間番号を引き算して
		// 最上位区切りから所属レベルを算出
		unsigned long def = RightBottom ^ LeftTop;
		unsigned long hiLevel = 1;

		for (auto i = 0u; i < mMinimumLevel; ++i)
		{
			unsigned long check = (def >> (i * 3)) & 0x7;
			if (check != 0)
			{
				hiLevel = i + 1;
			}
		}

		unsigned long spaceNum = RightBottom >> (hiLevel * 3);
		unsigned long addNum = (mPowArray.at(mMinimumLevel - hiLevel) - 1) / 7;
		spaceNum += addNum;

		if (spaceNum > mCellNumber) return 0xffffffff;

		return spaceNum;
	}

	// ビット分割関数
	unsigned int BitSeparateFor3D(std::byte n)
	{
		unsigned int s = static_cast<unsigned int>(n);
		s = (s | s << 8) & 0x0000f00f;
		s = (s | s << 4) & 0x000c30c3;
		s = (s | s << 2) & 0x00249249;
		return s;
	}

	// 3Dモートン空間番号算出関数
	unsigned int Get3DMortonNumber(std::byte x, std::byte y, std::byte z)
	{
		return (BitSeparateFor3D(x) | BitSeparateFor3D(y) << 1 | BitSeparateFor3D(z) << 2);
	}

	// 座標->線形8分木要素番号変換関数
	unsigned int GetPointElement(const Vector3& p)
	{
		return Get3DMortonNumber(
			static_cast<std::byte>((p.x - mRegionMin.x) / (mUnit.x)),
			static_cast<std::byte>((p.y - mRegionMin.y) / (mUnit.y)),
			static_cast<std::byte>((p.z - mRegionMin.z) / (mUnit.z))
		);
	}
};


