#include "H_ToGBuffer.hlsli"
#include "H_BumpMapping.hlsli"
//#include "H_2DSprite.hlsli"
#include "H_Functions.hlsli"
#include "H_DirectionalLight.hlsli"
#include "H_ShadowMap.hlsli"

// 参考資料
// First
// http://john-chapman-graphics.blogspot.com/2013/01/ssao-tutorial.html

// Second
// http://alextardif.com/SSAO.html
// https://learnopengl.com/Advanced-Lighting/SSAO
// http://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20101122
// https://enginetrouble.net/2016/10/reconstructing-world-position-from-depth-2016.html

#pragma warning(disable : 3571)
//--------------------------------------------
//	テクスチャ
//--------------------------------------------

Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

//--------------------------------------------
//	グローバル変数
//--------------------------------------------
cbuffer CBPerMatrix : register(b0)
{
	row_major float4x4 matWVP;
	row_major float4x4 world;

	float4 mat_color;

};


//----------------------------------
// データフォーマット
//----------------------------------
struct VS_Input
{
	float3 position : POSITION;
	float4 color    : COLOR;
	float2 texcoord      : TEXCOORD;
};

struct PS_Input
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float2 projPos : TEXCOORD1;
	float4 color : COLOR;
};

//--------------------------------------------
//	グローバル変数
//--------------------------------------------
static const float SAMPLING_RATIO = 2.0f;

//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// Vertex Shader(default)
//
PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1);


	output.position = P;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.projPos = output.texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	return output;
}


float3 DecodeSphereMap(float2 e)
{
	float2 tmp = e - e * e;
	float  f = tmp.x + tmp.y;
	float  m = sqrt(4.0 * f - 1.0);

	float3 n;
	n.xy = m * (e * 4.0 - 2.0);
	n.z = 3.0 - 8.0 * f;
	return n;
}


// Projection行列とスクリーンサイズを使って、ビュー空間の座標を算出
//float3 ComputePositionViewFromZ(uint2 coords, float zbuffer)
//{
//	float2 screenPixelOffset = float2(2.0, -2.0) / float2(screen_size.x, screen_size.y);
//	float2 positionScreen = (float2(coords.xy) + float2(0.5, 0.5)) * screenPixelOffset.xy + float2(-1.0, 1.0);
//	float  viewSpaceZ = proj._43 / (zbuffer - proj._33);
//
//	float2 screenSpaceRay = float2(positionScreen.x / proj._11, positionScreen.y / proj._22);
//	float3 positionView;
//	positionView.z = viewSpaceZ;
//	positionView.xy = screenSpaceRay.xy * positionView.z;
//
//	return positionView;
//}

//
// Pixel Shader
//
//PS_Output_SSAO PSmain(PS_Input input)
//{
//	PS_Output_SSAO output = (PS_Output_SSAO)0;
//
//	float depth = depth_texture.Sample(depth_sampler, input.texcoord).r;
//	float4 viewP = float4(input.texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), depth, 1.0f);
//	viewP = mul(viewP, inv_proj);
//	float4 wPos = mul(viewP, inv_view);
//	wPos /= wPos.w;
//	viewP /= viewP.w;
//
//	float3 P = wPos.xyz;
//	//float3 P = position_texture.Sample(position_sampler, input.texcoord).xyz;
//	float3 N = normal_texture.Sample(normal_sampler, input.texcoord).xyz;
//	float3 E = normalize(eye_pos.xyz - P);
//	float3 L = normalize(light_dir.xyz);
//
//	float3 A = /*ambient_color.rgb*/float3(0.8, 0.8, 0.8);
//
//	float3 C = light_color.rgb;
//	float3 Kd = float3(1, 1, 1);
//	float3 D = diffuse(N, L, C, Kd);
//
//	float3 Ks = float3(1, 1, 1);
//	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 30);
//
//	output.diffuse = float4(D, 1);
//	output.specular = float4(S, 1);
//
//	// 処理対象ピクセルのビュー座標系の位置を取得
//	//float4 viewP = mul(wPos, view);
//	//wP = mul(wP, view);
//	//viewP = position_texture.Sample(position_sampler, input.texcoord);
//
//	// 遮蔽されないポイントの数
//	int count = 0;
//
//	// 個々のサンプルについて
//	[unroll]
//	for (uint i = 0 ; i < MAX_SAMPLES ; ++i)
//	{
//		// サンプル点の位置をPから相対位置に平行移動し、クリッピング座標系上の位置qを得る。
//		float4 q = viewP + sample_pos[i];
//		//flaot4 q = wP + sample_pos[i]
//		q = mul(q, proj);
//
//		// テクスチャ座標系に変換
//		q = q * 0.5f / q.w + 0.5f;
//		float2 qXY = q.xy;
//		//q /= q.w;
//		//float2 qXY = (q.xy + float2(1, -1)) * float2(0.5f, -0.5f);
//
//		if ( q.z < depth_texture.Sample(depth_sampler, qXY).r ) ++count;
//	}
//
//	float a = clamp(float(count) * SAMPLING_RATIO / float(MAX_SAMPLES), 0.0f, 1.0f);
//	output.ambient = float4(A * (1.0 -a), 1);
//
//
//	return output;
//}

