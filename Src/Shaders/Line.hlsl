#include "HF_GlobalVariables.hlsli"

static const float Pi = 3.141592653589f;

//----------------------------------
// データフォーマット
//----------------------------------
struct VS_Input
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct GS_Input
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float  thickness : THICKNESS;
};

struct PS_Input
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};


//----------------------------------
// エントリーポイント
//----------------------------------
//
// vertex shader(default)
//
GS_Input VSMainThicker(VS_Input input)
{
	GS_Input output = (GS_Input)0;
	output.position = float4(input.position, 1.0f);
	output.color = mat_color;
	output.thickness = param.x;
	return output;
}

GS_Input VSMainDuplicate(VS_Input input)
{
	GS_Input output = (GS_Input)0;
	output.position = float4(input.position, 1.0f);
	return output;
}

//
// geometry shader
//
[maxvertexcount(6)]
void GSmainThicker(
	line GS_Input input[2],
	inout TriangleStream<PS_Input> output
)
{
		float offset = input[0].thickness / 2.0f;
		
		{
			PS_Input data;
			data.position = input[0].position + float4(0.0f, 0.0f, offset, 0.0f);
			data.position = mul(data.position, matWVP);
			data.color = input[0].color;
			output.Append(data);

			data.position = input[0].position + float4(0.0f, 0.0f, -offset, 0.0f);
			data.position = mul(data.position, matWVP);
			data.color = input[0].color;
			output.Append(data);

			data.position = input[1].position + float4(0.0f, 0.0f, offset, 0.0f);
			data.position = mul(data.position, matWVP);
			data.color = input[1].color;
			output.Append(data);
		}
		output.RestartStrip();

		{
			PS_Input data;
			data.position = input[1].position + float4(0.0f, 0.0f, offset, 0.0f);
			data.position = mul(data.position, matWVP);
			data.color = input[1].color;
			output.Append(data);

			data.position = input[1].position + float4(0.0f, 0.0f, -offset, 0.0f);
			data.position = mul(data.position, matWVP);
			data.color = input[1].color;
			output.Append(data);

			data.position = input[0].position + float4(0.0f, 0.0f, -offset, 0.0f);
			data.position = mul(data.position, matWVP);
			data.color = input[0].color;
			output.Append(data);

		}
		output.RestartStrip();
}

[maxvertexcount(102)]
void GSmainDuplicate(
	line GS_Input input[2],
	inout LineStream<PS_Input> output
)
{
	int j = 0;
	for (j = 0; j < 25; ++j)
	{
		for (uint i = 0; i < 2; ++i)
		{

			PS_Input data;
			float4 position = input[i].position + float4(0, 0, 5.0f * (j - 12) * 0.00625f, 0);
			data.position = mul(position, matWVP);
			data.color = float4(1.0, sign(abs(j - 12)), sign(abs(j - 12)), 0.3);
			output.Append(data);
		}
		output.RestartStrip();

	}

	for (j = 0; j < 25; ++j)
	{
		for (uint i = 0; i < 2; ++i)
		{
			PS_Input data;
			float4 position = input[i].position + float4(0, 0, 5.0f * (j - 12) * 0.00625f, 0);
			data.position = float4(position.z, position.y, position.x, position.w);
			data.position = mul(data.position, matWVP);
			data.color = float4(sign(abs(j - 12)), 1.0, sign(abs(j - 12)), 0.3);
			output.Append(data);
		}
		output.RestartStrip();
	}

	for (j = 0; j < 2; ++j)
	{
		PS_Input data;
		float4 position = input[j].position;
		data.position = float4(position.y, position.x, position.z, position.w);
		data.position = mul(data.position, matWVP);
		data.color = float4(0, 0, 1, 0.3);
		output.Append(data);
	}
}


//
// pixel shader
//
float4 PSmain(PS_Input input) : SV_TARGET
{
	return input.color;
}