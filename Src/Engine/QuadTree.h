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

	// �����N��S�ă��Z�b�g����
	void ResetLink(std::weak_ptr<ObjectForTree<T>>& obj)
	{
		if (!obj.lock()->m_pNextObject.expired())
			ResetLink(obj.lock()->m_pNextObject);

		obj.reset(); // ���Z�b�g
	}

	// ObjectForTree���v�b�V��
	bool Push(const std::shared_ptr<ObjectForTree<T>>& obj)
	{
		if (obj == nullptr) return false; // �����I�u�W�F�N�g�͓o�^���Ȃ�
		if (obj->m_pCell == this->shared_from_this()) return false; // ��d�o�^�`�F�b�N

		if (m_pLatest.expired()) m_pLatest = obj; // ��ԂɐV�K�o�^
		else
		{
			// �ŐVObjectForTree�I�u�W�F�N�g���X�V
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
	std::unordered_map<unsigned int, std::shared_ptr<Cell<T>>> m_pCellmap;  // ���`��ԃ|�C���^�z��
	std::array<unsigned int, MAX_LEVEL + 1> mPowArray;           // �ׂ��搔�l�z��

	Vector3 mWidth = Vector3(1.0f, 1.0f, 1.0f);        // �̈�̕�
	Vector3 mRegionMin = Vector3(0.0f, 0.0f, 0.0f);    // �̈�̍ŏ��l
	Vector3 mRegionMax = Vector3(1.0f, 1.0f, 1.0f);    // �̈�̍ő�l
	Vector3 mUnit = Vector3(1.0f, 1.0f, 1.0f);         // �ŏ��̈�̕ӂ̒���
	unsigned int mCellNumber = 0;                      // ��Ԃ̐�
	unsigned int mMinimumLevel = 0;                    // �ŉ��ʃ��x��

public:
	LinearQuadTree()
	{
	}

	virtual ~LinearQuadTree()
	{
	}


	bool Init(unsigned int level, const Vector3& min, const Vector3& max)
	{
		// �ő僌�x���ȏ�̋�Ԃ͍��Ȃ�
		if (level >= MAX_LEVEL) return false;

		// �e���x���ł̋�Ԑ����Z�o
		mPowArray.at(0) = 1;
		for (auto i = 1; i < MAX_LEVEL + 1; ++i)
		{
			mPowArray.at(i) = mPowArray.at(i - 1) * 8;
		}

		// Level�̔z��쐬
		mCellNumber = (mPowArray.at(level + 1) - 1) / 7;
		//for (auto j = 0; j < mCellNumber; ++j)
		//{
		//	m_pCellmap.emplace_back(new Cell<T>());
		//}
		//ZeroMemory(ppCellArray, sizeof(Cell<T>*) * mCellNumber);

		// �̈�̓o�^
		mRegionMin = min;
		mRegionMax = max;
		mWidth = mRegionMax - mRegionMin;
		mUnit = mWidth / static_cast<float>(1 << level);

		mMinimumLevel = level;

		return true;
	}

	bool Regist(Vector3* min, Vector3* max, const std::shared_ptr<ObjectForTree<T>>& obj)
	{
		// �I�u�W�F�N�g�̋��E�͈͂���o�^���[�g���ԍ����Z�o
		unsigned long element = GetMortonNumber(min, max);
		if (element < mCellNumber)
		{
			// ��Ԃ��Ȃ��ꍇ�͐V�K�쐬
			auto it = m_pCellmap.find(element);
			if (it == m_pCellmap.end())
			{
				CreateNewCell(element);
				std::shared_ptr<Cell<T>> cell = m_pCellmap.at(element);
			}
			return m_pCellmap.at(element)->Push(obj);

		}
		return false; // �o�^���s
	}

	unsigned int GetAllCollisionList(
		std::vector<std::shared_ptr<T>>& colList)
	{
		// ���X�g������
		colList.clear();

		// ���[�g��Ԃ̑��݂��`�F�b�N
		if (m_pCellmap[0] == NULL) return 0; // ��Ԃ����݂��Ă��Ȃ�

		// ���[�g��Ԃ�����
		std::list<std::shared_ptr<T>> colStac;
		GetCollisionList(0, colList, colStac);

		return static_cast<unsigned int>(colList.size());
	}

protected:

	// ��ԓ��ŏՓ˃��X�g���쐬����
	bool GetCollisionList(
		unsigned int element,
		std::vector<std::shared_ptr<T>>& colList,
		std::list<std::shared_ptr<T>>& colStac)
	{
		// ��ԓ��̃I�u�W�F�N�g�����̏Փ˃��X�g�쐬	
		std::weak_ptr<ObjectForTree<T>> pObject1 = m_pCellmap.at(element)->GetFirstObj();


		while (pObject1.lock() != nullptr)
		{
			std::weak_ptr<ObjectForTree<T>> pObject2 = pObject1.lock()->m_pNextObject;

			while (pObject2.lock() != nullptr)
			{
				// �Փ˃��X�g�쐬
				colList.push_back(pObject1.lock()->m_pObject);
				colList.push_back(pObject2.lock()->m_pObject);
				pObject2 = pObject2.lock()->m_pNextObject;
			}

			// �Փ˃X�^�b�N�Ƃ̏Փ˃��X�g�쐬
			for (auto it = colStac.begin(); it != colStac.end(); ++it)
			{
				colList.push_back(pObject1.lock()->m_pObject);
				colList.push_back(*it);
			}
			pObject1 = pObject1.lock()->m_pNextObject;
		}

		bool childFlag = false;

		// �q��ԂɈړ�
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
					// �o�^�I�u�W�F�N�g���X�^�b�N�ɒǉ�
					pObject1 = m_pCellmap.at(element)->GetFirstObj();

					while (pObject1.lock() != nullptr)
					{
						colStac.push_back(pObject1.lock()->m_pObject);
						objNumber++;
						pObject1 = pObject1.lock()->m_pNextObject;
					}
				}

				childFlag = true;

				GetCollisionList(element * 8 + 1 + i, colList, colStac); // �q��Ԃ�
			}
		}

		// �X�^�b�N����I�u�W�F�N�g���O��
		if (childFlag)
		{
			for (auto i = 0u; i < objNumber; ++i)
			{
				colStac.pop_back();
			}
		}
		return true;
	}

	// ��Ԃ𐶐�
	bool CreateNewCell(unsigned int element)
	{
		// �����̗v�f�ԍ�
		while (m_pCellmap.find(element) == m_pCellmap.end())
		{
			// �w��̗v�f�ԍ��ɋ�Ԃ�V�K�쐬
			std::shared_ptr<Cell<T>> cell;
			m_pCellmap.emplace(element, cell->Initialize());

			// �e��ԂɃW�����v
			element = (element - 1) >> 3;
			if (element >= mCellNumber) break;
		}

		return true;
	}

	// ���W�����Ԕԍ����Z�o
	unsigned long GetMortonNumber(Vector3* min, Vector3* max)
	{
		// �ŏ����x���ɂ�����e���ʒu���Z�o
		unsigned long LeftTop = GetPointElement(*min);
		unsigned long RightBottom = GetPointElement(*max);

		// ��Ԕԍ��������Z����
		// �ŏ�ʋ�؂肩�珊�����x�����Z�o
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

	// �r�b�g�����֐�
	unsigned int BitSeparateFor3D(std::byte n)
	{
		unsigned int s = static_cast<unsigned int>(n);
		s = (s | s << 8) & 0x0000f00f;
		s = (s | s << 4) & 0x000c30c3;
		s = (s | s << 2) & 0x00249249;
		return s;
	}

	// 3D���[�g����Ԕԍ��Z�o�֐�
	unsigned int Get3DMortonNumber(std::byte x, std::byte y, std::byte z)
	{
		return (BitSeparateFor3D(x) | BitSeparateFor3D(y) << 1 | BitSeparateFor3D(z) << 2);
	}

	// ���W->���`8���ؗv�f�ԍ��ϊ��֐�
	unsigned int GetPointElement(const Vector3& p)
	{
		return Get3DMortonNumber(
			static_cast<std::byte>((p.x - mRegionMin.x) / (mUnit.x)),
			static_cast<std::byte>((p.y - mRegionMin.y) / (mUnit.y)),
			static_cast<std::byte>((p.z - mRegionMin.z) / (mUnit.z))
		);
	}
};


