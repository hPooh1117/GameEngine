#include "..\TransformComponent.h"

namespace ECS
{
	XMFLOAT3 TransformComponent::GetPosition() const
	{
		return *((XMFLOAT3*)&world._41);
	}

	XMFLOAT4 TransformComponent::GetRotation() const
	{
		XMFLOAT4 rotation;
		XMStoreFloat4(&rotation, GetRotationVector());
		return rotation;
	}

	XMFLOAT3 TransformComponent::GetScale() const
	{
		XMFLOAT3 scale;
		XMStoreFloat3(&scale, GetScaleVector());
		return scale;
	}

	XMVECTOR TransformComponent::GetPositionVector() const
	{
		return XMLoadFloat3((XMFLOAT3*)&world._41);
	}

	XMVECTOR TransformComponent::GetRotationVector() const
	{
		XMVECTOR S, R, T; 
		XMMatrixDecompose(&S, &R, &T, XMLoadFloat4x4(&world));
		return R;
	}

	XMVECTOR TransformComponent::GetScaleVector() const
	{
		XMVECTOR S, R, T;
		XMMatrixDecompose(&S, &R, &T, XMLoadFloat4x4(&world));
		return S;
	}

	XMMATRIX TransformComponent::GetLocalMatrix() const
	{
		XMVECTOR S_local = XMLoadFloat3(&scaleLocal);
		XMVECTOR R_local = XMLoadFloat4(&rotationLocal);
		XMVECTOR T_local = XMLoadFloat3(&translationLocal);
		return
			XMMatrixScalingFromVector(S_local) *
			XMMatrixRotationQuaternion(R_local) *
			XMMatrixTranslationFromVector(T_local);
	}

	void TransformComponent::UpdateTransform()
	{
		if (IsDirty())
		{
			SetDirty(false);

			XMStoreFloat4x4(&world, GetLocalMatrix());
		}
	}

	void TransformComponent::UpdateTransform_Parented(const TransformComponent& parent)
	{
		XMMATRIX W = GetLocalMatrix();
		XMMATRIX W_parent = XMLoadFloat4x4(&parent.world);
		W = W * W_parent;

		XMStoreFloat4x4(&world, W);
	}

	void TransformComponent::ApplyTransform()
	{
		SetDirty();

		XMVECTOR S, R, T;
		XMMatrixDecompose(&S, &R, &T, XMLoadFloat4x4(&world));
		XMStoreFloat3(&scaleLocal, S);
		XMStoreFloat4(&rotationLocal, R);
		XMStoreFloat3(&translationLocal, T);
	}

	void TransformComponent::ClearTransform()
	{
		SetDirty();
		scaleLocal = XMFLOAT3(1, 1, 1);
		rotationLocal = XMFLOAT4(0, 0, 0, 1);
		translationLocal = XMFLOAT3(0, 0, 0);
	}

	void TransformComponent::Translate(const XMFLOAT3& value)
	{
		SetDirty();
		translationLocal.x += value.x;
		translationLocal.y += value.y;
		translationLocal.z += value.z;
	}

	void TransformComponent::Translate(const XMVECTOR& value)
	{
		XMFLOAT3 translation;
		XMStoreFloat3(&translation, value);
		Translate(translation);
	}

	void TransformComponent::RotateRollPitchYaw(const XMFLOAT3& value)
	{
		SetDirty();

		XMVECTOR quat = XMLoadFloat4(&rotationLocal);
		XMVECTOR x = XMQuaternionRotationRollPitchYaw(value.x, 0, 0);
		XMVECTOR y = XMQuaternionRotationRollPitchYaw(0, value.y, 0);
		XMVECTOR z = XMQuaternionRotationRollPitchYaw(0, 0, value.z);

		quat = XMQuaternionMultiply(x, quat);
		quat = XMQuaternionMultiply(quat, y);
		quat = XMQuaternionMultiply(z, quat);
		quat = XMQuaternionNormalize(quat);

		XMStoreFloat4(&rotationLocal, quat);
	}

