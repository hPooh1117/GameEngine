#include "H_DataFormat.hlsli"

PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	output.position = mul(float4(input.position, 1), matWVP);
	output.texcoord = input.texcoord;
	output.color = input.color;
	return output;
}
