#include "ForwardPasses.h"

#include "./Renderer/Shader.h"


void ForwardPass::Initialize(D3D::DevicePtr& device)
{
	AddVertexAndPixelShader(device, "Phong", L"Phong.hlsl", L"Phong.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);

	AddVertexAndPixelShader(device, "Line", L"line.hlsl", L"line.hlsl", "VSMainDuplicate", "PSmain", VEDType::VED_GEOMETRIC_PRIMITIVE);
	AddGeometryShader(device, "Line", L"line.hlsl", "GSmain");

	AddVertexAndPixelShader(device, "EnvironmentMap", L"EnvironmentMap.hlsl", L"EnvironmentMap.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);

	AddVertexAndPixelShader(device, "SkyBox", L"SkyBox.hlsl", L"SkyBox.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);

	AddVertexAndPixelShader(device, "NormalMap", L"NormalMap.hlsl", L"NormalMap.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);

	AddVertexAndPixelShader(device, "ToonGeo", L"ToonGeo.hlsl", L"ToonGeo.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddGeometryShader(device, "ToonGeo", L"ToonGeo.hlsl", "GSmain");

	AddVertexAndPixelShader(device, "Sprite", L"FromGBuffer.hlsl", L"FromGBuffer.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);

	AddVertexAndPixelShader(device, "ForwardPBR", L"ForwardPBR.hlsl", L"ForwardPBR.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
}

void ForwardPass::RenderForwardLighting()
{
}
