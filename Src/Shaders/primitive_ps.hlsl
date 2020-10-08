#include "primitive.hlsli"


float4 main(VS_OUT input) : SV_TARGET
{
    return input.color;
}