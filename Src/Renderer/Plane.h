#pragma once
#include "Mesh.h"

#include "Texture.h"


class Plane final : public Mesh
{
private:
	//D3D::BufferPtr m_pVertexBuffer;
	//D3D::BufferPtr m_pIndexBuffer;
	std::unique_ptr<Graphics::GPUBuffer> mpVertexBuffer;
	std::unique_ptr<Graphics::GPUBuffer> mpIndexBuffer;

	std::unique_ptr<Texture> mpTexture;

public:
	Plane(Graphics::GraphicsDevice* p_device, const wchar_t* filename);

	virtual void CreateBuffers(Graphics::GraphicsDevice* p_device) override;
	virtual void Render(
		Graphics::GraphicsDevice* p_device,
		float elapsed_time,
		const DirectX::XMMATRIX& world,
		CameraController* camera,
		Shader* shader,
		const Material& mat_data,
		bool isShadow = false,
		bool isSolid = true
	) override;

	virtual ~Plane();
};


class PlaneBatch : public Mesh
{
private:

	struct VertexForBatch
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};

	struct Instance
	{
		DirectX::XMFLOAT4X4 ndc_transform;
		//DirectX::XMFLOAT4 texcoord_transform;
		DirectX::XMFLOAT4 color;
	};
	const unsigned int MAX_INSTANCES = 256;

private:
	std::vector<VertexForBatch> mVertices;
	unsigned int mInstancesCount = 0;
	Instance* mpInstance;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> mpVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mpInstanceBuffer;
	std::unique_ptr<Graphics::GPUBuffer> m_pVertexBuffer;
	std::unique_ptr<Graphics::GPUBuffer> m_pInstanceBuffer;

	std::unique_ptr<Texture> mpTexture;

public:
	PlaneBatch(
		Graphics::GraphicsDevice* p_device,
		const wchar_t* filename, 
		const int max_instances);

	virtual void CreateBuffers(Graphics::GraphicsDevice* p_device) override;

	void Begin(Graphics::GraphicsDevice* p_device,
		const std::shared_ptr<Shader>& shader
		);
	virtual void Render(
		Graphics::GraphicsDevice* p_device,
		float elapsed_time,
		const DirectX::XMMATRIX& world,
		CameraController* camera,
		Shader* shader,
		const Material& mat_data,
		bool isShadow = false,
		bool isSolid = true
	) override;

	void Render(
		Graphics::GraphicsDevice* p_device,
		const DirectX::XMMATRIX& world,
		const std::shared_ptr<CameraController>& camera,
		const DirectX::XMFLOAT4& mat_color = DirectX::XMFLOAT4(1, 1, 1, 1),
		bool particle_mode = true
		);
	void End(Graphics::GraphicsDevice* p_device);

	~PlaneBatch();
};