//--------------------------------------------
//	テクスチャ
//--------------------------------------------
Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

Texture2D diffuse_light_texture : register(t1);
SamplerState diffuse_sampler : register(s1);

Texture2D specular_texture : register(t2);
SamplerState specular_sampler : register(s2);

Texture2D ambient_texture : register(t3);
SamplerState ambient_sampler : register(s3);

//--------------------------------------------
//	グローバル変数
//--------------------------------------------
cbuffer CBPerMatrix : register(b0)
{
	row_major float4x4 matWVP;
	row_major float4x4 world;

	float4 mat_color;

};

static const int BLUR_SIZE = 5;

//--------------------------------------------
//	データレイアウト
//--------------------------------------------
struct VS_Input
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

struct PS_Input
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

struct PS_Output
{
	float4 result : SV_TARGET0;
	float4 postEffect : SV_TARGET1;
	float4 noAmbient : SV_TARGET2;
};

//************************************************************
//	Vertex Shader
//************************************************************
PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	float4 P = float4(input.position, 1);
	
	output.position = P;
	output.color = input.color;
	output.texcoord = input.texcoord;

	return output;
}


//************************************************************
//	Pixel Shader
//************************************************************
PS_Output PSmain(PS_Input input)
{
	PS_Output output = (PS_Output)0;


	float3 D = diffuse_light_texture.Sample(diffuse_sampler, input.texcoord).rgb;
	float3 S = specular_texture.Sample(specular_sampler, input.texcoord).rgb;
	float3 A = ambient_texture.Sample(ambient_sampler, input.texcoord).rgb;

	float2 textureInfo;
	ambient_texture.GetDimensions(textureInfo.x, textureInfo.y);
	float2 texelSize = 1.0 / textureInfo;
	float result = 0.0;
	float2 hlim = float2(float(-BLUR_SIZE) * 0.5 + 0.5, float(-BLUR_SIZE) * 0.5 + 0.5);
	for (int i = 0; i < BLUR_SIZE; ++i)
	{
		for (int j = 0; j < BLUR_SIZE; ++j)
		{
			float2 offset = (hlim + float2(float(i), float(j))) * texelSize;
			result += ambient_texture.Sample(ambient_sampler, input.texcoord + offset.xy).r;
		}
	}
	result /= float(BLUR_SIZE * BLUR_SIZE);



	float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);
	color.a = 1;
	output.result = float4(D * result + S * result, 1) * color;
	output.postEffect = float4(result, result, result, 1);
	output.noAmbient = float4(D + S, 1) * color;
	return output;
}