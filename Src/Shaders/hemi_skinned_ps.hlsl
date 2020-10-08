#include "phong.hlsli"
#include "Functions.hlsli"
Texture2D diffuseTexture : register(t0);
SamplerState decalSampler : register(s0);

static const float3 fogColor = { 0.8,0.8,0.6 };
static const float fogNear = 20.0;
static const float fogFar = 100.0;

static const float3 skyColor = { 0.4,0.6,0.8 };
static const float3 groundColor = { 0.6,0.4,0.2 };

float4 main(PS_Input input) : SV_TARGET
{
	float4 color = diffuseTexture.Sample(decalSampler, input.texcoord) * input.color;
	float3 N = normalize(input.w_normal);
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	float3 L = normalize(light_dir.xyz);

	float3 A = hemiSphereLight(N, skyColor, groundColor);

	float3 C = light_color.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	float3 Ks = float3(1, 1, 1);
	float Shininess = 20;
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, Shininess);

	color *= float4(A + D + S, 1.0);

	color.rgb = fog(color.rgb, eye_pos.xyz, input.w_pos, fogColor, fogNear, fogFar);

	return color;
}