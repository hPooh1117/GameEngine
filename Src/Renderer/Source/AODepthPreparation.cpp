#include "..\AODepthPreparation.h"

void AODepthPreparation::Initialize(Graphics::GraphicsDevice* device, uint32_t width, uint32_t height)
{
	D3D::DevicePtr pDevice = device->GetDevicePtr();
	{
		D3D11_BUFFER_DESC cbDesc = {};
		ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
		cbDesc.ByteWidth = sizeof(CBuffer);
		cbDesc.Usage = D3D11_USAGE_DEFAULT;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = 0;

		pDevice->CreateBuffer(&cbDesc, nullptr, mpCBuffer.GetAddressOf());
	}
	{

	}
}

void AODepthPreparation::Execute(Graphics::GraphicsDevice* device)
{

}
