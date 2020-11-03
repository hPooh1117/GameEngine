#include "H_Functions.hlsli"
#include "H_DataFormat.hlsli"
#include "H_DirectionalLight.hlsli"


Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

//----------------------------------
// グローバル変数
//----------------------------------


static const float LW = 0.02;				//輪郭線の太さ(モデルサイズ無視)
static const float4 LC = { 0.2, 0.2, 0.2, -1 };	//アルファ値が負の値の時アウトライン

//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1);
	output.position = P;

	float3 N = input.normal;
	N = normalize(N);

	float4 wP = mul(P, world);

	output.color = input.color;
	output.texcoord = input.texcoord;
	output.w_normal = N;
	output.w_pos = wP.xyz;

	return output;
}

//
// vertex shader(SkinnedMesh)
//
PS_Input VSmainS(VS_InputS input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1);
	output.position = P;

	float3 N = input.normal;
	N = normalize(N);

	float4 wP = mul(P, world);

	output.color = input.color;
	output.texcoord = input.texcoord;
	output.w_normal = N;
	output.w_pos = wP.xyz;

	return output;
}


PS_Input VSmainSkinning(VS_InputS input)
{
	PS_Input output = (PS_Input)0;


	// スキニング処理
	float4 p = { 0, 0, 0, 0 };
	float4 n = { 0, 0, 0, 0 };
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		p += (input.bone_weights[i] * mul(float4(input.position, 1), bone_transforms[input.bone_indices[i]]));
		n += (input.bone_weights[i] * mul(float4(input.normal, 0), bone_transforms[input.bone_indices[i]]));
	}
	p.w = 1.0f;
	n.w = 0.0f;

	output.position = p;

	float3 N = n.xyz;
	N = normalize(N);

	float4 wP = mul(p, world);

	output.color = input.color;
	output.texcoord = input.texcoord;
	output.w_normal = N;
	output.w_pos = wP.xyz;

	return output;
}


//
// geometry shader
//
[maxvertexcount(6)]
void GSmain(triangle GS_Input input[3],
	uint primID : SV_PrimitiveID,
	inout TriangleStream<PS_Input> Stream)
{
	for (int i = 0; i < 3; ++i)
	{
		PS_Input data;
		data.position = mul(input[i].position, matWVP);
		//data.position = mul(input[i].position, matWVP);
		data.w_normal = normalize(mul(input[i].normal, (float3x3)world));

		data.texcoord = input[i].texcoord;
		data.color = input[i].color;
		data.w_pos = input[i].w_pos;
		Stream.Append(data);
	}
	Stream.RestartStrip();

	{
		PS_Input data;
		float4 P = input[0].position + float4(input[0].normal, 0) * LW;
		data.position = mul(P, matWVP);
		data.w_normal = -mul(input[0].normal, (float3x3)world);
		data.texcoord = input[0].texcoord;
		data.color = LC;
		data.w_pos = input[0].w_pos;
		Stream.Append(data);

		P = input[2].position + float4(input[2].normal, 0) * LW;
		data.position = mul(P, matWVP);
		data.w_normal = -mul(input[2].normal, (float3x3)world);
		data.texcoord = input[2].texcoord;
		data.color = LC;
		data.w_pos = input[2].w_pos;
		Stream.Append(data);

		P = input[1].position + float4(input[1].normal, 0) * LW;
		data.position = mul(P, matWVP);
		data.w_normal = -mul(input[1].normal, (float3x3)world);
		data.texcoord = input[1].texcoord;
		data.color = LC;
		data.w_pos = input[1].w_pos;
		Stream.Append(data);


	}
	Stream.RestartStrip();
}


//
// pixel shader
//
float4 PSmain( PS_Input input ) : SV_TARGET
{
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	float3 L = normalize(light_dir.xyz);
	float3 N = normalize(input.w_normal);

	float3 A = ambient_color.rgb;
	float3 C = light_color.rgb;

	float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);

	float3 Kd = float3(1, 1, 1);
	float3 D = ToonDiffuse(N, L, C, Kd);

	float3 Ks = float3(1, 1, 1);
	float3 S = ToonSpecular(N, L, C, E, Ks, 20);

	if (input.color.a < 0) return float4(input.color.rgb, 1);
	return color * input.color * float4(A + D + S, 1.0);
}


