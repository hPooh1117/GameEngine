#include "H_Functions.hlsli"
#include "H_DirectionalLight.hlsli"
#include "H_DataFormat.hlsli"
#include "H_ShadowMap.hlsli"
#include "H_BumpMapping.hlsli"

//--------------------------------------------
//	�G���g���[�|�C���g
//--------------------------------------------
PS_Input_Shadow VSshadow(VS_Input input)
{
	PS_Input_Shadow output = (PS_Input_Shadow)0;

	float4 P = float4(input.position, 1);

	// WVP�ϊ�
	output.position = mul(P, matWVP);

	// �o�͒l�ݒ�
	output.depth.xy = output.position.zw;

	return output;
}

PS_Input_Last VSmain(VS_Input input)
{
	PS_Input_Last output = (PS_Input_Last)0;

	float4 P = float4(input.position, 1);

	// ���[���h�ϊ����W
	float3 wPos = mul(P, world).xyz;

	// �ˉe��Ԃɕϊ�
	P = mul(P, matWVP);

	// ���[���h�@���Z�o
	float3 wN = mul(input.normal, (float3x3)world).xyz;
	wN = normalize(wN);

	// �o�͒l�ݒ�
	output.position = P;
	output.w_pos = wPos;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;

	// ���C�g��ԍ��W�ɕϊ�
	output.v_shadow = GetShadowTex(light_view_projection, wPos.xyz);

	return output;
}

PS_Input_Last VSmain2(VS_Input input)
{
	PS_Input_Last output = (PS_Input_Last)0;

	float4 P = float4(input.position, 1);

	// ���[���h�ϊ����W
	float3 wPos = mul(P, world).xyz;

	// �ˉe��Ԃɕϊ�
	P = mul(P, matWVP);

	// ���[���h�@���Z�o
	float3 wN = mul(input.normal, (float3x3)world).xyz;
	wN = normalize(wN);

	// �o�͒l�ݒ�
	output.position = P;
	output.w_pos = wPos;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;

	// ���C�g��ԍ��W�ɕϊ�
	output.v_shadow = GetShadowTex(light_view_projection, wPos.xyz);

	return output;


}


PS_Input_Shadow VSshadowS(VS_InputS input)
{
	PS_Input_Shadow output = (PS_Input_Shadow)0;

	float4 P = float4(input.position, 1);

	// WVP�ϊ�
	output.position = mul(P, matWVP);

	// �o�͒l�ݒ�
	output.depth.xy = output.position.zw;

	return output;
}

PS_Input_Shadow VSshadowSkinning(VS_InputS input)
{
	PS_Input_Shadow output = (PS_Input_Shadow)0;


	// �X�L�j���O����
	float4 p = { 0, 0, 0, 0 };
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		p += (input.bone_weights[i] * mul(float4(input.position, 1), bone_transforms[input.bone_indices[i]]));
	}
	p.w = 1.0f;

	// WVP�ϊ�
	output.position = mul(p, matWVP);

	// �o�͒l�ݒ�
	output.depth.xy = output.position.zw;

	return output;

}

PS_Input_Last VSmainS(VS_InputS input)
{
	PS_Input_Last output = (PS_Input_Last)0;

	float4 P = float4(input.position, 1);

	// ���[���h�ϊ����W
	float3 wPos = mul(P, world).xyz;

	// �ˉe��Ԃɕϊ�
	P = mul(P, matWVP);

	// ���[���h�@���Z�o
	float3 wN = mul(input.normal, (float3x3)world).xyz;
	wN = normalize(wN);

	// �o�͒l�ݒ�
	output.position = P;
	output.w_pos = wPos;
	output.color = input.color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;

	// ���C�g��ԍ��W�ɕϊ�
	output.v_shadow = GetShadowTex(light_view_projection, wPos.xyz);

	return output;
}