// 20201004
Texture2D      noise_texture : register(t8);
SamplerState   noise_sampler : register(s8);


float LinearizeDepth(float depth, float near, float far)
{
	return (2.0 * near) / (far + near - depth * (far - near));
}

PS_Output_SSAO PSmain(PS_Input input)
{
	PS_Output_SSAO output = (PS_Output_SSAO)0;

	float2 depthBufferColor = depth_texture.Sample(depth_sampler, input.texcoord).rg;
	float depth = depthBufferColor.r;
	//depth = LinearizeDepth(depth, 0.1, 100.0);
	float4 projP = float4(input.projPos.xy, depth, 1);
	float4 viewP = mul(projP, inv_proj);
	//viewP /= viewP.w;
	//float4 wPos = mul(viewP, inv_view);
	float4 wPos = mul(projP, inv_viewproj);
	wPos /= wPos.w;
	viewP /= viewP.w;

	float3 P = wPos.xyz;
	//float3 P = position_texture.Sample(position_sampler, input.texcoord).xyz;
	float3 N = normal_texture.Sample(normal_sampler, input.texcoord).xyz;
	float3 E = normalize(eye_pos.xyz - P);
	float3 L = normalize(light_dir.xyz);

	float3 A = /*ambient_color.rgb*/float3(0.8, 0.8, 0.8);

	float3 C = light_color.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);

	output.diffuse = float4(D, 1);
	output.specular = float4(S, 1);

	//float3 centerDepthPos = ComputePositionViewFromZ(uint2(input.projPos.xy), depth);
	float3 randomVector = noise_texture.Sample(noise_sampler, input.texcoord * noise_scale).xyz;
	float3 tangent = normalize(randomVector - N * dot(randomVector, N));
	float3 bitangent = cross(N, tangent);
	row_major float3x3 TBN = float3x3(tangent, bitangent, N);

	float occlusion = 0.0;

	for (unsigned int i = 0; i < (unsigned int)kernelSize; ++i)
	{
		float3 samplePos = mul(sample_pos[i].xyz, TBN);
		samplePos = samplePos * radius + viewP.xyz;

		float3 sampleDir = normalize(samplePos - viewP.xyz);

		float NdotS = max(dot(sampleDir, N), 0.0);

		float4 offset = mul(float4(samplePos, 1.0), proj);
		offset.xy /= offset.w;

		float sampleDepth = depth_texture.Sample(depth_sampler, float2(offset.x * 0.5 + 0.5, -offset.y * 0.5 + 0.5)).r;
		float4 sampleProjPos = float4(offset.xy, sampleDepth, 1);
		float4 sampleViewPos = mul(sampleProjPos, inv_proj);
		sampleDepth = sampleViewPos.z / sampleViewPos.w;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewP.z - sampleDepth));
		occlusion += rangeCheck * step(sampleDepth, samplePos.z) * NdotS;
	}

	occlusion = 1.0 - (occlusion / kernelSize);
	float finalOcclusion = pow(abs(occlusion), power);

	//output.ambient = float4(occlusion > 1 ? 1 : 0, 0, 0, 1.0);
	output.ambient = float4(finalOcclusion, finalOcclusion, finalOcclusion, 1.0);
	//output.ambient = float4(P.xyz, 1.0);
	//output.ambient = float4(centerDepthPos.xyz, 1.0);
	//output.ambient = float4(depth, 0, 0, 1.0);

	return output;
}
