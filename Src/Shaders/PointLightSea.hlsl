#include "H_DataFormat.hlsli"
#include "H_BumpMapping.hlsli"
#include "H_PointLight.hlsli"
#include "H_Functions.hlsli"


Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

Texture2D normal_texture : register(t1);
SamplerState normal_sampler : register(s1);

Texture2D height_texture : register(t2);
SamplerState height_sampler : register(s2);

//--------------------------------------------------
// �O���[�o���ϐ�
//--------------------------------------------------
// �g�̑����A����
static const float2 wave1 = { 0.01, 0.005 };
static const float2 wave2 = { -0.005, -0.004 };

// �g�̑傫��
static const float wave_size = 5.0f;

// ���ʂ̑S���˂̗ՊE�p
static const float limit = 0.248;

//--------------------------------------------
//	�G���g���[�|�C���g
//--------------------------------------------
//
// vertex shader(default)
//
PS_InputBump VSmain(VS_Input input)
{
	PS_InputBump output = (PS_InputBump)0;

	float4 P = float4(input.position, 1.0);

	// ���[���h�ϊ����W
	float4 wPos = mul(P, world);

	// �ˉe��Ԃɕϊ�
	P = mul(P, matWVP);

	// ���[���h�@���Z�o
	float3 wN = mul(input.normal, (float3x3)world);
	wN = normalize(wN); // ���K��

	// �ڋ��
	float3 vN = wN;
	float3 vB = { 0, 1, -0.001 };
	float3 vT;
	vB = normalize(vB);
	vT = normalize(cross(vB, vN));
	vB = normalize(cross(vN, vT));

	// �o�͒l�ݒ�
	output.position = P;
	output.color = float4(1, 1, 1, 1);
	output.texcoord = input.texcoord;
	output.w_pos = wPos.xyz;

	// �ڋ�Ԏ�
	output.v_tan = vT;
	output.v_bin = vB;
	output.v_normal = vN;

	return output;
}


//
// pixel shader
//
float4 PSmain(PS_InputBump input) : SV_TARGET
{
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	float3 L = normalize(light_dir.xyz);

	float3 A = ambient_color.rgb;
	float3 C = light_color.rgb;
	
	// �ڋ�Ԏ�
	float3 vx = normalize(input.v_tan);
	float3 vy = normalize(input.v_bin);
	float3 vz = normalize(input.v_normal);

	// �e�N�X�`���T�C�Y����
	float2 tex = input.texcoord * wave_size;

	// �e�N�X�`���F�擾
	float4 color = diffuse_texture.Sample(decal_sampler, tex);

	// �@���擾
	float3 N1 = normal_texture.Sample(normal_sampler, input.texcoord + wave1 * time).xyz;
	N1 = 2.0 * N1 - 1.0;
	float3 N2 = normal_texture.Sample(normal_sampler, input.texcoord + wave2 * time).xyz;
	N2 = 2.0 * N2 - 1.0;

	// �@������
	float3 N = normalize(N1 + N2);

	// �@�����[���h�ϊ�
	float3x3 mat = { vx, vy, vz };
	N = normalize(mul(N, mat));

	// �g�U����
	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	// ���ʔ���
	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 30);

	// �|�C���g���C�g
	float3 PL;
	float3 PC;
	float influence;
	float3 PD = (float3)0;
	float3 PS = (float3)0;

	for (int i = 0; i < SPOTMAX; ++i)
	{
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

	color *= input.color * float4(A + /*D + S*/ +PD + PS, 1);



	// �C�ʑS���˂Ɠ��ߕ\��
	float angle = 1.0 - dot(E, N);
	angle = saturate(angle * 1.5);
	color.z *= angle;

	return color;

}