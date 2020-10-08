#include "H_ToGBuffer.hlsli"
#include "H_DataFormat.hlsli"
#include "H_BumpMapping.hlsli"
#include "H_DirectionalLight.hlsli"


//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_InputBump VSmain(VS_Input input)
{
	PS_InputBump output = (PS_InputBump)0;
	float4 P = float4(input.position, 1);

	float4 wPos = mul(P, world);

	float3 N = mul(input.normal, (float3x3)world);

	// 接空間行列
	float3 vy = { 0.0, 1.0, 0.001 };
	float3 vz = N;
	float3 vx = cross(vy, vz);
	vx = normalize(vx);
	vy = cross(vz, vx);
	vy = normalize(vy);


	output.position = mul(P, matWVP);
	output.w_pos = wPos.xyz;
	output.w_normal = N;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.v_tan = vx;
	output.v_bin = vy;
	output.v_normal = vz;

	return output;
}

//
// vertex shader(SkinnedMesh)
//
PS_InputBump VSmainS(VS_InputS input)
{
	PS_InputBump output = (PS_InputBump)0;
	float4 P = float4(input.position, 1);

	float4 wPos = mul(P, world);

	float3 N = mul(input.normal, (float3x3)world);

	// 接空間行列
	float3 vy = { 0.0, 1.0, 0.001 };
	float3 vz = N;
	float3 vx = cross(vy, vz);
	vx = normalize(vx);
	vy = cross(vz, vx);
	vy = normalize(vy);


	output.position = mul(P, matWVP);
	output.w_pos = wPos.xyz;
	output.w_normal = N;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.v_tan = vx;
	output.v_bin = vy;
	output.v_normal = vz;
	return output;
}


PS_InputBump VSmainSkinning(VS_InputS input)
{
	PS_InputBump output = (PS_InputBump)0;

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

	float4 wPos = mul(p, world);

	float3 N = n.xyz;

	// 接空間行列
	float3 vy = { 0.0, 1.0, 0.001 };
	float3 vz = N;
	float3 vx = cross(vy, vz);
	vx = normalize(vx);
	vy = cross(vz, vx);
	vy = normalize(vy);


	output.position = mul(p, matWVP);
	output.w_pos = wPos.xyz;
	output.w_normal = N;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.v_tan = vx;
	output.v_bin = vy;
	output.v_normal = vz;

	return output;
}


//
// pixel shader
//
PS_Output PSmain(PS_InputBump input)
{
	PS_Output output = (PS_Output)0;
	float4 P = float4(input.w_pos, 1);

	float3 vx = normalize(input.v_tan);
	float3 vy = normalize(input.v_bin);
	float3 vz = normalize(input.v_normal);

	float3 E = normalize(eye_pos.xyz - input.w_pos);

	// 接空間行列
	float3x3 vMat = { {vx}, {vy}, {vz} };

	// 接空間からワールドへ変換
	float3 invE = normalize(mul(vMat, E));

	float H = height_texture.Sample(height_sampler, input.texcoord).r;
	H = H * 2.0f - 1.0f;
	input.texcoord.x -= invE.x * H * viewOffset;
	input.texcoord.y -= invE.y * H * viewOffset;

	// 法線マップから法線取得
	float3 N = normal_texture.Sample(normal_sampler, input.texcoord).xyz;
	N = N * 2.0 - 1.0;
	// 接空間からワールドへ変換
	N = normalize(mul(vMat, N));


	output.color = diffuse_texture.Sample(decal_sampler, input.texcoord) * input.color;
	output.normal = float4(N, 1);
	output.position = P;
	return output;
}