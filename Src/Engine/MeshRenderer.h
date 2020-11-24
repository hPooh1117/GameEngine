//#pragma once
//#include <vector>
//#include <memory>
//#include <unordered_map>
//
//#include "./Application/Helper.h"
//
//#include "./Renderer/D3D_Helper.h"
//
//class Mesh;
//class MeshComponent;
//class GeometricPrimitiveSelf;
//class ActorManager;
//class CameraController;
//
//
//
//#pragma region OLDIMPL
//class MeshRenderer final
//{
//private:
//	D3D::DevicePtr m_pD3dDevice;
//	std::shared_ptr<CameraController> m_pCameraController;
//
//private:
//	std::vector<std::shared_ptr<MeshComponent>> m_pRenderQueue;
//
//public:
//	MeshRenderer(
//		D3D::DevicePtr& device,
//		const std::shared_ptr<CameraController>& camera
//		);
//
//	//std::shared_ptr<Mesh>& addMesh(const char* filename, int mesh_id, std::shared_ptr<Shader>& shader);
//	bool AddMesh(int mesh_id, const std::shared_ptr<MeshComponent>& mesh);
//	bool AddMesh(const char* filename, int mesh_id, const std::shared_ptr<MeshComponent>& mesh);
//	bool AddMesh(const wchar_t* filename, int mesh_id, const std::shared_ptr<MeshComponent>& mesh);
//
//	bool AddShaderForShadow(const std::shared_ptr<Shader>& p_shader_for_shadow);
//
//	void RenderShadowMapQueue(
//		D3D::DeviceContextPtr& imm_context,
//		float elapsed_time,
//		const std::shared_ptr<CameraController>& p_camera);
//
//	void RenderQueue(D3D::DeviceContextPtr& imm_context, float elapsed_time);
//
//	virtual ~MeshRenderer();
//
//	private:
//	MeshRenderer(const MeshRenderer&) = delete;
//	MeshRenderer& operator=(const MeshRenderer&) = delete;
//};
//#pragma endregion
//
////class NewMeshComponent;
////
////class NewMeshRenderer
////{
////private:
////	std::unordered_map<u_int, std::shared_ptr<NewMeshComponent>> mMeshDataTable;
////	std::unordered_map<u_int, std::unique_ptr<Mesh>>             mMeshTable;
////
////public:
////	NewMeshRenderer();
////	~NewMeshRenderer() = default;
////
////	bool Create(D3D::DevicePtr& p_device);
////	bool InitializeMeshes(D3D::DevicePtr& p_device);
////	void RegistMeshDataFromActors(const std::unique_ptr<ActorManager>& p_actors);
////	void RenderMesh(D3D::DeviceContextPtr& p_imm_context, float elapsed_time, u_int current_pass_id);
////};