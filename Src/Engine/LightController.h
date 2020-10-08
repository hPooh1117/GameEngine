#pragma once
#include <memory>
#include <vector>

#include "UIClient.h"

#include "./Renderer/D3D_Helper.h"

#include "./Utilities/Vector.h"

struct PointLightData
{
	unsigned int   index;
	float          range;
	bool           b_enable;
	Vector3        pos;
	Vector4        color;
};

struct SpotLightData
{
	unsigned int   index = 0;
	float          range = 0.0f;
	bool           b_enable = false;
	float          inner_corn = 0.99f;
	float          outer_corn = 0.8f;
	float          angle = 0.0f;
	float          height = -0.5f;
	Vector3        pos;
	Vector4        color;
	Vector3        dir;
};

struct PointLightGPU;
struct SpotLightGPU;
struct DirectilnalLightGPU;
class  CameraController;

class LightController : public UIClient
{
private:
	D3D::BufferPtr m_pConstantBuffer;

	// Directional Light Settings
	Vector4 mLightDirection;
	float   mAngle;
	Vector4 mAmbientColor = { 0.1f, 0.1f, 0.1f, 0 };
	Vector4 mLightColor = { 0.8f, 0.8f, 0.8f, 1.0f };
	
	float   mTessFactor = 1.0f;
	float   mEnvironmentMapAlpha;
	float   mTimer;

	bool    m_bUsingBRDF;
	float   mRoughness;
	float   mMetalness;

	// Point Light Settings
	std::vector<PointLightData> mPointLights;

	// Spot Light Settings
	std::vector<SpotLightData> mSpotLights;

	// Other Settings
	bool m_bSpotLightControllable;

public:
	LightController() = delete;
	LightController(D3D::DevicePtr& p_device);
	~LightController() = default;

	//--< CORE INTERFACE >--------------------------------------------------------------
	bool Init(unsigned int pointLightNum, unsigned int spotLightNum, bool isPBR = false);
	void Update(float elapsed_time);

private:
	void UpdateDirectionalLights(float elapsed_time);
	void UpdateSpotLights(float elapsed_time);

public:
	void ActivatePointLight(unsigned int index, bool b_activate);
	void ActivateSpotLight(unsigned int index, bool b_activate);


	//--< DATA INTERACTION >------------------------------------------------------------
	void SendLightData(
		PointLightGPU&       pointData, 
		SpotLightGPU&        spotLightData, 
		DirectilnalLightGPU& dirctionalData);
	
	//--< SETTER >----------------------------------------------------------------------
	void SetLightColor(const Vector4& color);

	void SetPointData(
		unsigned int        index, 
		const Vector3&      pos,
		const Vector4&      color, 
		float               range);

	void SetSpotData(
		unsigned int        index, 
		const Vector3&      pos,
		const Vector4&      color,
		const Vector3&      direction,
		float               range,
		float               nearCorn,
		float               farCorn);


	void SetDataForGPU(
		D3D::DeviceContextPtr& pImmContext, 
		std::shared_ptr<CameraController>& pCamera); // �����\��i�萔�o�b�t�@�͑��̏ꏊ�ō�肽���j

	//--< GETTER >----------------------------------------------------------------------
	inline const Vector4& GetLightDirection();

	//--< RENDER >----------------------------------------------------------------------
	void RenderUI();

};

const Vector4& LightController::GetLightDirection()
{
	return mLightDirection;
}