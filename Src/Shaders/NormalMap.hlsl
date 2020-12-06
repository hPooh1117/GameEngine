#include "H_Functions.hlsli"
#include "H_DataFormat.hlsli"
#include "H_BumpMapping.hlsli"
#include "H_DirectionalLight.hlsli"


Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

Texture2D normal_texture : register(t1);
SamplerState normal_sampler : register(s1);

Texture2D height_texture : register(t2);
SamplerState height_sampler : register(s2);


//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
PS_InputBump VSmain(VS_Input input)
{
	PS_InputBump output = (PS_InputBump)0;

	float4 P = float4(input.position, 1.0);

	output.position = mul(P, matWVP);

	float3 wPos = mul(P, world).xyz;
	
	float4 N = float4(input.normal, 0.0f);
	float3 wN = mul(N, world).xyz;
	wN = normalize(wN);

	float3 vN = wN;
	float3 vB = { 0, 1, -0.001f };
	float3 vT;
	vB = normalize(vB);
	vT = normalize(cross(vB, vN));
	vB = normalize(cross(vN, vT));
		

	output.color = input.color;

	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_pos = wPos;

	output.v_tan = vT;
	output.v_bin = vB;
	output.v_normal = vN;

	return output;
}

float4 PSmain(PS_InputBump input) : SV_TARGET
{
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	float3 L = normalize(light_dir.xyz);

	float3 A = ambient_color.rgb;
	float3 C = light_color.rgb;

	float3 vx = normalize(input.v_tan);
	float3 vy = normalize(input.v_bin);
	float3 vz = normalize(input.v_normal);

	float2 texcoord = input.texcoord;

	float3x3 inv_mat = { vx, vy, vz };
	float3 inv_E = normalize(mul(inv_mat, E));

	float H = height_texture.Sample(height_sampler, texcoord).r;
	H = H * 2.0f - 1.0f;
	texcoord.x -= inv_E.x * H * viewOffset;
	texcoord.y -= inv_E.y * H * viewOffset;

	float4 color = diffuse_texture.Sample(decal_sampler, texcoord);
	float3 N = normal_texture.Sample(decal_sampler, texcoord).xyz;
	N = N * 2.0f - 1.0f;

	float3x3 mat = { vx, vy, vz };
	N = normalize(mul(N, mat));

	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, shininess);

	return color * input.color * float4(A + D + S, 1.0);
}