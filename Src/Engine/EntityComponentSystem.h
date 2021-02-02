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


	// スレッドセーフで	Entityをシリアライズ
	inline void SerializeEntity(Archive& archive, Entity& entity, EntitySerializer& seri)
	{
		if (archive.IsReadMode())
		{
			// Entity は uint64_t のサイズでシリアライズされている
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
		/// reserved_count : 初期化時にいくつComponentを持つか
		ComponentManager(size_t reserved_count = 0)
		{
			mComponents.reserve(reserved_count);
			mEntities.reserve(reserved_count);
			mLookUp.reserve(reserved_count);
		}

		// 全消去
		inline void Clear()
		{
			mComponents.clear();
			mEntities.clear();
			mLookUp.clear();
		}

		// すべての要素を他にディープコピーする
		inline void Copy(const ComponentManager<Component>& other)
		{
			Clear();
			mComponents = other.mComponents;
			mEntities = other.mEntities;
			mLookUp = other.mLookUp;
		}

		// 自分と同じタイプの他のComponentManagerとマージ
		// 注　：　他のComponentManagerは同じEntityを含んでいてはいけない
		// 注　：　他のComponentManagerはこの処理の後に扱うことができなくなる
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

		// Read/Write everything to an archive depending on the archive state  アーカイブに読み取り / 書き込みを実行
		inline void Serialize(Archive& archive, EntitySerializer& seri)
		{
			// 読み取り処理
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
			// 書き込み処理
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

		// 新しいComponentを作成し、取得
		inline Component& Create(Entity entity)
		{
			// INVALID_ENTITYは無効
			assert(entity != INVALID_ENTITY);

			// Entityに同じComponentを登録することはできない
			assert(mLookUp.find(entity) == mLookUp.end());

			// Entityの数はComponentの数と同じ
			assert(mEntities.size() == mComponents.size());
			assert(mLookUp.size() == mComponents.size());

			// Entity LookUpテーブルを更新
			mLookUp[entity] = mComponents.size();

			// 新しいComponentは末尾にプッシュ
			mComponents.emplace_back();

			// 対応したEntityもプッシュ
			mEntities.push_back(entity);

			return mComponents.back();
		}

		// Entityが指定のComponentを持っていたら、除去
		inline void Remove(Entity entity)
		{
			auto it = mLookUp.find(entity);
			if (it != mLookUp.end())
			{
				// ComponentとEntityのインデックス
				const size_t index = it->second;
				const Entity entity = mEntities[index];

				if (index < mComponents.size() - 1)
				{
					// 指定された要素と末尾の要素をスワップ
					mComponents[index] = std::move(mComponents.back()); // コピーでなくムーブ
					mEntities[index] = mEntities.back();

					// LookUpテーブルを更新
					mLookUp[mEntities[index]] = index;
				}

				// コンテナを圧縮
				mComponents.pop_back();
				mEntities.pop_back();
				mLookUp.erase(entity);
			}
		}


		// Entityに指定されたComponentが存在するなら消去し、ソートする
		//
		inline void RemoveAndKeepSorted(Entity entity)
		{
			auto it = mLookUp.find(entity);
			if (it != mLookUp.end())
			{
				// ComponentとEntityの直接参照用インデックスを取得
				const size_t index = it->second;
				const Entity entity = mEntities[index];

				if (index < mComponents.size() - 1)
				{
					// 指定されたインデックス以降のComponentをすべてずらす
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

				// コンテナの圧縮
				mComponents.pop_back();
				mEntities.pop_back();
				mLookUp.erase(entity);
			}
		}

		// Entity-Componentを特定のインデックスを指定して移動させる
		inline void MoveItem(size_t index_from, size_t index_to)
		{
			assert(index_from < GetCount());
			assert(index_to < GetCount());
			if (index_from == index_to)
			{
				return;
			}

			// 移動対象のComponent-Entityをセーブ
			Component component = std::move(mComponents[index_from]);
			Entity entity = mEntities[index_from];

			// 他すべてのEntity-Componentをずらす
			const int direction = index_from < index_to ? 1 : -1;
			for (size_t i = index_from; i != index_to; i += direction)
			{
				const size_t next = i + direction;
				mComponents[i] = std::move(mComponents[next]);
				mEntities[i] = mEntities[next];
				mLookUp[mEntities[i]] = i;
			}

			// 対象Entity-Componentの移動実行
			mComponents[index_to] = std::move(component);
			mEntities[index_to] = entity;
			mLookUp[entity] = index_to;
		}


		// 提示されたEntityがComponentを持っているか
		inline bool Contains(Entity entity) const
		{
			return mLookUp.find(entity) != mLookUp.end();
		}

		// 特定のComponentを取得（読み取り / 書き込み）
		inline Component* GetComponent(Entity entity)
		{
			auto it = mLookUp.find(entity);
			if (it != mLookUp.end())
			{
				return &mComponents[it->second];
			}
			return nullptr;
		}

		// 特定のComponentを取得（読み取り専用）
		inline const Component* GetComponent(Entity entity) const
		{
			const auto it = mLookUp.find(entity);
			if (it != mLookUp.end())
			{
				return &mComponents[it->second];
			}
			return nullptr;
		}

		// EntityのIDからComponentのインデックスを取得
		inline size_t GetIndex(Entity entity) const
		{
			const auto it = mLookUp.find(entity);
			if (it != mLookUp.end())
			{
				return it->second;
			}
			return ~0; // -1
		}

		// Componentの数を取得
		inline size_t GetCount() const { return mComponents.size(); }

		// インデックスによるEntityの直接参照
		//	0 <= index < GetCount()
		inline Entity GetEntity(size_t index) const { return mEntities[index]; }

		// インデックスによるComponentの直接参照(読み取り / 書き込み)
		//	0 <= index < GetCount()
		inline Component& operator[](size_t index) { return mComponents[index]; }

		// インデックスによるComponentの直接参照(読み取り専用)
		//	0 <= index < GetCount()
		inline const Component& operator[](size_t index) const { return mComponents[index]; }

	};
}