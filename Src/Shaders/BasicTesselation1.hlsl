#include "H_DirectionalLight.hlsli"
#include "H_DataFormat.hlsli"
#include "H_Functions.hlsli"

//--------------------------------------------
//	ハルシェーダコンスタント関数
//--------------------------------------------
HSConstantOutput HSConstant(
	InputPatch<HS_Input, 3> ip,
	uint pid : SV_PrimitiveID
)
{
	HSConstantOutput output;
	output.factor[0] = tess;
	output.factor[1] = tess;
	output.factor[2] = tess;
	output.inner_factor = tess;
	return output;
}

//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
HS_Input VSmain(VS_Input input)
{
	HS_Input output = (HS_Input)0;

	// 出力値設定
	float4 P = float4(input.position, 1.0);
	float3 wP = mul(P, world).xyz;

	float3 N = mul(input.normal, (float3x3)world);

	output.position = P.xyz;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.normal = N;
	output.w_pos = wP;
	return output;
}

HS_Input VSmainS(VS_InputS input)
{
	HS_Input output = (HS_Input)0;

	// 出力値設定
	float4 P = float4(input.position, 1.0);
	float3 wP = mul(P, world).xyz;

	float3 N = mul(input.normal, (float3x3)world);

	output.position = P.xyz;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.normal = N;
	output.w_pos = wP;
	return output;
}


//
// Hull Shader
//
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HSConstant")]
DS_Input HSmain(InputPatch<HS_Input, 3> input,
	uint cpid : SV_OutputControlPointID,
	uint pid : SV_PrimitiveID)
{
	DS_Input output = (DS_Input)0;
	output.position = input[cpid].position;
	output.color = input[cpid].color;
	output.texcoord = input[cpid].texcoord;
	output.normal = input[cpid].normal;
	output.w_pos = input[cpid].w_pos;
	return output;
}


//
// Domain Shader
//
[domain("tri")]
GS_Input DSmain(HSConstantOutput input,
	float3 UV : SV_DomainLocation,
	const OutputPatch<DS_Input, 3> patch)
{
	GS_Input output = (GS_Input)0;

	// 頂点座標
	float3 pos = patch[0].position * UV.x + patch[1].position * UV.y + patch[2].position * UV.z;
	float4 P = mul(float4(pos, 1), matWVP);
	output.position = P;

	// 頂点ワールド位置
	float3 wPos = patch[0].w_pos * UV.x + patch[1].w_pos * UV.y + patch[2].w_pos * UV.z;
	output.w_pos = wPos;

	// uv座標
	float2 tex = patch[0].texcoord * UV.x + patch[1].texcoord * UV.y + patch[2].texcoord * UV.z;
	output.texcoord = tex;

	// 法線座標
	float3 N = patch[0].normal * UV.x + patch[1].normal * UV.y + patch[2].normal * UV.z;
	output.normal = N;

	// 頂点色
	float4 C = patch[0].color * UV.x + patch[1].color * UV.y + patch[2].color * UV.z;
	output.color = C;

	return output;
}

//
// Geometry Shader
//
[maxvertexcount(3)]
void GSmain(
	triangle GS_Input input[3],
	uint id : SV_PrimitiveID,
	inout TriangleStream<PS_Input> TriStream)
{
	PS_Input output = (PS_Input)0;

	for (int v = 0; v < 3; ++v)
	{
		output.w_normal = input[v].normal;
		output.position = input[v].position;
		output.w_pos = input[v].w_pos;
		output.texcoord = input[v].texcoord;
		output.color = input[v].color;
		TriStream.Append(output);
	}
	TriStream.RestartStrip();
}

//
// Pixel Shader
//
float4 PSmain(PS_Input input) : SV_TARGET
{
	float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);
	float3 N = normalize(input.w_normal);
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	float3 L = normalize(light_dir.xyz);

	float3 A = ambient_color.rgb;

	float3 C = light_color.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);

	color *= input.color * float4(A + D + S, 1.0f);
	return color;


}