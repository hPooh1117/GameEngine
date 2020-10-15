#pragma once
#include <memory>
#include <array>

#include "D3D_Helper.h"

class Shader;
class Texture;

class Blur
{
	std::unique_ptr<Texture> mpPass0Tex;
	std::unique_ptr<Texture> mpPass1Tex;

	std::array<std::shared_ptr<Shader>, 2> mpShaders;

public:
	Blur(D3D::DevicePtr& p_device);
	~Blur();

	void ActivateBlurPass();
	void RenderBlur();
	void Deactivate();
};