#include "Plane.h"
#include "./Utilities/misc.h"
#include <DirectXMath.h>
#include "Shader.h"
#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "ResourceManager.h"

//--------------------------------------------------------------------------------------------------------------------------------


using namespace DirectX;


//********************************************************************************************************************************
//
//      Plane
//
//********************************************************************************************************************************
//--------------------------------------------------------------------------------------------------------------------------------

Plane::Plane(Graphics::GraphicsDevice* p_device, const wchar_t* filename) :Mesh()
{
	auto result = S_OK;
	auto& pDevice = p_device->GetDevicePtr();

	Vertex vertices[] = {
	{ XMFLOAT3(-0.5, +0.0, +0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1) },
	{ XMFLOAT3(+0.5, +0.0, +0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1) },
	{ XMFLOAT3(-0.5, +0.0, -0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1) },
	{ XMFLOAT3(+0.5, +0.0, -0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1) },

	};

	for (auto& vertex : vertices)
	{
		mVertices.emplace_back(vertex);
	}

	uint32_t indices[] =
	{
		0, 1, 2, 2, 1, 3
	};
	for (auto& index : indices)
	{
		mIndices.emplace_back(index);
	}

	CreateBuffers(p_device);

	result = pDevice->CreateBuffer(
		&CD3D11_BUFFER_DESC(
			sizeof(CBufferForMesh),
			D3D11_BIND_CONSTANT_BUFFER,
			D3D11_USAGE_DEFAULT,
			0,
			0,
			0
		),
		nullptr,
		mpConstantBufferMesh.GetAddressOf()
	);
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));

	result = pDevice->CreateBuffer(
		&CD3D11_BUFFER_DESC(
			sizeof(CBufferForMaterial),
			D3D11_BIND_CONSTANT_BUFFER,
			D3D11_USAGE_DEFAULT,
			0,
			0,
			0
		),
		nullptr,
		mpConstantBufferMaterial.GetAddressOf()
	);
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));



	//result = pDevice->CreateRasterizerState(
	//	&CD3D11_RASTERIZER_DESC(
	//		D3D11_FILL_WIREFRAME,     // D3D11_FILL_MODE FillMode;
	//		D3D11_CULL_NONE,          // D3D11_CULL_MODE CullMode;
	//		FALSE,                    // BOOL FrontCounterClockwise;
	//		0,                        // INT DepthBias;
	//		0.0f,                     // FLOAT DepthBiasClamp;
	//		0.0f,                     // FLOAT SlopeScaledDepthBias;
	//		TRUE,                    // BOOL DepthClipEnable;
	//		FALSE,                    // BOOL ScissorEnable;
	//		FALSE,                    // BOOL MultisampleEnable;
	//		FALSE                     // BOOL AntialiasedLineEnable;
	//	),
	//	m_pRasterizerWire.GetAddressOf()
	//);
	//_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));

	//result = pDevice->CreateRasterizerState(
	//	&CD3D11_RASTERIZER_DESC(
	//		D3D11_FILL_SOLID,         // D3D11_FILL_MODE FillMode;
	//		D3D11_CULL_NONE,          // D3D11_CULL_MODE CullMode;
	//		FALSE,                    // BOOL FrontCounterClockwise;
	//		0,                        // INT DepthBias;
	//		0.0f,                     // FLOAT DepthBiasClamp;
	//		0.0f,                     // FLOAT SlopeScaledDepthBias;
	//		TRUE,                    // BOOL DepthClipEnable;
	//		FALSE,                    // BOOL ScissorEnable;
	//		FALSE,                    // BOOL MultisampleEnable;
	//		FALSE                     // BOOL AntialiasedLineEnable;
	//	),
	//	m_pRasterizerSolid.GetAddressOf()
	//);
	//_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));
}

//--------------------------------------------------------------------------------------------------------------------------------

