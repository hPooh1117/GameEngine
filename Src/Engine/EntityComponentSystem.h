#pragma once
#include "./Application/JobSystem.h"
#include "./Utilities/CommonInclude.h"
#include "./Utilities/Archive.h"
#include <atomic>
#include <unordered_map>
#include <vector>

// https://wickedengine.net/2019/09/29/entity-component-system/

namespace ECS
{
	using Entity = uint32_t;
	static const Entity INVALID_ENTITY = 0;

	inline Entity CreateEntity()
	{
		static std::atomic<Entity> next{ INVALID_ENTITY + 1 };
		return next.fetch_add(1);
	}

	struct EntitySerializer
	{
		JobSystem::Context ctx;
		std::unordered_map<uint64_t, Entity> remap;
		bool allow_remap = true;

		~EntitySerializer()
		{
			JobSystem::Wait(ctx);
		}
	};


	// �X���b�h�Z�[�t��	Entity���V���A���C�Y
	inline void SerializeEntity(Archive& archive, Entity& entity, EntitySerializer& seri)
	{
		if (archive.IsReadMode())
		{
			// Entity �� uint64_t �̃T�C�Y�ŃV���A���C�Y����Ă���
			uint64_t mem;
			archive >> mem;

			if (seri.allow_remap)
			{
				auto it = seri.remap.find(mem);
				if (it == seri.remap.end())
				{
					entity = CreateEntity();
					seri.remap[mem] = entity;
				}
				else
				{
					entity = it->second;
				}
			}
			else
			{
				entity = (Entity)mem;
			}
		}
		else
		{
			archive << entity;
		}
	}

	template<typename Component>
	class ComponentManager
	{
	private:
		std::vector<Component> mComponents;
		std::vector<Entity> mEntities;
		std::unordered_map<Entity, size_t> mLookUp;

		ComponentManager(const ComponentManager&) = delete;

	public:
		/// reserved_count : ���������ɂ�����Component������
		ComponentManager(size_t reserved_count = 0)
		{
			mComponents.reserve(reserved_count);
			mEntities.reserve(reserved_count);
			mLookUp.reserve(reserved_count);
		}

		// �S����
		inline void Clear()
		{
			mComponents.clear();
			mEntities.clear();
			mLookUp.clear();
		}

		// ���ׂĂ̗v�f�𑼂Ƀf�B�[�v�R�s�[����
		inline void Copy(const ComponentManager<Component>& other)
		{
			Clear();
			mComponents = other.mComponents;
			mEntities = other.mEntities;
			mLookUp = other.mLookUp;
		}

		// �����Ɠ����^�C�v�̑���ComponentManager�ƃ}�[�W
		// ���@�F�@����ComponentManager�͓���Entity���܂�ł��Ă͂����Ȃ�
		// ���@�F�@����ComponentManager�͂��̏����̌�Ɉ������Ƃ��ł��Ȃ��Ȃ�
		inline void Merge(ComponentManager<Component>& other)
		{
			mComponents.reserve(GetCount() + other.GetCount());
			mEntities.reserve(GetCount() + other.GetCount());
			mLookUp.reserve(GetCount() + other.GetCount());

			for (size_t i = 0; i < other.GetCount(); ++i)
			{
				Entity entity = other.mEntities[i];
				assert(!Contains(entity));
				mEntities.push_back(entity);
				mLookUp[entity] = mComponents.size();
				mComponents.push_back(std::move(other.mComponents[i]));
			}
			other.Clear();
		}

		// Read/Write everything to an archive depending on the archive state  �A�[�J�C�u�ɓǂݎ�� / �������݂����s
		inline void Serialize(Archive& archive, EntitySerializer& seri)
		{
			// �ǂݎ�菈��
			if (archive.IsReadMode())
			{
				Clear();

				size_t count;
				archive >> count;

				mComponents.resize(count);
				for (size_t i = 0; i < count; ++i)
				{
					mComponents[i].Serialize(archive, seri);
				}

				mEntities.resize(count);
				for (size_t i = 0; i < count; ++i)
				{
					Entity entity;
					SerializeEntity(archive, entity, seri);
					mEntities[i] = entity;
					mLookUp[entity] = i;
				}
			}
			// �������ݏ���
			else
			{
				archive << mComponents.size();
				for (Component& component : mComponents)
				{
					component.Serialize(archive, seri);
				}
				for (Entity entity : mEntities)
				{
					SerializeEntity(archive, entity, seri);
				}
			}
		}

		// �V����Component���쐬���A�擾
		inline Component& Create(Entity entity)
		{
			// INVALID_ENTITY�͖���
			assert(entity != INVALID_ENTITY);

			// Entity�ɓ���Component��o�^���邱�Ƃ͂ł��Ȃ�
			assert(mLookUp.find(entity) == mLookUp.end());

			// Entity�̐���Component�̐��Ɠ���
			assert(mEntities.size() == mComponents.size());
			assert(mLookUp.size() == mComponents.size());

			// Entity LookUp�e�[�u�����X�V
			mLookUp[entity] = mComponents.size();

			// �V����Component�͖����Ƀv�b�V��
			mComponents.emplace_back();

			// �Ή�����Entity���v�b�V��
			mEntities.push_back(entity);

			return mComponents.back();
		}

