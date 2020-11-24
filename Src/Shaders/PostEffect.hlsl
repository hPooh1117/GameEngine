#include "H_2DSprite.hlsli"

//--------------------------------------------
//	テクスチャ
//--------------------------------------------

Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

static const float GAMMA_CORRECTION = 2.2;
static const float CONTRAST = 1.4;
static const float3 GRAY_SCALE = float3(0.299, 0.587, 0.114);

cbuffer CBPerPostProcess : register(b1)
{
	float3 correctedColor;
	float contrast;
	float saturation;
	float vignette;
	float dummy0;
	float dummy1;
};

//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	output.position = float4(input.position, 1);
	output.color = input.color;
	output.texcoord = input.texcoord;
	return output;
}

//
// pixel shader
//
float4 PSmain(PS_Input input) : SV_TARGET
{
	float3 color = diffuse_texture.Sample(decal_sampler, input.texcoord).rgb * input.color.rgb;

	// コントラスト
	color = contrast + (color - contrast) * CONTRAST;

	// 彩度
	float gray = dot(color, GRAY_SCALE);
	color = lerp(float3(gray, gray, gray), color, saturation);

	// カラー補正
	color *= correctedColor;

	// ビネット
	float2 uv = input.texcoord - 0.5;
	float d = length(uv);
	d += vignette;
	d = pow(d, 2.0);
	color = lerp(color, float3(0, 0, 0), d);
	

	// ガンマ補正
	//color = pow(abs(color.rgb), 1 / GAMMA_CORRECTION);

	float4 result = float4(color, 1.0);
	//float4 result = float4(color, 1);
	return result;
}