void Plane::CreateBuffers(Graphics::GraphicsDevice* p_device)
{
	//HRESULT hr = S_OK;
	//auto& pDevice = p_device->GetDevicePtr();

	////--> Create Vertex Buffer
	//D3D11_SUBRESOURCE_DATA vSrData = {};
	//vSrData.pSysMem = mVertices.data();
	//vSrData.SysMemPitch = 0;
	//vSrData.SysMemSlicePitch = 0;

	//hr = pDevice->CreateBuffer(
	//	&CD3D11_BUFFER_DESC(
	//		sizeof(Vertex) * mVertices.size(),
	//		D3D11_BIND_VERTEX_BUFFER,
	//		D3D11_USAGE_IMMUTABLE,
	//		0,
	//		0,
	//		0
	//	),
	//	&vSrData,
	//	m_pVertexBuffer.GetAddressOf()
	//);
	//_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));



	//D3D11_SUBRESOURCE_DATA iSrData = {};
	//iSrData.pSysMem = mIndices.data();
	//iSrData.SysMemPitch = 0;
	//iSrData.SysMemSlicePitch = 0;


	//hr = pDevice->CreateBuffer(
	//	&CD3D11_BUFFER_DESC(
	//		sizeof(u_int) * mIndices.size(),
	//		D3D11_BIND_INDEX_BUFFER,
	//		D3D11_USAGE_IMMUTABLE,
	//		0,
	//		0,
	//		0
	//	),
	//	&iSrData,
	//	m_pIndexBuffer.GetAddressOf()
	//);
	//_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	Graphics::GPUBufferDesc desc = {};
	desc.ByteWidth = sizeof(Vertex) * mVertices.size();
	desc.BindFlags = Graphics::BIND_VERTEX_BUFFER;
	desc.Usage = Graphics::USAGE_IMMUTABLE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	Graphics::SubresourceData data = {};
	data.pSysMem = mVertices.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	p_device->CreateBuffer(&desc, &data, mpVertexBuffer.get());

	desc.ByteWidth = sizeof(uint32_t) * mIndices.size();
	desc.BindFlags = Graphics::BIND_INDEX_BUFFER;
	data.pSysMem = mIndices.data();

	p_device->CreateBuffer(&desc, &data, mpIndexBuffer.get());
}

//--------------------------------------------------------------------------------------------------------------------------------

void Plane::Render(
	Graphics::GraphicsDevice* p_device,
	float elapsed_time,
	const DirectX::XMMATRIX& world,
	CameraController* camera,
	Shader* shader,
	const Material& mat_data,
	bool isShadow,
	bool isSolid)
{
	HRESULT hr = S_OK;
	auto& ImmContext = p_device->GetImmContextPtr();

	// シェーダを設定
	if (shader != nullptr)shader->ActivateShaders(ImmContext);


	CBufferForMesh meshData = {};
	XMStoreFloat4x4(&meshData.world, world);
	XMStoreFloat4x4(&meshData.WVP, world * camera->GetViewMatrix() * camera->GetProjMatrix(ImmContext));
	XMStoreFloat4x4(&meshData.invViewProj, camera->GetInvProjViewMatrix(ImmContext));
	DirectX::XMStoreFloat4x4(&meshData.invView, camera->GetInvViewMatrix());
	DirectX::XMStoreFloat4x4(&meshData.invProj, camera->GetInvProjMatrix(ImmContext));

	ImmContext->UpdateSubresource(mpConstantBufferMesh.Get(), 0, nullptr, &meshData, 0, 0);
	ImmContext->VSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
	ImmContext->PSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());

	CBufferForMaterial matData = {};
	matData.mat_color = mat_data.mat_color;
	matData.metalness = mat_data.metalness;
	matData.roughness = mat_data.roughness;
	matData.specularColor = mat_data.specularColor;
	matData.textureConfig = mat_data.textureConfig;
	matData.diffuse = mat_data.diffuse;
	matData.specular = mat_data.specular;


	ImmContext->UpdateSubresource(mpConstantBufferMaterial.Get(), 0, nullptr, &matData, 0, 0);
	ImmContext->VSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
	ImmContext->HSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
	ImmContext->DSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
	ImmContext->GSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
	ImmContext->PSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());


	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	//ImmContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	//ImmContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//ImmContext->IASetPrimitiveTopology(mTopologyType);
	Graphics::GPUBuffer* vb = mpVertexBuffer.get();
	p_device->BindVertexBuffer(&vb, 0, 1, &stride, &offset);
	p_device->BindIndexBuffer(mpIndexBuffer.get(), Graphics::IndexBufferFormat::INDEXFORMAT_32BIT, 0);
	p_device->BindPrimitiveTopology(mToplogyID);
	p_device->RSSetState(isSolid ? Graphics::RS_DOUBLESIDED : Graphics::RS_WIRE_DOUBLESIDED);

	//isSolid ? ImmContext->RSSetState(m_pRasterizerSolid.Get())
	//	    : ImmContext->RSSetState(m_pRasterizerWire.Get());

	//mpTextures->Set(imm_context);

	Graphics::GPUBufferDesc bufDesc = mpIndexBuffer->GetDesc();
	ImmContext->DrawIndexed(bufDesc.ByteWidth / sizeof(u_int), 0, 0);

}

