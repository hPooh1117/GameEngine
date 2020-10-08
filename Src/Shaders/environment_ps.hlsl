#include "environment.hlsli"
#include "Functions.hlsli"
//--------------------------------------------------
// Texture
//--------------------------------------------------
Texture2D diffuseTexture : register(t0);
SamplerState decalSampler : register(s0);

Texture2D environmentTexture : register(t1);
SamplerState environmentSampler : register(s1);

// EntryPoint----------------------------------------
float4 main(PS_Input input) : SV_TARGET
{
	float4 color = diffuseTexture.Sample(decalSampler, input.texcoord);
	float3 N = normalize(input.w_normal);
	float3 E = normalize(input.w_eye_dir);
	float3 L = normalize(light_dir.xyz);

	// Ambient Light
	float3 A = ambient_color.rgb;

	// Diffuse Light
	float3 C = light_color.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	// Specular Light
	float3 Ks = float3(1, 1, 1);
	float Shininess = 20;
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, Shininess);

	// Environment Map
	float3 ref = reflect(-E, N);
	ref = ref * 0.5 + 0.5;
	ref.y = -ref.y;
	float4  envColor = environmentTexture.Sample(environmentSampler, ref.xy);
	color = color * (1.0 - env_alpha) + envColor * env_alpha;

	color *= input.color * float4(A + D + S, 1.0f);

	return color;
}