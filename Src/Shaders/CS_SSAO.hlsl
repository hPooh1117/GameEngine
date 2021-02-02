// src:
// http://www.audentia-gestion.fr/NVIDIA/PDF/High_Quality_Graphic_Effects_using_DX11_en.pdf
// https://github.com/turanszkij/WickedEngine

#include "H_CS_Global.hlsli"

Texture2D<float4>      depth_texture : register(t0);
Texture2D<float4>      normal_texture : register(t1);
Texture2D<float4>      noise_texture : register(t9);
SamplerState           clamp_sampler : register(s1);
SamplerState           noise_sampler : register(s9);
RWTexture2D<float4>    outputTexture : register(u0);

static const uint BLOCK_SIZE_X = 32;
static const uint BLOCK_SIZE_Y = 16;

static const uint TILE_BORDER = 1;
static const uint TILE_SIZE = BLOCK_SIZE_X + TILE_BORDER * 2;
static const uint MAX_SAMPLES = 16;





cbuffer CBPerAO : register(b5)
{
	row_major float4x4 inv_proj;
	row_major float4x4 inv_view;
	row_major float4x4 proj;
	row_major float4x4 view;

	float2   screenSize;
	float    radius;
	float    power;

	float    kernelSampleSize;
	float    cameraNearZ;
	float    cameraFarZ;
	float    kernelSampleSize_rcp;

	float2   screenSize_rcp;
	float2   noise_scale;
	float4   sample_pos[MAX_SAMPLES];
}




[numthreads(BLOCK_SIZE_X, BLOCK_SIZE_Y, 1)]
void CSmain(
	uint3 dispatchTid : SV_DispatchThreadID,
	uint3 Gid : SV_GroupID,
	uint3 GTid : SV_GroupThreadID,
	uint groupIndex : SV_GroupIndex)
{
	const float depth = depth_texture[dispatchTid.xy].r;
	if (depth < 0.1)
	{
		outputTexture[dispatchTid.xy] = float4(1, 1, 1, 1);
		return;
	}


	// depth bufferからflat normalsを再構築
	// ・再構築されたpositionは共有メモリに保存。そこから三角形メッシュを作るが、アーティファクトが生じる（深度の不連続性によるもの）
	// ・アーティファクトを直すために、４つのpositionを中心からとり、不連続性の最も少ない深度からpositionを計算することで最適な三角形メッシュを探す。(NormalMapが無効かされてしまう)
	//// ・src: https://stackoverflow.com/questions/37627254/how-to-reconstruct-normal-from-depth-without-artifacts-on-edge

	float2 uv = (float2)dispatchTid.xy;
	uv *= screenSize_rcp;
	const float3 P = ReconstructPosition(uv, depth, inv_proj);
	const float3 normal = /*normalize(cross(p2 - P, p1 - P));*/normalize(normal_texture[dispatchTid.xy].xyz);

	//const float2 uv = (dispatchTid.xy + 0.5) * screenSize_rcp;

	float seed = 1.0;
	const float3 noise = float3(Rand(seed, uv), Rand(seed, uv), Rand(seed, uv)) * 2 - 1;

	const float3 tangent = normalize(noise - normal * dot(noise, normal));
	const float3 binormal = cross(normal, tangent);
	const row_major float3x3 tangentSpace = float3x3(tangent, binormal, normal);

	float ao = 0;
	[unroll]
	for (uint i = 0; i < MAX_SAMPLES; ++i)
	{
		float3 samplePos = mul(sample_pos[i].xyz, tangentSpace);
		samplePos = samplePos * radius + P.xyz;

		float3 sampleDir = normalize(samplePos - P.xyz);
		const float NdotS = saturate(dot(sampleDir, normal));

		float4 vProjectedCoord = mul(float4(samplePos, 1.0), proj);
		vProjectedCoord.xyz /= vProjectedCoord.w;
		vProjectedCoord.xy = vProjectedCoord.xy * float2(0.5, -0.5) + float2(0.5, 0.5);

		float sampleDepth = depth_texture.SampleLevel(clamp_sampler, vProjectedCoord.xy, 0).r;
		const float4 sampleProjPos = float4(vProjectedCoord.xy, sampleDepth, 1);
		const float4 sampleViewPos = mul(sampleProjPos, inv_proj);
		sampleDepth = sampleViewPos.z / sampleViewPos.w;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(P.z - sampleDepth));
		ao += rangeCheck * step(sampleDepth, samplePos.z) * NdotS;
	}
	ao *= kernelSampleSize_rcp;
	ao = pow(saturate(1 - ao), power);
	outputTexture[dispatchTid.xy] = float4(ao, ao, ao, 1);
}