		// Entity���w���Component�������Ă�����A����
		inline void Remove(Entity entity)
		{
			auto it = mLookUp.find(entity);
			if (it != mLookUp.end())
			{
				// Component��Entity�̃C���f�b�N�X
				const size_t index = it->second;
				const Entity entity = mEntities[index];

				if (index < mComponents.size() - 1)
				{
					// �w�肳�ꂽ�v�f�Ɩ����̗v�f���X���b�v
					mComponents[index] = std::move(mComponents.back()); // �R�s�[�łȂ����[�u
					mEntities[index] = mEntities.back();

					// LookUp�e�[�u�����X�V
					mLookUp[mEntities[index]] = index;
				}

				// �R���e�i�����k
				mComponents.pop_back();
				mEntities.pop_back();
				mLookUp.erase(entity);
			}
		}


		// Entity�Ɏw�肳�ꂽComponent�����݂���Ȃ�������A�\�[�g����
		//
		inline void RemoveAndKeepSorted(Entity entity)
		{
			auto it = mLookUp.find(entity);
			if (it != mLookUp.end())
			{
				// Component��Entity�̒��ڎQ�Ɨp�C���f�b�N�X���擾
				const size_t index = it->second;
				const Entity entity = mEntities[index];

				if (index < mComponents.size() - 1)
				{
					// �w�肳�ꂽ�C���f�b�N�X�ȍ~��Component�����ׂĂ��炷
					for (size_t i = index + 1; i < mComponents.size(); ++i)
					{
						mComponents[i - 1] = std::move(mComponents[i]);
					}
					for (size_t i = index + 1; i < mEntities.size(); ++i)
					{
						mEntities[i - 1] = mEntities[i];
						mLookUp[mEntities[i - 1]] = i - 1;
					}
				}

				// �R���e�i�̈��k
				mComponents.pop_back();
				mEntities.pop_back();
				mLookUp.erase(entity);
			}
		}

		// Entity-Component�����̃C���f�b�N�X���w�肵�Ĉړ�������
		inline void MoveItem(size_t index_from, size_t index_to)
		{
			assert(index_from < GetCount());
			assert(index_to < GetCount());
			if (index_from == index_to)
			{
				return;
			}

			// �ړ��Ώۂ�Component-Entity���Z�[�u
			Component component = std::move(mComponents[index_from]);
			Entity entity = mEntities[index_from];

			// �����ׂĂ�Entity-Component�����炷
			const int direction = index_from < index_to ? 1 : -1;
			for (size_t i = index_from; i != index_to; i += direction)
			{
				const size_t next = i + direction;
				mComponents[i] = std::move(mComponents[next]);
				mEntities[i] = mEntities[next];
				mLookUp[mEntities[i]] = i;
			}

			// �Ώ�Entity-Component�̈ړ����s
			mComponents[index_to] = std::move(component);
			mEntities[index_to] = entity;
			mLookUp[entity] = index_to;
		}


		// �񎦂��ꂽEntity��Component�������Ă��邩
		inline bool Contains(Entity entity) const
		{
			return mLookUp.find(entity) != mLookUp.end();
		}

		// �����Component���擾�i�ǂݎ�� / �������݁j
		inline Component* GetComponent(Entity entity)
		{
			auto it = mLookUp.find(entity);
			if (it != mLookUp.end())
			{
				return &mComponents[it->second];
			}
			return nullptr;
		}

		// �����Component���擾�i�ǂݎ���p�j
		inline const Component* GetComponent(Entity entity) const
		{
			const auto it = mLookUp.find(entity);
			if (it != mLookUp.end())
			{
				return &mComponents[it->second];
			}
			return nullptr;
		}

		// Entity��ID����Component�̃C���f�b�N�X���擾
		inline size_t GetIndex(Entity entity) const
		{
			const auto it = mLookUp.find(entity);
			if (it != mLookUp.end())
			{
				return it->second;
			}
			return ~0; // -1
		}

		// Component�̐����擾
		inline size_t GetCount() const { return mComponents.size(); }

		// �C���f�b�N�X�ɂ��Entity�̒��ڎQ��
		//	0 <= index < GetCount()
		inline Entity GetEntity(size_t index) const { return mEntities[index]; }

		// �C���f�b�N�X�ɂ��Component�̒��ڎQ��(�ǂݎ�� / ��������)
		//	0 <= index < GetCount()
		inline Component& operator[](size_t index) { return mComponents[index]; }

		// �C���f�b�N�X�ɂ��Component�̒��ڎQ��(�ǂݎ���p)
		//	0 <= index < GetCount()
		inline const Component& operator[](size_t index) const { return mComponents[index]; }

	};
}