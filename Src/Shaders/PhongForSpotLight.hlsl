#include "H_DataFormat.hlsli"
#include "H_Functions.hlsli"
//#include "H_SpotLight.hlsli"
#include "H_Light.hlsli"
#include "H_BumpMapping.hlsli"

//--------------------------------------------
//	�G���g���[�|�C���g
//--------------------------------------------
//
// vertex shader(default)
//
PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1.0);
	// ���[���h�ϊ����W
	float4 worldPos = mul(P, world);
	// �ˉe��Ԃɕϊ�(��D��)
	P = mul(P, matWVP);
	// ���[���h�@���Z�o
	float3 N = mul(input.normal, (float3x3)world);
	N = normalize(N);//���K��


	// �o�͒l�ݒ�.
	output.position = P;
	//	output.Color = input.Color;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_pos = worldPos.xyz;
	output.w_normal = N;

	return output;
}

PS_InputBump VSmainBump(VS_Input input)
{
	PS_InputBump output = (PS_InputBump)0;

	float4 P = float4(input.position, 1.0);

	output.position = mul(P, matWVP);

	float3 wPos = mul(P, world).xyz;

	float4 N = float4(input.normal, 0.0f);
	float3 wN = mul(N, world).xyz;
	wN = normalize(wN);

	float3 vN = wN;
	float3 vB = { 0, 1, -0.001f };
	float3 vT;
	vB = normalize(vB);
	vT = normalize(cross(vB, vN));
	vB = normalize(cross(vN, vT));

	output.color = input.color;

	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_pos = wPos;

	output.v_tan = vT;
	output.v_bin = vB;
	output.v_normal = vN;

	return output;
}


//
// vertex shader(SkinnendMesh)
//
PS_Input VSmainS(VS_InputS input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1.0);
	// ���[���h�ϊ����W
	float4 worldPos = mul(P, world);
	// �ˉe��Ԃɕϊ�(��D��)
	P = mul(P, matWVP);
	// ���[���h�@���Z�o
	float3 N = mul(input.normal, (float3x3)world);
	N = normalize(N);//���K��


	// �o�͒l�ݒ�.
	output.position = P;
	//	output.Color = input.Color;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_pos = worldPos.xyz;
	output.w_normal = N;

	return output;
}

PS_Input VSmainSkinning(VS_InputS input)
{
	PS_Input output = (PS_Input)0;


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
	float4 worldPos = mul(p, world);

	// ���[���h�@���Z�o
	float3 N = mul(n, world).xyz;
	N = normalize(N);//���K��


	// �o�͒l�ݒ�.
	output.position = mul(p, matWVP);
	//	output.Color = input.Color;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_pos = worldPos.xyz;
	output.w_normal = N;

	return output;
}

PS_InputBump VSmainSBump(VS_InputS input)
{
	PS_InputBump output = (PS_InputBump)0;

	float4 P = float4(input.position, 1.0);

	output.position = mul(P, matWVP);

	float3 wPos = mul(P, world).xyz;

	float4 N = float4(input.normal, 0.0f);
	float3 wN = mul(N, world).xyz;
	wN = normalize(wN);

	float3 vN = wN;
	float3 vB = { 0, 1, -0.001f };
	float3 vT;
	vB = normalize(vB);
	vT = normalize(cross(vB, vN));
	vB = normalize(cross(vN, vT));

	output.color = input.color;

	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_pos = wPos;

	output.v_tan = vT;
	output.v_bin = vB;
	output.v_normal = vN;

	return output;
}

