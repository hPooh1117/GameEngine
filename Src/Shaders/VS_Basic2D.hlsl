
//--------------------------------------------------------------------------------

struct VS_Input
{
	float3 position : POSITION;
	float4 color    : COLOR;
	float2 texcoord : TEXCOORD;
};

struct PS_Input
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float4 color    : COLOR;
	float2 projPos  : TEXCOORD1;
};

//--------------------------------------------------------------------------------

PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1);


	output.position = P;
	output.color = input.color;
	output.texcoord = input.texcoord;
	output.projPos = output.texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	return output;
}

//--------------------------------------------------------------------------------
