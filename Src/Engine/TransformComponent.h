#pragma once
#include "./Utilities/CommonInclude.h"
#include "EntityComponentSystem.h"
#include <string>

namespace ECS
{
	struct TransformComponent
	{
		// What is DirtyFlag??? *wrote down it below*
		// https://gameprogrammingpatterns.com/dirty-flag.html
		enum Flags
		{
			EEmpty = 0,
			EDirty = 1 << 0,
		};

		uint32_t flags = EDirty;

		XMFLOAT3 scaleLocal = XMFLOAT3(1, 1, 1);
		XMFLOAT4 rotationLocal = XMFLOAT4(0, 0, 0, 1);
		XMFLOAT3 translationLocal = XMFLOAT3(0, 0, 0);

		XMFLOAT4X4 world = IDENTITYMATRIX;

		inline void SetDirty(bool value = true) {
			if (value) { flags |= EDirty; }
			else { flags |= ~EDirty; }
		}
		inline bool IsDirty() const { return flags & EDirty; }

		XMFLOAT3 GetPosition() const;
		XMFLOAT4 GetRotation() const;
		XMFLOAT3 GetScale() const;
		XMVECTOR GetPositionVector() const;
		XMVECTOR GetRotationVector() const;
		XMVECTOR GetScaleVector() const;
		XMMATRIX GetLocalMatrix() const;

		void UpdateTransform();
		void UpdateTransform_Parented(const TransformComponent& parent);
		void ApplyTransform();
		void ClearTransform();
		void Translate(const XMFLOAT3& value);
		void Translate(const XMVECTOR& value);
		void RotateRollPitchYaw(const XMFLOAT3& value);
		void Rotate(const XMFLOAT4& quaternion);
		void Rotate(const XMVECTOR& quaternion);
		void Scale(const XMFLOAT3& value);
		void Scale(const XMVECTOR& value);
		void MatrixTransform(const XMFLOAT4X4& matrix);
		void MatrixTransform(const XMMATRIX& matrix);
		void Lerp(const TransformComponent& a, const TransformComponent& b, float t);
		// http://naochang.me/?p=1119
		void CatmullRom(const TransformComponent& a, const TransformComponent& b, const TransformComponent& c, const TransformComponent& d, float t);

		void Serialize(Archive& archive, ECS::EntitySerializer& seri);
	};
}

/*
ゲームエンジンで採用される SceneGraph。これはワールド内のすべてのオブジェクトを含むビッグデータ構造である。
描画エンジンでは SceneGraph が、スクリーン上に描画するものを決定する際に利用される。

・SceneGraphで扱われるオブジェクトについて、
　	ー実体はオブジェクトのフラットなリストである、
　	ーそれぞれのオブジェクトはモデルか幾何学的プリミティブ、そして Transform を持つ。
　	ーTransform は position, rotation, scale の情報を含んでいる。



仮に人間が剣を持ち、剣にチャームがついている場合、
ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
|													  |
|   Human <-- Sword <-- Charm                         |
| (Parent)   (Child)   (GrandChild)                   |
|													  |
ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
 という構図になり、Sword の座標は Human の座標に依存する。
 Charm はさらに Sword, Human の双方に依存する。


 このとき Charm のワールド座標は
 ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
 |													  |
 | CharmWorld = HumanLocal * SwordLocal * CharmLocal  |
 |													  |
 ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
 で表せる。

 ・最も単純にワールド座標を得るには、各フレームで SceneGraph のヒエラルキーのトップから再帰的に計算していくことである。
 ・↑これを毎フレーム計算するとCPU計算が多くなり、無駄が多い。

 ・現実的な方法は World Transform をキャッシュ化することである。
    Local Transform と Local Transform に由来する World Transform をそれぞれ保持する。
    レンダリング時、事前計算された World Transform のみを利用する。
    ---オブジェクトが動かない場合、その World Transform は常に更新せずに使える。
    ---オブジェクトが動く場合、World Transformを再計算する。（このとき、すべての子を再帰的に計算することに注意）
    =============================================================|
    |															 |
    | Move Human -> Recalc Human -> Recalc Sword -> Recalc Charm | 
    | Move Sword -> Recalc Sword -> Recalc Charm				 |
    | Move Charm -> Recalc Charm								 |
    |															 |
    =============================================================|
    つまり、ヒエラルキーの低位にいるオブジェクトが動く場合、また計算の無駄が生じる。

 ・上記の問題を解決するために、Deffered recalculation を行う。
	Deffered recalc により、Local Transform のまとまりを1度で変え、World Transform を再計算できる。これはレンダリングの直前に行う。
	Local Transform が変化したら、Flag をセットし、セットされたものの World Transform を計算し、Flag をクリアする。
	よって、Flagが示すものは "World Transform は最新のものかどうか" ということになる。この Flag を通常 "out-of-date-ness" "dirty" と呼ぶ。
	=============================================================|
	|															 |
	| Move Human												 |
	| Move Sword												 |
	| Move Charm												 |
	|															 |
	|　Render -> Recalc Human -> Recalc Sword -> Recalc Charm	 |
	|															 |
	=============================================================|

	つまり、Deffered recalculationの恩恵は以下。
	ー毎フレーム SceneGraph のすべてのオブジェクトを再帰的に計算しなくてよい。
	ー動かないオブジェクトの計算は必要ない。
	ーレンダリングの前にのオブジェクトの更新でオブジェクトが削除されたとき、World Transform の計算はされなくなる。
 */