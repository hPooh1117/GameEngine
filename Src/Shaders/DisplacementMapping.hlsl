#include "H_DataFormat.hlsli"
#include "H_BumpMapping.hlsli"
#include "H_Functions.hlsli"
#include "H_DirectionalLight.hlsli"


//--------------------------------------------------
// グローバル変数
//--------------------------------------------------
static const float height = 0.06;

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
// Vertex Shader(default)
//
HS_Input VSmain(VS_Input input)
{
	HS_Input output = (HS_Input)0;
	output.position = input.position;
	output.normal = input.normal;
	output.texcoord = input.texcoord;
	output.color = float4(1, 1, 1, 1);
	output.w_pos = input.position;
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
DS_Input HSmain(
	InputPatch<HS_Input, 3> input,
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
GS_InputBump DSmain(
	HSConstantOutput input,
	float3 UV : SV_DomainLocation,
	const OutputPatch<DS_Input, 3> patch)
{
	GS_InputBump output = (GS_InputBump)0;

	// 頂点色
	float4 C = patch[0].color * UV.x + patch[1].color * UV.y + patch[2].color * UV.z;

	// UV座標
	float2 tex = patch[0].texcoord * UV.x + patch[1].texcoord * UV.y + patch[2].texcoord * UV.z;

	// ローカル法線取得
	float3 N = patch[0].normal * UV.x + patch[1].normal * UV.y + patch[2].normal * UV.z;

	// ワールド法線取得
	float3 wN = normalize(mul(N, (float3x3)world));

	// ワールド接空間
	float3 vN = wN;
	float3 vB = { 0, 1, -0.001f };
	float3 vT;
	vB = normalize(vB);
	vT = normalize(cross(vB, vN));
	vB = normalize(cross(vN, vT));

	// ハイトマップ
	float H = height_texture.SampleLevel(height_sampler, tex, 0).x;

	// 頂点座標
	float3 pos = patch[0].position * UV.x + patch[1].position * UV.y + patch[2].position * UV.z;
	float4 P = float4(pos, 1.0);
	P.xyz += N * H * height;

	// 情報設定
	output.position = mul(P, matWVP);
	output.w_pos = mul(P, world).xyz;
	output.color = C;
	output.texcoord = tex;

	// ワールド接空間軸(ライティング)
	output.v_tan = vT;
	output.v_bin = vB;
	output.v_normal = vN;

	return output;
}



//
// Geometry Shader
//
[maxvertexcount(3)]
void GSmain(
	triangle GS_InputBump input[3],
	uint id : SV_PrimitiveID,
	inout TriangleStream<PS_InputBump> TriStream)
{
	PS_InputBump output = (PS_InputBump)0;
	for (int v = 0; v < 3; ++v)
	{
		output.position = input[v].position;
		output.w_pos = input[v].w_pos;
		output.texcoord = input[v].texcoord;
		output.color = input[v].color;

		output.v_tan = input[v].v_tan;
		output.v_bin = input[v].v_bin;
		output.v_normal = input[v].v_normal;

		TriStream.Append(output);

	}
	TriStream.RestartStrip();
}


//
// Pixel Shader
//
float4 PSmain(PS_InputBump input): SV_TARGET
{
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	float3 L = normalize(light_dir.xyz);


	float3 vx = normalize(input.v_tan);
	float3 vy = normalize(input.v_bin);
	float3 vz = normalize(input.v_normal);

	float2 texcoord = input.texcoord;

	float4 color = diffuse_texture.Sample(decal_sampler, texcoord);
	float3 N = normal_texture.Sample(normal_sampler, texcoord).xyz;
	N = N * 2.0f - 1.0f;

	float3x3 mat = { vx, vy, vz };
	N = normalize(mul(N, mat));

	float3 A = ambient_color.rgb;
	float3 C = light_color.rgb;

	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);

	return color * input.color * float4(A + D + S, 1.0);

}