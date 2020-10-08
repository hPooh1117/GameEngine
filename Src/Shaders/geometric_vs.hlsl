#include "geometric_primitive.hlsli"


PS_Input main(float4 pos : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
    PS_Input output;
    output.position = mul(pos, world_view_projection);

    normal.w = 0;
    float4 N = normalize(mul(normal, world));
    float4 L = normalize(-light_dir);

    float diffuseShade = 0.3f;
    output.color = color * max(0, dot(L, N) * diffuseShade + (1- diffuseShade));
    output.color.a = color.a;
    output.texcoord = texcoord;
    return output;
}