//--------------------------------------------------------------------------------------------------------------------------------

Plane::~Plane()
{
}

//--------------------------------------------------------------------------------------------------------------------------------


//********************************************************************************************************************************
//
//      PlaneBatch
//
//********************************************************************************************************************************
//--------------------------------------------------------------------------------------------------------------------------------

PlaneBatch::PlaneBatch(
	Graphics::GraphicsDevice* p_device,
	const wchar_t* filename,
	const int max_instances)
	:Mesh(),
	MAX_INSTANCES(max_instances)
{
	HRESULT hr = S_OK;
	auto& pDevice = p_device->GetDevicePtr();

	mToplogyID = Graphics::TRIANGLESTRIP;

	VertexForBatch vertices[] =
	{
		{XMFLOAT3(-0.5f, -0.5f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0, 0)},
		{XMFLOAT3(+0.5f, -0.5f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(1, 0)},
		{XMFLOAT3(-0.5f, +0.5f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0, 1)},
		{XMFLOAT3(+0.5f, +0.5f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(1, 1)}
	};

	for (auto& vertex : vertices)
	{
		mVertices.emplace_back(vertex);
	}

	CreateBuffers(p_device);

	//D3D11_RASTERIZER_DESC rrDesc = {};
	//rrDesc.AntialiasedLineEnable = FALSE;
	//rrDesc.FillMode = D3D11_FILL_SOLID;
	//rrDesc.CullMode = D3D11_CULL_NONE;
	//rrDesc.FrontCounterClockwise = FALSE;
	//rrDesc.DepthBias = 0;
	//rrDesc.DepthBiasClamp = 0;
	//rrDesc.SlopeScaledDepthBias = 0;
	//rrDesc.DepthClipEnable = TRUE;
	//rrDesc.ScissorEnable = FALSE;
	//rrDesc.MultisampleEnable = FALSE;
	//hr = pDevice->CreateRasterizerState(&rrDesc, m_pRasterizerSolid.GetAddressOf());
	//if (FAILED(hr)) return;

	mpTexture = std::make_unique<Texture>();
	if (filename != L"\0")
	{
		mpTexture->Load(pDevice, filename);
	}
	else
	{
		mpTexture->Load(pDevice);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

void PlaneBatch::CreateBuffers(Graphics::GraphicsDevice* p_device)
{
	//HRESULT hr = S_OK;
	//auto& pDevice = p_device->GetDevicePtr();

	////--> Create Vertex Buffer
	//D3D11_SUBRESOURCE_DATA vSrData = {};
	//vSrData.pSysMem = mVertices.data();
	//vSrData.SysMemPitch = 0;
	//vSrData.SysMemSlicePitch = 0;


	//hr = pDevice->CreateBuffer(
	//	&CD3D11_BUFFER_DESC(
	//		sizeof(VertexForBatch) * mVertices.size(),
	//		D3D11_BIND_VERTEX_BUFFER,
	//		D3D11_USAGE_IMMUTABLE,
	//		0,
	//		0,
	//		0
	//	),
	//	&vSrData,
	//	mpVertexBuffer.GetAddressOf()
	//);
	//_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));


	//Instance* instances = new Instance[MAX_INSTANCES];
	//{
	//	D3D11_BUFFER_DESC ibDesc = {};
	//	ibDesc.Usage = D3D11_USAGE_DYNAMIC;
	//	ibDesc.ByteWidth = sizeof(Instance) * MAX_INSTANCES;
	//	ibDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//	ibDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//	ibDesc.MiscFlags = 0;
	//	ibDesc.StructureByteStride = 0;

	//	D3D11_SUBRESOURCE_DATA srData = {};
	//	srData.pSysMem = instances;
	//	srData.SysMemPitch = 0;
	//	srData.SysMemSlicePitch = 0;

	//	hr = pDevice->CreateBuffer(&ibDesc, &srData, mpInstanceBuffer.GetAddressOf());
	//	if (FAILED(hr)) return;
	//}
	//delete[] instances;

	Graphics::GPUBufferDesc desc = {};
	desc.ByteWidth = sizeof(Vertex) * mVertices.size();
	desc.BindFlags = Graphics::BIND_VERTEX_BUFFER;
	desc.Usage = Graphics::USAGE_IMMUTABLE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	Graphics::SubresourceData data = {};
	data.pSysMem = mVertices.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	p_device->CreateBuffer(&desc, &data, m_pVertexBuffer.get());

	Instance* instances = new Instance[MAX_INSTANCES];
	desc.ByteWidth = sizeof(Instance) * MAX_INSTANCES;
	desc.Usage = Graphics::USAGE_DYNAMIC;
	desc.BindFlags = Graphics::BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = Graphics::CPU_ACCESS_WRITE;
	data.pSysMem = instances;

	p_device->CreateBuffer(&desc, &data, m_pInstanceBuffer.get());
	delete[] instances;

}

//--------------------------------------------------------------------------------------------------------------------------------

void PlaneBatch::Begin(Graphics::GraphicsDevice* p_device, const std::shared_ptr<Shader>& shader)
{
	HRESULT hr = S_OK;
	auto& pImmContext = p_device->GetImmContextPtr();

	shader->ActivateShaders(pImmContext);

	UINT strides[2] = { sizeof(VertexForBatch), sizeof(Instance) };
	UINT offsets[2] = { 0,0 };

	//ID3D11Buffer* vbs[2] = { mpVertexBuffer.Get(), mpInstanceBuffer.Get() };
	//pImmContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	//pImmContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Graphics::GPUBuffer* buffer[2] = { m_pVertexBuffer.get(), m_pInstanceBuffer.get() };
	p_device->BindVertexBuffer(buffer, 0, 2, strides, offsets);
	p_device->BindPrimitiveTopology(mToplogyID);

	//pImmContext->RSSetState(m_pRasterizerSolid.Get());

	mpTexture->Set(pImmContext);


	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;
	hr = pImmContext->Map(mpInstanceBuffer.Get(), 0, map, 0, &mappedBuffer);
	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	mpInstance = static_cast<Instance*>(mappedBuffer.pData);

	mInstancesCount = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------

void PlaneBatch::Render(
	Graphics::GraphicsDevice* p_device,
	float elapsed_time,
	const DirectX::XMMATRIX& world,
	CameraController* camera,
	Shader* shader,
	const Material& mat_data,
	bool isShadow,
	bool isSolid
)
{
	// TODO GPUBufferに対応
	_ASSERT_EXPR(mInstancesCount < MAX_INSTANCES, L"Too Many Instances is EXISTING");

	auto& ImmContext = p_device->GetImmContextPtr();
	if (shader != nullptr) shader->ActivateShaders(ImmContext);
	p_device->RSSetState(isSolid ? Graphics::RS_DOUBLESIDED : Graphics::RS_WIRE_DOUBLESIDED);

	DirectX::XMMATRIX view_proj = camera->GetViewMatrix() * camera->GetProjMatrix(ImmContext);
	XMStoreFloat4x4(&mpInstance[mInstancesCount].ndc_transform, DirectX::XMMatrixTranspose(world * view_proj));

	//mpInstance[mInstancesCount].texcoord_transform = DirectX::XMFLOAT4(0, 0, 1, 1);
	mpInstance[mInstancesCount].color = mat_data.mat_color;


	mInstancesCount++;

}

//--------------------------------------------------------------------------------------------------------------------------------

void PlaneBatch::Render(
	Graphics::GraphicsDevice* p_device,
	const DirectX::XMMATRIX& world,
	const std::shared_ptr<CameraController>& camera,
	const DirectX::XMFLOAT4& mat_color,
	bool particle_mode)
{
	_ASSERT_EXPR(mInstancesCount < MAX_INSTANCES, L"Too Many Instances is EXISTING");
	auto& ImmContext = p_device->GetImmContextPtr();
	DirectX::XMMATRIX view_proj = camera->GetViewMatrix() * camera->GetProjMatrix(ImmContext);
	XMMATRIX W = world;
	if (particle_mode) W = camera->GetInvViewMatrix() * world;
	XMStoreFloat4x4(&mpInstance[mInstancesCount].ndc_transform, DirectX::XMMatrixTranspose(W * view_proj));

	mpInstance[mInstancesCount].color = mat_color;


	mInstancesCount++;
}

//--------------------------------------------------------------------------------------------------------------------------------

void PlaneBatch::End(Graphics::GraphicsDevice* p_device)
{
	auto& pImmContext = p_device->GetImmContextPtr();
	pImmContext->Unmap(mpInstanceBuffer.Get(), 0);
	pImmContext->DrawInstanced(4, mInstancesCount, 0, 0);
}

//--------------------------------------------------------------------------------------------------------------------------------

PlaneBatch::~PlaneBatch()
{
}

//--------------------------------------------------------------------------------------------------------------------------------
