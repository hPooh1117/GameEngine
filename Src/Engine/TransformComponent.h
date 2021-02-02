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
�Q�[���G���W���ō̗p����� SceneGraph�B����̓��[���h���̂��ׂẴI�u�W�F�N�g���܂ރr�b�O�f�[�^�\���ł���B
�`��G���W���ł� SceneGraph ���A�X�N���[����ɕ`�悷����̂����肷��ۂɗ��p�����B

�ESceneGraph�ň�����I�u�W�F�N�g�ɂ��āA
�@	�[���̂̓I�u�W�F�N�g�̃t���b�g�ȃ��X�g�ł���A
�@	�[���ꂼ��̃I�u�W�F�N�g�̓��f�����􉽊w�I�v���~�e�B�u�A������ Transform �����B
�@	�[Transform �� position, rotation, scale �̏����܂�ł���B



���ɐl�Ԃ����������A���Ƀ`���[�������Ă���ꍇ�A
�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
|													  |
|   Human <-- Sword <-- Charm                         |
| (Parent)   (Child)   (GrandChild)                   |
|													  |
�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
 �Ƃ����\�}�ɂȂ�ASword �̍��W�� Human �̍��W�Ɉˑ�����B
 Charm �͂���� Sword, Human �̑o���Ɉˑ�����B


 ���̂Ƃ� Charm �̃��[���h���W��
 �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
 |													  |
 | CharmWorld = HumanLocal * SwordLocal * CharmLocal  |
 |													  |
 �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
 �ŕ\����B

 �E�ł��P���Ƀ��[���h���W�𓾂�ɂ́A�e�t���[���� SceneGraph �̃q�G�����L�[�̃g�b�v����ċA�I�Ɍv�Z���Ă������Ƃł���B
 �E������𖈃t���[���v�Z�����CPU�v�Z�������Ȃ�A���ʂ������B

 �E�����I�ȕ��@�� World Transform ���L���b�V�������邱�Ƃł���B
    Local Transform �� Local Transform �ɗR������ World Transform �����ꂼ��ێ�����B
    �����_�����O���A���O�v�Z���ꂽ World Transform �݂̂𗘗p����B
    ---�I�u�W�F�N�g�������Ȃ��ꍇ�A���� World Transform �͏�ɍX�V�����Ɏg����B
    ---�I�u�W�F�N�g�������ꍇ�AWorld Transform���Čv�Z����B�i���̂Ƃ��A���ׂĂ̎q���ċA�I�Ɍv�Z���邱�Ƃɒ��Ӂj
    =============================================================|
    |															 |
    | Move Human -> Recalc Human -> Recalc Sword -> Recalc Charm | 
    | Move Sword -> Recalc Sword -> Recalc Charm				 |
    | Move Charm -> Recalc Charm								 |
    |															 |
    =============================================================|
    �܂�A�q�G�����L�[�̒�ʂɂ���I�u�W�F�N�g�������ꍇ�A�܂��v�Z�̖��ʂ�������B

 �E��L�̖����������邽�߂ɁADeffered recalculation ���s���B
	Deffered recalc �ɂ��ALocal Transform �̂܂Ƃ܂��1�x�ŕς��AWorld Transform ���Čv�Z�ł���B����̓����_�����O�̒��O�ɍs���B
	Local Transform ���ω�������AFlag ���Z�b�g���A�Z�b�g���ꂽ���̂� World Transform ���v�Z���AFlag ���N���A����B
	����āAFlag���������̂� "World Transform �͍ŐV�̂��̂��ǂ���" �Ƃ������ƂɂȂ�B���� Flag ��ʏ� "out-of-date-ness" "dirty" �ƌĂԁB
	=============================================================|
	|															 |
	| Move Human												 |
	| Move Sword												 |
	| Move Charm												 |
	|															 |
	|�@Render -> Recalc Human -> Recalc Sword -> Recalc Charm	 |
	|															 |
	=============================================================|

	�܂�ADeffered recalculation�̉��b�͈ȉ��B
	�[���t���[�� SceneGraph �̂��ׂẴI�u�W�F�N�g���ċA�I�Ɍv�Z���Ȃ��Ă悢�B
	�[�����Ȃ��I�u�W�F�N�g�̌v�Z�͕K�v�Ȃ��B
	�[�����_�����O�̑O�ɂ̃I�u�W�F�N�g�̍X�V�ŃI�u�W�F�N�g���폜���ꂽ�Ƃ��AWorld Transform �̌v�Z�͂���Ȃ��Ȃ�B
 */