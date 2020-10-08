struct VS_Input
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	column_major float4x4 ndc_transform : NDC_TRANSFORM;
	//float4 texcoord_transform : TEXCOORD_TRANSFORM;
	float4 color : COLOR;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};