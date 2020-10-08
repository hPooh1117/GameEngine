#include "primitive.hlsli"


VS_OUT main( float4 pos : POSITION, float4 normal : NORMAL, float4 color : COLOR )
{
    VS_OUT output;
    output.pos = pos;
    output.normal = normal;
    output.color = color;
	return output;
}