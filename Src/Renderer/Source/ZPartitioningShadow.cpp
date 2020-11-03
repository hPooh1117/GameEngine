#include "ZPartitioningShadow.h"

#include "Shader.h"
#include "GraphicsEngine.h"
#include "./Utilities/misc.h"
#include "./Engine/OrthoView.h"
#include "./Engine/CameraController.h"
#include "./Engine/DirectionalLight.h"
#include "./Engine/LightController.h"
#include "./Application/Helper.h"
#include "Texture.h"

#include "./Utilities/ImguiSelf.h"
#include "./Utilities/Util.h"

using namespace DirectX;

ZPartitioningShadow::ZPartitioningShadow(D3D::DevicePtr& p_device)
	:mDistanceToLight(-40.0f)
{
	D3D11_BUFFER_DESC cbDesc = {};
	ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
	cbDesc.ByteWidth = sizeof(CBufferForShadow);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;

	p_device->CreateBuffer(&cbDesc, nullptr, m_pCBufferForShadow.GetAddressOf());

	mBoxForLight.min = Vector3(-50.0f, -50.0f, -50.0f);
	mBoxForLight.max = Vector3(50.0f, 50.0f, 50.0f);
}


void ZPartitioningShadow::Activate(
	const std::unique_ptr<GraphicsEngine>& p_graphics,
	LightController* p_light,
	CameraController* p_camera)
{
	D3D::DeviceContextPtr& immContext = p_graphics->GetImmContextPtr();

	Vector4 light = p_light->GetLightDirection();
	Vector3 lightDir = light.getXYZ();

	p_camera->SetOrthoPos(mDistanceToLight * lightDir);
	XMStoreFloat4x4(&mLightViewProj, p_camera->GetOrthoView() * p_camera->GetOrthoProj(immContext));

	CBufferForShadow cb = {};
	cb.shadowVP = mLightViewProj;
	immContext->UpdateSubresource(m_pCBufferForShadow.Get(), 0, nullptr, &cb, 0, 0);
	immContext->VSSetConstantBuffers(6, 1, m_pCBufferForShadow.GetAddressOf());
	immContext->PSSetConstantBuffers(6, 1, m_pCBufferForShadow.GetAddressOf());

}

void ZPartitioningShadow::Deactivate(std::unique_ptr<GraphicsEngine>& p_graphics)
{
}

void ZPartitioningShadow::RenderUI()
{
	using namespace ImGui;
	Text("Shadow");

	SliderFloat("Distance to light", &mDistanceToLight, -100.0f, -30.0f);


	//Image((void*)m_pShadowTex->GetShaderResourceV().Get(), ImVec2(320, 180)/*, ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT)*/);
	
}

void ZPartitioningShadow::ComputeShadowMatrixPSSM(CameraController* p_camera)
{
	//******************* ���C�g�̃r���[�s��Ǝˉe�s���ݒ� *******************//
	// Bounding Box��ݒ�i�L���X�^�[�����锠�j
	// Light�̎���p�A�A�X�y�N�g�䂩��Bouding Sphere���쐬
	// �J�X�P�[�h�T�C�Y���̕����ʒu�ƃV���h�E�}�b�v�s���ݒ�
	// ??? Bounding Sphere�ƃJ�X�P�[�h�̕����ʒu�̒����͂ǂ̂悤�ɁH�H


	//******************* �J������񂩂畽�s�������� *******************//
	Vector3 pos = p_camera->GetCameraPosition();

	Vector3 dir = p_camera->GetCameraTarget() - pos;
	dir.normalize();
}

// Parallel Split Shadow Maps �̏���
// cf):http://www.project-asura.com/program/d3d11/d3d11_009.html
void ZPartitioningShadow::ComputeSplitPositions(unsigned int split_count, float lambda, float near_clip, float far_clip, std::vector<float>& Zcoords)
{
	// �ŏ���near clip�܂ł̋�����ݒ�
	Zcoords.emplace_back(near_clip);

	if (split_count == 1)
	{
		Zcoords.emplace_back(far_clip);
		return;
	}

	float invM = 1.0f / static_cast<float>(split_count);

	// �[�����Z�͂͂���
	assert(near_clip != 0.0f);

	// (f / n ) �v�Z
	float fDivN = far_clip / near_clip;

	// ( f - n ) �v�Z
	float fSubN = far_clip - near_clip;


	// ���p�����X�L�[����K�p
	for (unsigned int i = 0; i < split_count; ++i)
	{
		// �ΐ������X�L�[���Ōv�Z (Logarithmic Splitting)
		float CiLog = near_clip * powf(fDivN, invM * i);
		
		// ��l�����X�L�[���Ōv�Z (Uniforme Splitting)
		float CiUni = near_clip;

		// ���Z���ʂ���`���
		Zcoords.emplace_back(Math::Lerp(CiUni, CiLog, lambda));
	}

	// �Ō��far clip �܂ł̋�����ݒ�
	Zcoords.emplace_back(far_clip);
}

ZPartitioningShadow::~ZPartitioningShadow()
{
}
