#pragma once
#include <memory>

#include "./Renderer/D3D_Helper.h"

#include "./Utilities/Vector.h"

class CameraController;

class Light
{

protected:
    D3D::BufferPtr m_pConstantBuffer;

public:
	Light(){}
	virtual void Update(float elapsed_time) = 0;
	virtual void Set(D3D::DeviceContextPtr& p_imm_context, const std::shared_ptr<CameraController>& p_camera) = 0;
	virtual void RenderUI() = 0;
	virtual ~Light(){}
};