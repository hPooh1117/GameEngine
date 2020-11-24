//#include "MeshRenderer.h"
//
//#include <DirectXMath.h>
//
//#include "ActorManager.h"
//#include "CameraController.h"
//#include "GameSystem.h"
//#include "LightController.h"
//
//#include "./Component/MeshComponent.h"
//
//#include "./Renderer/GeometricPrimitiveSelf.h"
//#include "./Renderer/StaticMesh.h"
//#include "./Renderer/SkinnedMesh.h"
//#include "./Renderer/Plane.h"
//
//#include "./RenderPass/RenderPasses.h"
//
////--------------------------------------------------------------------------------------------------------------------------------
//
//using namespace DirectX;
//
//#pragma region OLDIMPL
////--------------------------------------------------------------------------------------------------------------------------------
//
//MeshRenderer::MeshRenderer(
//	D3D::DevicePtr& device,
//	const std::shared_ptr<CameraController>& camera) 
//	:m_pD3dDevice(device), m_pCameraController(camera)
//{
//}
//
////--------------------------------------------------------------------------------------------------------------------------------
//
//bool MeshRenderer::AddMesh(int mesh_id, const std::shared_ptr<MeshComponent>& mesh)
//{
//	m_pRenderQueue.emplace_back(mesh);
//
//	switch (mesh_id)
//	{
//	case MeshComponent::MeshID::EBasicCylinder:
//		mesh->GetMesh() = std::make_shared<BasicCylinder>(m_pD3dDevice);
//		break;
//	case MeshComponent::MeshID::EBasicCapsule:
//		mesh->GetMesh() = std::make_shared<BasicCapsule>(m_pD3dDevice);
//		break;
//	case MeshComponent::MeshID::EBasicLine:
//		mesh->GetMesh() = std::make_shared<BasicLine>(m_pD3dDevice);
//		break;
//	default:
//		return false;
//	}
//
//	return true;
//
//}
//
////--------------------------------------------------------------------------------------------------------------------------------
//
//bool MeshRenderer::AddMesh(const char* filename, int mesh_id, const std::shared_ptr<MeshComponent>& mesh)
//{
//	assert(mesh_id == MeshComponent::MeshID::ESkinnedMesh);
//
//	m_pRenderQueue.emplace_back(mesh);
//	
//		mesh->GetMesh() = std::make_shared<SkinnedMesh>(m_pD3dDevice, filename, 0);
//
//	return true;
//}
//
////--------------------------------------------------------------------------------------------------------------------------------
//
//bool MeshRenderer::AddMesh(const wchar_t* filename, int mesh_id, const std::shared_ptr<MeshComponent>& mesh)
//{
//	assert(
//		mesh_id == MeshComponent::MeshID::EStaticMesh || 
//		mesh_id == MeshComponent::MeshID::EPlane ||
//		mesh_id == MeshComponent::MeshID::EBasicSphere || 
//		mesh_id == MeshComponent::MeshID::EBasicCube
//	);
//
//	m_pRenderQueue.emplace_back(mesh);
//
//	switch (mesh_id)
//	{
//	case MeshComponent::MeshID::EBasicSphere:
//		mesh->GetMesh() = std::make_shared<BasicSphere>(m_pD3dDevice, 16, 16);
//		break;
//	case MeshComponent::MeshID::EBasicCube:
//		mesh->GetMesh() = std::make_shared<BasicCube>(m_pD3dDevice);
//		break;
//	case MeshComponent::MeshID::EPlane:
//		mesh->GetMesh() = std::make_shared<Plane>(m_pD3dDevice, filename);
//		break;
//	case MeshComponent::MeshID::EStaticMesh:
//		mesh->GetMesh() = std::make_shared<StaticMesh>(m_pD3dDevice, filename, true);
//		break;
//	}
//
//	return true;
//}
//
////--------------------------------------------------------------------------------------------------------------------------------
//
//bool MeshRenderer::AddShaderForShadow(const std::shared_ptr<Shader>& p_shader_for_shadow)
//{
//	for (auto& mesh : m_pRenderQueue)
//	{
//		mesh->SetShader(p_shader_for_shadow, true);
//	}
//	return true;
//}
//
////--------------------------------------------------------------------------------------------------------------------------------
//
//void MeshRenderer::RenderShadowMapQueue(
//	D3D::DeviceContextPtr& imm_context,
//	float elapsed_time,
//	const std::shared_ptr<CameraController>& p_camera)
//{
//	for (auto& mesh : m_pRenderQueue)
//	{
//		mesh->RenderShadow(imm_context, elapsed_time, p_camera);
//	}
//}
//
////--------------------------------------------------------------------------------------------------------------------------------
//
//void MeshRenderer::RenderQueue(D3D::DeviceContextPtr& imm_context, float elapsed_time)
//{
//	for (auto& mesh : m_pRenderQueue)
//	{
//		mesh->RenderMesh(imm_context, elapsed_time, m_pCameraController);
//	}
//}
//
////--------------------------------------------------------------------------------------------------------------------------------
//
//MeshRenderer::~MeshRenderer()
//{
//}
//
////--------------------------------------------------------------------------------------------------------------------------------
//#pragma endregion
//
//
////NewMeshRenderer::NewMeshRenderer()
////{
////}
////
////bool NewMeshRenderer::Create(D3D::DevicePtr& p_device)
////{
////	return true;
////}
////
////bool NewMeshRenderer::InitializeMeshes(D3D::DevicePtr& p_device)
////{
////	return true;
////}
////
////void NewMeshRenderer::RegistMeshDataFromActors(const std::unique_ptr<ActorManager>& p_actors)
////{
////	for (auto i = 0u; i < p_actors->GetActorsSize(); ++i)
////	{
////		if (mMeshDataTable.find(i) != mMeshDataTable.end()) continue;
////		mMeshDataTable.emplace(i, p_actors->GetActor(i)->GetComponent<NewMeshComponent>());
////	}
////
////
////}
////
////void NewMeshRenderer::RenderMesh(D3D::DeviceContextPtr& p_imm_context, float elapsed_time, u_int current_pass_id)
////{
////	for (auto& mesh : mMeshTable)
////	{
////		std::shared_ptr<NewMeshComponent> component = mMeshDataTable.find(mesh.first)->second;
////		DirectX::XMMATRIX world = component->GetWorldMatrix();
////		Vector4 matColor = component->GetMaterialColor();
////		bool bIsSolid = component->GetIsSolid();
////		ShaderID mID = component->GetShaderID(0);
////		ENGINE.GetRenderPass(current_pass_id)->SetShader(p_imm_context, mID);
////
////		mesh.second->Render(p_imm_context, elapsed_time, world, ENGINE.GetCameraPtr().get(), nullptr, matColor, bIsSolid);
////	}
////}
