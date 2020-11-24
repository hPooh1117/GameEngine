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
	float   mPitch;
	float   mYaw;
	Vector4 mAmbientColor = { 0.1f, 0.1f, 0.1f, 0 };
	Vector4 mLightColor = { 0.9f, 0.9f, 0.9f, 1.0f };
	
	float   mTessFactor = 1.0f;
	float   mEnvironmentMapAlpha;
	float   mTimer;


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

	bool Init(unsigned int pointLightNum, unsigned int spotLightNum);
	void Update(float elapsed_time);

private:
	void UpdateDirectionalLights(float elapsed_time);
	void UpdateSpotLights(float elapsed_time);

public:
	void ActivatePointLight(unsigned int index, bool b_activate);
	void ActivateSpotLight(unsigned int index, bool b_activate);

	

	void SendLightData(
		PointLightGPU&       pointData, 
		SpotLightGPU&        spotLightData, 
		DirectilnalLightGPU& dirctionalData);
	
	void SetLightColor(const Vector4& color) { mLightColor = color; }
	void SetLightColor(const Vector3& color) { SetLightColor(color.x, color.y, color.z); }
	void SetLightColor(float r, float g, float b) { mLightColor.x = r, mLightColor.y = g, mLightColor.z = b; }

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
		CameraController* pCamera); // 消去予定（定数バッファは他の場所で作りたい）

	inline const Vector4& GetLightDirection();

	void RenderUI();

};

const Vector4& LightController::GetLightDirection()
{
	return mLightDirection;
}