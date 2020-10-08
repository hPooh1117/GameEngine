#include "H_DataFormat.hlsli"
#include "H_PointLight.hlsli"
#include "H_Functions.hlsli"
//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
GS_Input VSmain(VS_Input input)
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
GS_Input VSmainS(VS_InputS input)
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


GS_Input VSmainSkinning(VS_InputS input)
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
[maxvertexcount(3)]
void GSmain(triangle GS_Input input[3],
	uint primID : SV_PrimitiveID,
	inout TriangleStream<PS_Input> Stream)
{
	float3 v1 = input[1].position.xyz - input[0].position.xyz;
	float3 v2 = input[2].position.xyz - input[0].position.xyz;
	float3 N = normalize(cross(v1, v2));

	for (int i = 0; i < 3; ++i)
	{
		PS_Input data = (PS_Input)0;
		data.position = mul(input[i].position, matWVP);
		data.w_normal = normalize(mul(N, (float3x3)world));
		data.texcoord = input[i].texcoord;
		data.color = input[i].color;
		data.w_pos = input[i].w_pos;
		Stream.Append(data);
	}
	Stream.RestartStrip();
}


//
// pixel shader
//
float4 PSmain(PS_Input input) : SV_TARGET
{
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	float3 L = normalize(light_dir.xyz);
	float3 N = normalize(input.w_normal);

	float3 A = ambient_color.rgb;
	float3 C = light_color.rgb;

	float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);

	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);

	//ポイントライト
	float3 PL;
	float3 PC;
	float influence;
	float3 PD = (float3)0;
	float3 PS = (float3)0;
	for (int i = 0; i < POINTMAX; i++) {
		if (point_lights[i].type == 0) continue;
		PL = input.w_pos - point_lights[i].pos.xyz;
		float d = length(PL);

		float r = point_lights[i].range;
		if (d > r) continue;
		influence = saturate(1.0f - d / r);
		PL = normalize(PL);
		PC = point_lights[i].color.rgb;
		PD += diffuse(N, PL, PC, Kd) * influence * influence;
		PS += blinnPhongSpecular(N, PL, PC, E, Ks, 20) * influence * influence;
	}


	if (input.color.a < 0) return float4(input.color.rgb, 1);
	return color * input.color * float4(A + D + S + PD + PS, 1.0);
}


