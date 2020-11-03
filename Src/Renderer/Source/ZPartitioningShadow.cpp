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
	//******************* ライトのビュー行列と射影行列を設定 *******************//
	// Bounding Boxを設定（キャスターが入る箱）
	// Lightの視野角、アスペクト比からBouding Sphereを作成
	// カスケードサイズ分の分割位置とシャドウマップ行列を設定
	// ??? Bounding Sphereとカスケードの分割位置の調整はどのように？？


	//******************* カメラ情報から平行分割処理 *******************//
	Vector3 pos = p_camera->GetCameraPosition();

	Vector3 dir = p_camera->GetCameraTarget() - pos;
	dir.normalize();
}

// Parallel Split Shadow Maps の準備
// cf):http://www.project-asura.com/program/d3d11/d3d11_009.html
void ZPartitioningShadow::ComputeSplitPositions(unsigned int split_count, float lambda, float near_clip, float far_clip, std::vector<float>& Zcoords)
{
	// 最初はnear clipまでの距離を設定
	Zcoords.emplace_back(near_clip);

	if (split_count == 1)
	{
		Zcoords.emplace_back(far_clip);
		return;
	}

	float invM = 1.0f / static_cast<float>(split_count);

	// ゼロ除算ははじく
	assert(near_clip != 0.0f);

	// (f / n ) 計算
	float fDivN = far_clip / near_clip;

	// ( f - n ) 計算
	float fSubN = far_clip - near_clip;


	// 実用分割スキームを適用
	for (unsigned int i = 0; i < split_count; ++i)
	{
		// 対数分割スキームで計算 (Logarithmic Splitting)
		float CiLog = near_clip * powf(fDivN, invM * i);
		
		// 一様分割スキームで計算 (Uniforme Splitting)
		float CiUni = near_clip;

		// 演算結果を線形補間
		Zcoords.emplace_back(Math::Lerp(CiUni, CiLog, lambda));
	}

	// 最後はfar clip までの距離を設定
	Zcoords.emplace_back(far_clip);
}

ZPartitioningShadow::~ZPartitioningShadow()
{
}
