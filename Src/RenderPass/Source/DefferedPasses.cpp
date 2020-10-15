#include "DefferedPasses.h"

#include "./Renderer/VertexDecleration.h"

void DefferedPasses::Initialize(D3D::DevicePtr& device)
{
	AddVertexAndPixelShader(device, "Phong", L"Phong.hlsl", L"Phong.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);

}

void DefferedPasses::InitializeGBuffer()
{
}
