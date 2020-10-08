#include "primitive_batch.hlsli"

VS_OUT main( float4 pos : POSITION, column_major float4x4 ndc_transform : NDC_TRANSFORM, float4 color : COLOR )
{
    VS_OUT vout;
vout.pos = mul(pos, ndc_transform);
vout.color = color;
	return vout;
}