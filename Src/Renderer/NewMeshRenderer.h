#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "./Application/Helper.h"

#include "./Renderer/D3D_Helper.h"

class ActorManager;
class Mesh;
class NewMeshComponent;
class TextureHolder;

class NewMeshRenderer
{
private:
	std::unordered_map<unsigned int, std::shared_ptr<NewMeshComponent>> mMeshDataTable;
	std::unordered_map<unsigned int, std::unique_ptr<Mesh>>             mMeshTable;
	std::unique_ptr<Skybox>                                             mpSkybox;
	std::wstring                                                        mSkyboxFilename;
public:
	NewMeshRenderer();
	~NewMeshRenderer() = default;

	bool Initialize(D3D::DevicePtr& p_device);
	bool InitializeMeshes(D3D::DevicePtr& p_device);
	void RegistMeshDataFromActors(D3D::DevicePtr& p_device, const std::unique_ptr<ActorManager>& p_actors);
	void RenderSkybox(D3D::DeviceContextPtr& p_imm_context, float elapsed_time, unsigned int current_pass_id);
	void RenderMesh(D3D::DeviceContextPtr& p_imm_context, float elapsed_time, unsigned int current_pass_id);
	UINT ChooseShaderUsageForMesh(UINT current_pass); // 現在の描画パスによって、idを選択
	UINT ChooseShaderIdForSkybox(UINT current_pass);
	void RenderUI();
	void ClearAll();

	void SetSkybox(int id);
};