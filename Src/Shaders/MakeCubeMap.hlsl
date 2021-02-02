#include "HF_GlobalVariables.hlsli"
#include "H_Functions.hlsli"

struct VS_Input
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
	//float3 tangent : TANGENT;
	//float3 binormal : BINORMAL;
};

struct VS_InputS
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
	float4 bone_weights : WEIGHTS;
	uint4  bone_indices : BONES;
	//float3 tangent : TANGENT;
	//float3 binormal : BINORMAL;
};


struct GS_Input
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord :TEXCOORD;
	float3 w_pos : TEXCOORD2;
	float3 normal : TEXCOORD3;
	//float3 tangent : TEXCOORD4;
	//float3 binormal : TEXCOORD5;
};

struct PS_Input
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 w_normal : TEXCOORD1;
	float2 texcoord : TEXCOORD2;
	float3 w_pos    : TEXCOORD3;
	//float3 w_tangent : TEXCOORD2;
    //float3 w_binormal : TEXCOORD3;
	uint  RTIndex : SV_RenderTargetArrayIndex;
};




Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
GS_Input VSmain(VS_Input input)
{
	GS_Input output = (GS_Input)0;
	float4 P = float4(input.position, 1);

	output.position = mul(P, world);
	output.normal = normalize(mul(input.normal, (float3x3)world)).xyz;
	//output.tangent = normalize(mul(input.tangent, (float3x3)world)).xyz;
	//output.binormal = normalize(mul(input.binormal, (float3x3)world)).xyz;
	output.texcoord = input.texcoord;
	output.color = input.color * mat_color;
	output.w_pos = output.position.xyz;

	return output;
}

GS_Input VSmainS(VS_InputS input)
{
	GS_Input output = (GS_Input)0;
	float4 P = float4(input.position, 1);

	output.position = mul(P, world);
	output.normal = normalize(mul(input.normal, (float3x3)world)).xyz;
	//output.tangent = normalize(mul(input.tangent, (float3x3)world)).xyz;
	//output.binormal = normalize(mul(input.binormal, (float3x3)world)).xyz;
	output.texcoord = input.texcoord;
	output.color = input.color * mat_color;
	output.w_pos = output.position.xyz;

	return output;
}


GS_Input VSmainSkinning(VS_InputS input)
{
	GS_Input output = (GS_Input)0;

	// スキニング処理
	float4 p = { 0, 0, 0, 0 };
	float4 n = { 0, 0, 0, 0 };
	//float4 t = { 0, 0, 0, 0 };
	//float4 b = { 0, 0, 0, 0 };
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		p += (input.bone_weights[i] * mul(float4(input.position, 1), bone_transforms[input.bone_indices[i]]));
		n += (input.bone_weights[i] * mul(float4(input.normal, 0), bone_transforms[input.bone_indices[i]]));
		//t += (input.bone_weights[i] * mul(float4(input.tangent, 0), bone_transforms[input.bone_indices[i]]));
		//b += (input.bone_weights[i] * mul(float4(input.binormal, 0), bone_transforms[input.bone_indices[i]));
	}
	p.w = 1.0;
	n.w = 0.0;
	//t.w = 0.0;
	//b.w = 0.0;

	float4 N = normalize(mul(n, world));
	//float4 T = normalize(mul(t, world));
	//float4 B = normalize(mul(b, world));

	


	output.color = input.color * mat_color;

	output.position = mul(p, world);
	output.texcoord = input.texcoord;
	output.normal = N.xyz;
	//output.tangent = T.xyz;
	//output.binormal = B.xyz:
	output.w_pos = output.position.xyz;

	return output;

}



[maxvertexcount(18)]
void GSmain(
	triangle GS_Input input[3],
	inout TriangleStream<PS_Input> output)
{
	for (int f = 0; f < 6; ++f)
	{
		PS_Input element = (PS_Input)0;
		element.RTIndex = f;
		for (int i = 0; i < 3; ++i)
		{
			float4 P = mul(input[i].position, view[f]);
			element.position = mul(P, proj);
			element.w_normal = normalize(input[i].normal).xyz;
			//element.w_tangent = normalize(input[i].tangent).xyz;
			//element.w_binormal = normalize(input[i].binormal).xyz;
			element.color = input[i].color;
			element.texcoord = input[i].texcoord;
			element.w_pos = input[i].position.xyz;
			output.Append(element);
		}
		output.RestartStrip();
	}
}

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 PSmain(PS_Input input) : SV_TARGET
{
	float3 P = input.w_pos;
	float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);
	float3 N = normalize(input.w_normal);
	//float3 T = normalize(input.w_tangent);
	//float3 B = normalize(input.w_binormal);

	float3 E = normalize(eye_pos.xyz - input.w_pos);
	float3 L = normalize(light_dir.xyz);

	float3 A = ambient_color.rgb;

	float3 C = light_color.rgb;
	float3 Kd = { 1,1,1 };
	float3 D = diffuse(N, L, C, Kd);

	float3 Ks = { 0.5, 0.5, 0.5 };
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);

	////// ポイントライト
	////float3 PL;
	////float3 PC;
	////float influence;
	////float3 PD = (float3)0;
	////float3 PS = (float3)0;

	////int i;
	////[unroll]
	////for (i = 0; i < POINTMAX; ++i)
	////{
	////	if (point_lights[i].type == 0) continue;
	////	PL = P - point_lights[i].pos.xyz;
	////	float d = length(PL);

	////	float r = point_lights[i].range;
	////	if (d > r) continue;

	////	influence = saturate(1.0f - d / r);
	////	PL = normalize(PL);
	////	PC = point_lights[i].color.rgb;
	////	PD += diffuse(N, PL, PC, Kd) * influence * influence;
	////	PS += blinnPhongSpecular(N, PL, PC, E, Ks, 20) * influence * influence;
	////}

	//////スポットライト
	////float3 SpotLight;
	////float3 SpotLightColor;
	////float3 SpotDiffuse = (float3)0;
	////float3 SpotSpecular = (float3)0;

	////[unroll]
	////for (i = 0; i < SPOTMAX; ++i)
	////{
	////	if (spot_lights[i].type == 0.0f) continue;

	////	SpotLight = P - spot_lights[i].pos.xyz;
	////	float d = length(SpotLight);

	////	float r = spot_lights[i].range;
	////	if (d > r) continue;

	////	float3 SpotFront = normalize(spot_lights[i].dir.xyz);
	////	SpotLight = normalize(SpotLight);

	////	float angle = dot(SpotLight, SpotFront);
	////	float area = spot_lights[i].inner_corn - spot_lights[i].outer_corn;

	////	float influence2 = spot_lights[i].inner_corn - angle;
	////	influence2 = saturate(1.0 - influence2 / area);
	////	influence = 1;
	////	SpotLightColor = spot_lights[i].color.rgb;
	////	SpotDiffuse += diffuse(N, SpotLight, SpotLightColor, Kd) * influence * influence * influence2;
	////	SpotSpecular += blinnPhongSpecular(N, SpotLight, SpotLightColor, E, Ks, 20) * influence * influence * influence2;
	////}


	color *= input.color * float4(A + D + S, 1.0);
	return color;
}