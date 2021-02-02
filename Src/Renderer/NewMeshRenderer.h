#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "./Application/Helper.h"

#include "./Renderer/D3D_Helper.h"
#include "./Renderer/GraphicsDevice.h"
#include "./Renderer/Skybox.h"
#include "./Renderer/Mesh.h"
#include "./Component/MeshComponent.h"

#include "./Engine/ActorManager.h"
#include "./Renderer/TextureHolder.h"
#include "./Renderer/ComputedTexture.h"


class NewMeshRenderer
{
private:
	std::unordered_map<unsigned int, MeshComponent*> mMeshDataTable;
	std::unordered_map<unsigned int, std::unique_ptr<Mesh>>             mMeshTable;
	std::unique_ptr<Skybox>                                             mpSkybox;
	std::wstring                                                        mSkyboxFilename;

	std::unique_ptr<Graphics::GPUBuffer> mpCBufferForMesh;
	std::unique_ptr<Graphics::GPUBuffer> mpCBufferForMaterial;

public:
	NewMeshRenderer();
	~NewMeshRenderer() = default;


	void CreateConstantBuffers(Graphics::GraphicsDevice* p_device);
	bool Initialize(Graphics::GraphicsDevice* device);
	bool InitializeMeshes(Graphics::GraphicsDevice* device);

	void RegistMeshDataFromActors(Graphics::GraphicsDevice* device, ActorManager* p_actors);

	void Render(Graphics::GraphicsDevice* device, float elapsed_time, unsigned int current_pass_id);
	void RenderMesh(Graphics::GraphicsDevice* device, float elapsed_time, unsigned int current_pass_id);

private:
	void RenderSkybox(Graphics::GraphicsDevice* device, float elapsed_time, unsigned int current_pass_id);
	UINT ChooseShaderUsageForMesh(UINT current_pass); // 現在の描画パスによって、idを選択
	UINT ChooseShaderIdForSkybox(UINT current_pass);
public:
	void RenderUI();
	void ClearAll();
	
	ComputedTexture* GetCurrentSkyTex();
	void SetSkybox(int id);
};