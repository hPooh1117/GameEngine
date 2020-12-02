#include "H_DataFormat.hlsli"

PS_Input VSmain(VS_InputS input)
{
	PS_Input output = (PS_Input)0;

    // スキニング処理
    float4 p = { 0, 0, 0, 0 };
    int i = 0;
    
    for (i = 0; i < 4; i++)
    {
        p += (input.bone_weights[i] * mul(float4(input.position, 1), bone_transforms[input.bone_indices[i]]));
    }
    p.w = 1.0f;



	output.position = mul(p, matWVP);
	output.texcoord = input.texcoord;
	output.color = input.color;
	return output;
}
