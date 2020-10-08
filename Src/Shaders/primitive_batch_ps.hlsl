#include "primitive_batch.hlsli"

float4 main(VS_OUT input) : SV_TARGET
{
    return input.color;
}