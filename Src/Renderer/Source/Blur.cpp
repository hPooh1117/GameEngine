#include "..\Blur.h"


#include "Texture.h"

#include "./Application/Helper.h"

Blur::Blur(D3D::DevicePtr& p_device)
{
	mpPass0Tex = std::make_unique<Texture>();
	mpPass0Tex->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

	mpPass1Tex = std::make_unique<Texture>();
	mpPass1Tex->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

}

Blur::~Blur()
{
}

void Blur::ActivateBlurPass()
{
}

void Blur::RenderBlur()
{
}

void Blur::Deactivate()
{
}