PS_Input_Last VSmainS2(VS_InputS input)
{
	PS_Input_Last output = (PS_Input_Last)0;

	float4 P = float4(input.position, 1);

	// ���[���h�ϊ����W
	float3 wPos = mul(P, world).xyz;

	// �ˉe��Ԃɕϊ�
	P = mul(P, matWVP);

	// ���[���h�@���Z�o
	float3 wN = mul(input.normal, (float3x3)world).xyz;
	wN = normalize(wN);

	// �o�͒l�ݒ�
	output.position = P;
	output.w_pos = wPos;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;

	// ���C�g��ԍ��W�ɕϊ�
	output.v_shadow = GetShadowTex(light_view_projection, wPos.xyz);

	return output;


}

PS_Input_Last VSmainSkinning(VS_InputS input)
{
	PS_Input_Last output = (PS_Input_Last)0;


	// �X�L�j���O����
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


	// ���[���h�ϊ����W
	float3 wPos = mul(p, world).xyz;


	// ���[���h�@���Z�o
	float3 wN = mul(n, world).xyz;
	wN = normalize(wN);

	// �o�͒l�ݒ�
	output.position = mul(p, matWVP);
	output.w_pos = wPos;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;

	// ���C�g��ԍ��W�ɕϊ�
	output.v_shadow = GetShadowTex(light_view_projection, wPos.xyz);

	return output;


}

PS_InputBumpShadow VSmainN(VS_InputS input)
{

	PS_InputBumpShadow output = (PS_InputBumpShadow)0;

	// �X�L�j���O����
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

	// ���[���h�ϊ����W
	float3 wPos = mul(p, world).xyz;

	// �ˉe��Ԃɕϊ�
	p = mul(p, matWVP);

	// ���[���h�@���Z�o
	float3 wN = mul(n, world).xyz;
	wN = normalize(wN);

	float3 vN = wN;
	float3 vB = { 0, 1, -0.001f };
	float3 vT;
	vB = normalize(vB);
	vT = normalize(cross(vB, vN));
	vB = normalize(cross(vN, vT));

	// �o�͒l�ݒ�
	output.position = p;
	output.w_pos = wPos;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;

	output.v_tan = vT;
	output.v_bin = vB;
	output.v_normal = vN;

	// ���C�g��ԍ��W�ɕϊ�
	output.v_shadow = GetShadowTex(light_view_projection, wPos.xyz);

	return output;
}


//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------
float4 PSshadow(PS_Input_Shadow input) : SV_TARGET0
{

	float4 color = input.depth.x / input.depth.y;
	color.a = 1.0f;
	return color;
}

float4 PSmain(PS_Input_Last input) : SV_TARGET
{
	float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);
	float3 N = normalize(input.w_normal);
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	float3 L = normalize(light_dir.xyz);

	// ambient light
	float3 A = ambient_color.rgb;

	// diffuse light
	float3 C = light_color.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	// specular reflection
	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);

	color *= input.color * float4(A + D + S, 1.0);

	// �V���h�E�}�b�v�K�p
	color.rgb *= GetShadow(shadow_texture, shadow_sampler, input.v_shadow, shadow_color, bias);
	
	return color;
}


float4 PSmainEx(PS_InputBumpShadow input) : SV_TARGET
{
	//float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);
	//float3 N = normalize(input.w_normal);
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	float3 L = normalize(light_dir.xyz);

	// ambient light
	float3 A = ambient_color.rgb;

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
	float3 N = normal_texture.Sample(normal_sampler, texcoord).xyz;
	N = N * 2.0f - 1.0f;

	float3x3 mat = { vx, vy, vz };
	N = normalize(mul(N, mat));


	// diffuse light
	float3 C = light_color.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);


	// specular reflection
	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);

	color *= input.color * float4(A + D + S, 1.0);

	// �V���h�E�}�b�v�K�p
	color.rgb *= GetShadow(shadow_texture, shadow_sampler, input.v_shadow, shadow_color, bias);

	return color;

}