	void TransformComponent::Rotate(const XMFLOAT4& quaternion)
	{
		SetDirty();

		XMVECTOR result = XMQuaternionMultiply(XMLoadFloat4(&rotationLocal), XMLoadFloat4(&quaternion));
		result = XMQuaternionNormalize(result);
		XMStoreFloat4(&rotationLocal, result);
	}

	void TransformComponent::Rotate(const XMVECTOR& quaternion)
	{
		XMFLOAT4 rotation;
		XMStoreFloat4(&rotation, quaternion);
		Rotate(rotation);
	}
	void TransformComponent::Scale(const XMFLOAT3& value)
	{
		SetDirty();
		scaleLocal.x *= value.x;
		scaleLocal.y *= value.y;
		scaleLocal.z *= value.z;
	}
	void TransformComponent::Scale(const XMVECTOR& value)
	{
		XMFLOAT3 scale;
		XMStoreFloat3(&scale, value);
		Scale(scale);
	}
	void TransformComponent::MatrixTransform(const XMFLOAT4X4& matrix)
	{
		MatrixTransform(XMLoadFloat4x4(&matrix));
	}
	void TransformComponent::MatrixTransform(const XMMATRIX& matrix)
	{
		SetDirty();

		XMVECTOR S;
		XMVECTOR R;
		XMVECTOR T;
		XMMatrixDecompose(&S, &R, &T, GetLocalMatrix() * matrix);

		XMStoreFloat3(&scaleLocal, S);
		XMStoreFloat4(&rotationLocal, R);
		XMStoreFloat3(&translationLocal, T);
	}
	void TransformComponent::Lerp(const TransformComponent& a, const TransformComponent& b, float t)
	{
		SetDirty();

		XMVECTOR aS, aR, aT;
		XMMatrixDecompose(&aS, &aR, &aT, XMLoadFloat4x4(&a.world));

		XMVECTOR bS, bR, bT;
		XMMatrixDecompose(&bS, &bR, &bT, XMLoadFloat4x4(&b.world));

		XMVECTOR S = XMVectorLerp(aS, bS, t);
		XMVECTOR R = XMQuaternionSlerp(aR, bR, t);
		XMVECTOR T = XMVectorLerp(aT, bT, t);

		XMStoreFloat3(&scaleLocal, S);
		XMStoreFloat4(&rotationLocal, R);
		XMStoreFloat3(&translationLocal, T);
	}
	void TransformComponent::CatmullRom(const TransformComponent& a, const TransformComponent& b, const TransformComponent& c, const TransformComponent& d, float t)
	{
		SetDirty();

		XMVECTOR aS, aR, aT;
		XMMatrixDecompose(&aS, &aR, &aT, XMLoadFloat4x4(&a.world));

		XMVECTOR bS, bR, bT;
		XMMatrixDecompose(&bS, &bR, &bT, XMLoadFloat4x4(&b.world));

		XMVECTOR cS, cR, cT;
		XMMatrixDecompose(&cS, &cR, &cT, XMLoadFloat4x4(&c.world));

		XMVECTOR dS, dR, dT;
		XMMatrixDecompose(&dS, &dR, &dT, XMLoadFloat4x4(&d.world));

		XMVECTOR T = XMVectorCatmullRom(aT, bT, cT, dT, t);

		XMVECTOR setupA;
		XMVECTOR setupB;
		XMVECTOR setupC;

		aR = XMQuaternionNormalize(aR);
		bR = XMQuaternionNormalize(bR);
		cR = XMQuaternionNormalize(cR);
		dR = XMQuaternionNormalize(dR);

		XMQuaternionSquadSetup(&setupA, &setupB, &setupC, aR, bR, cR, dR);
		XMVECTOR R = XMQuaternionSquad(bR, setupA, setupB, setupC, t);

		XMVECTOR S = XMVectorCatmullRom(aS, bS, cS, dS, t);

		XMStoreFloat3(&scaleLocal, S);
		XMStoreFloat4(&rotationLocal, R);
		XMStoreFloat3(&translationLocal, T);
	}

}
