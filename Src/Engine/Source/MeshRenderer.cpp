#include "MeshRenderer.h"

#include <DirectXMath.h>

#include "CameraController.h"
#include "LightController.h"

#include "./Component/MeshComponent.h"

#include "./Renderer/GeometricPrimitiveSelf.h"
#include "./Renderer/StaticMesh.h"
#include "./Renderer/SkinnedMesh.h"
#include "./Renderer/Plane.h"

//--------------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;


//--------------------------------------------------------------------------------------------------------------------------------

MeshRenderer::MeshRenderer(
	Microsoft::WRL::ComPtr<ID3D11Device>& device,
	const std::shared_ptr<CameraController>& camera) :m_pD3dDevice(device), m_pCameraController(camera)
{
}

//--------------------------------------------------------------------------------------------------------------------------------

bool MeshRenderer::AddMesh(int mesh_id, const std::shared_ptr<MeshComponent>& mesh)
{
	m_pRenderQueue.emplace_back(mesh);

	switch (mesh_id)
	{
	case MeshComponent::MeshID::kBasicCylinder:
		mesh->GetMesh() = std::make_shared<BasicCylinder>(m_pD3dDevice);
		break;
	case MeshComponent::MeshID::kBasicCapsule:
		mesh->GetMesh() = std::make_shared<BasicCapsule>(m_pD3dDevice);
		break;
	case MeshComponent::MeshID::kBasicLine:
		mesh->GetMesh() = std::make_shared<BasicLine>(m_pD3dDevice);
		break;
	default:
		return false;
	}

	return true;

}

//--------------------------------------------------------------------------------------------------------------------------------

bool MeshRenderer::AddMesh(const char* filename, int mesh_id, const std::shared_ptr<MeshComponent>& mesh)
{
	assert(mesh_id == MeshComponent::MeshID::kSkinnedMesh);

	m_pRenderQueue.emplace_back(mesh);
	
		mesh->GetMesh() = std::make_shared<SkinnedMesh>(m_pD3dDevice, filename);

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool MeshRenderer::AddMesh(const wchar_t* filename, int mesh_id, const std::shared_ptr<MeshComponent>& mesh)
{
	assert(
		mesh_id == MeshComponent::MeshID::kStaticMesh || 
		mesh_id == MeshComponent::MeshID::kPlane ||
		mesh_id == MeshComponent::MeshID::kBasicSphere || 
		mesh_id == MeshComponent::MeshID::kBasicCube
	);

	m_pRenderQueue.emplace_back(mesh);

	switch (mesh_id)
	{
	case MeshComponent::MeshID::kBasicSphere:
		mesh->GetMesh() = std::make_shared<BasicSphere>(m_pD3dDevice, filename, 16, 16);
		break;
	case MeshComponent::MeshID::kBasicCube:
		mesh->GetMesh() = std::make_shared<BasicCube>(m_pD3dDevice, filename);
		break;
	case MeshComponent::MeshID::kPlane:
		mesh->GetMesh() = std::make_shared<Plane>(m_pD3dDevice, filename);
		break;
	case MeshComponent::MeshID::kStaticMesh:
		mesh->GetMesh() = std::make_shared<StaticMesh>(m_pD3dDevice, filename, true);
		break;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool MeshRenderer::AddShaderForShadow(const std::shared_ptr<Shader>& p_shader_for_shadow)
{
	for (auto& mesh : m_pRenderQueue)
	{
		mesh->SetShader(p_shader_for_shadow, true);
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void MeshRenderer::RenderShadowMapQueue(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context,
	float elapsed_time,
	const std::shared_ptr<CameraController>& p_camera)
{
	for (auto& mesh : m_pRenderQueue)
	{
		mesh->RenderShadow(imm_context, elapsed_time, p_camera);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

void MeshRenderer::RenderQueue(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context, float elapsed_time)
{
	for (auto& mesh : m_pRenderQueue)
	{
		mesh->RenderMesh(imm_context, elapsed_time, m_pCameraController);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

MeshRenderer::~MeshRenderer()
{
}

//--------------------------------------------------------------------------------------------------------------------------------