//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------
//
// pixel shader
//
float4 PSmain(PS_Input input) : SV_TARGET
{
	float3 L = normalize(light_dir.xyz);
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	// ����
	float3 Ambient = ambient_color.rgb;//
	float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);
	float3 C = light_color.rgb;		//���ˌ�(�F�Ƌ���)
	// �@��
	float3 N = normalize(input.w_normal);


	float3 Kd = float3(1, 1, 1);	//���˗�
	float3 D = diffuse(N, L, C, Kd);


	// ���ʔ���
	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);


	//�X�|�b�g���C�g
	float3 SpotLight;
	float3 SpotLightColor;
	float influence;
	float3 SpotDiffuse =(float3)0;
	float3 SpotSpecular = (float3)0;

	for (int i = 0; i < SPOTMAX; ++i)
	{
		if (spot_lights[i].type == 0.0f) continue;

		SpotLight = input.w_pos - spot_lights[i].pos.xyz;
		float d = length(SpotLight);

		float r = spot_lights[i].range;
		if (d > r) continue;

		float3 SpotFront = normalize(spot_lights[i].dir.xyz);
		SpotLight = normalize(SpotLight);

		float angle = dot(SpotLight, SpotFront);
		float area = spot_lights[i].inner_corn - spot_lights[i].outer_corn;
		
		float influence2 = spot_lights[i].inner_corn - angle;
		influence2 = saturate(1.0 - influence2 / area);
		influence = 1;
		SpotLightColor = spot_lights[i].color.rgb;
		SpotDiffuse += diffuse(N, SpotLight, SpotLightColor, Kd) * influence * influence * influence2;
		SpotSpecular += blinnPhongSpecular(N, SpotLight, SpotLightColor, E, Ks, 20) * influence * influence * influence2;
	}


	return color * input.color * float4(Ambient + D + S + SpotDiffuse + SpotSpecular, 1.0f);
}

float4 PSmainBump(PS_InputBump input) : SV_TARGET
{
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	float3 L = normalize(light_dir.xyz);
	// ����	
	float3 A = ambient_color.rgb;
	float3 C = light_color.rgb;

	// �ڋ�Ԏ�
	float3 vx = normalize(input.v_tan);
	float3 vy = normalize(input.v_bin);
	float3 vz = normalize(input.v_normal);

	float2 Tex = input.texcoord;

	// �����ڋ�ԕϊ�
	float3x3 inv_mat = { vx,vy,vz };
	float3 inv_E = normalize(mul(inv_mat, E));

	// �����v�Z
	float H = height_texture.Sample(height_sampler, Tex).r;
	H = H * 2.0f - 1.0;
	Tex.x -= inv_E.x * H * viewOffset;
	Tex.y -= inv_E.y * H * viewOffset;


	// �e�N�X�`���F�擾
	float4 color = diffuse_texture.Sample(decal_sampler, Tex);
	float3 N = normal_texture.Sample(normal_sampler, Tex).xyz;
	N = N * 2.0f - 1.0f;

	// �@�����[���h�ϊ�
	float3x3 mat = { vx,vy,vz };
	N = normalize(mul(N,mat));

	// �g�U����
	float3 Kd = float3(1,1,1);
	float3 D = diffuse(N, L, C, Kd);

	// ���ʔ���
	float3 Ks = float3(1,1,1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);


	//�X�|�b�g���C�g
	float3 SpotLight;
	float3 SpotLightColor;
	float influence;
	float3 SpotDiffuse = (float3)0;
	float3 SpotSpecular = (float3)0;

	for (int i = 0; i < SPOTMAX; ++i)
	{
		if (spot_lights[i].type == 0.0f) continue;

		SpotLight = input.w_pos - spot_lights[i].pos.xyz;
		float d = length(SpotLight);

		float r = spot_lights[i].range;
		if (d > r) continue;

		float3 SpotFront = normalize(spot_lights[i].dir.xyz);
		SpotLight = normalize(SpotLight);

		float angle = dot(SpotLight, SpotFront);
		float area = spot_lights[i].inner_corn - spot_lights[i].outer_corn;

		float influence2 = spot_lights[i].inner_corn - angle;
		influence2 = saturate(1.0 - influence2 / area);
		influence = 1;
		SpotLightColor = spot_lights[i].color.rgb;
		SpotDiffuse += diffuse(N, SpotLight, SpotLightColor, Kd) * influence * influence * influence2;
		SpotSpecular += blinnPhongSpecular(N, SpotLight, SpotLightColor, E, Ks, 20) * influence * influence * influence2;
	}


	return color * input.color * float4(A + D + S + SpotDiffuse + SpotSpecular, 1.0f);
}
