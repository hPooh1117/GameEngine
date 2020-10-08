#pragma once
#include <vector>
#include <memory>

#include "./Application/Helper.h"

class MeshComponent;
class GeometricPrimitiveSelf;
class CameraController;

class MeshRenderer final
{
private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_pD3dDevice;
	std::shared_ptr<CameraController> m_pCameraController;

private:
	std::vector<std::shared_ptr<MeshComponent>> m_pRenderQueue;

public:
	MeshRenderer(
		Microsoft::WRL::ComPtr<ID3D11Device>& device, 
		const std::shared_ptr<CameraController>& camera
		);

	//std::shared_ptr<Mesh>& addMesh(const char* filename, int mesh_id, std::shared_ptr<Shader>& shader);
	bool AddMesh(int mesh_id, const std::shared_ptr<MeshComponent>& mesh);
	bool AddMesh(const char* filename, int mesh_id, const std::shared_ptr<MeshComponent>& mesh);
	bool AddMesh(const wchar_t* filename, int mesh_id, const std::shared_ptr<MeshComponent>& mesh);

	bool AddShaderForShadow(const std::shared_ptr<Shader>& p_shader_for_shadow);

	void RenderShadowMapQueue(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context,
		float elapsed_time,
		const std::shared_ptr<CameraController>& p_camera);

	void RenderQueue(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context, float elapsed_time);

	virtual ~MeshRenderer();

	private:
	MeshRenderer(const MeshRenderer&) = delete;
	MeshRenderer& operator=(const MeshRenderer&) = delete;
};