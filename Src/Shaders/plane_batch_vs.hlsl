#include "plane_batch.hlsli"

PS_Input main( VS_Input input )
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.pos, 1.0f);
	output.pos = mul(P, input.ndc_transform);
	output.texcoord = input.texcoord/* * input.texcoord_transform.zw + input.texcoord_transform.xy*/;
	output.color = input.color;
	return output;
}