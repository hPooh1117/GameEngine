#pragma once
#include "Component.h"

#include <string>
#include <d3d11.h>
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <wrl\client.h>
#include <iostream>

#include "./Renderer/Shader.h"
#include "./Utilities/Vector.h"

class Mesh;
class MeshRenderer;
class ColliderComponent;
class Texture;
class CameraController;
class LightController;

class MeshComponent : public Component
{
private:
	//std::shared_ptr<Mesh> m_mesh;
	std::shared_ptr<Mesh>   m_pMesh = nullptr;
	std::shared_ptr<Shader> m_pShader = nullptr;
	std::shared_ptr<Shader> m_pShaderForShadow = nullptr;
	u_int mMeshId;
	Vector4 mMatColor = { 1, 1, 1, 1 };
	bool mIsSolid = true;
	std::shared_ptr<ColliderComponent> m_pColData;
	std::vector<std::shared_ptr<Texture>> m_pShaderResources;

private:
	MeshComponent(const std::shared_ptr<Actor>& owner);
	
public:
	~MeshComponent();

	static std::shared_ptr<MeshComponent> Initialize(const std::shared_ptr<Actor>& owner);

	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time);
	void RenderMesh(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context,
		float elapsed_time,
		const std::shared_ptr<CameraController>& camera);
	void RenderShadow(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context,
		float elapsed_time,
		const std::shared_ptr<CameraController>& p_camera
		);

	bool Load(int mesh_id, const std::shared_ptr<MeshRenderer>& renderer);
	bool Load(const char* filename, int mesh_id, const std::shared_ptr<MeshRenderer>& renderer);
	bool Load(const wchar_t* filename, int mesh_id, const std::shared_ptr<MeshRenderer>& renderer);

	bool AddShaderResource(
		Microsoft::WRL::ComPtr<ID3D11Device>& pDevice,
		const wchar_t* filename);

	bool AddMotion(std::string name, const char* filename);
	void Play(std::string name);

	void SetAdditionalResource(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context);
	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY type);
	inline void SetShader(const std::shared_ptr<Shader>& shader, bool isForShadow = false){
		if (isForShadow) m_pShaderForShadow = shader;
		else             m_pShader = shader;
	}
	inline void SetColor(const Vector4& color) {
		mMatColor = color;
	}
	inline void SetSolidMode() { mIsSolid = true; }
	inline void SetWireframeMode() { mIsSolid = false; }
	inline void SetColliderData(const std::shared_ptr<ColliderComponent> col) { m_pColData = col; }
	inline std::shared_ptr<Mesh>& GetMesh() {
		return m_pMesh;
	}
	static constexpr int GetID() { return ComponentID::kRender; }
	inline int GetMeshID() { return mMeshId; }
	inline bool IsUsingShadow() { return m_pShaderForShadow != nullptr; }
public:
	enum MeshID
	{
		kBasicCube,
		kBasicCylinder,
		kBasicSphere,
		kBasicCapsule,
		kBasicLine,

		kPlane,

		kStaticMesh,
		kSkinnedMesh,

		MESH_ID_MAX,
	};
};