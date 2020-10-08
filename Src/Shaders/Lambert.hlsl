#include "H_DataFormat.hlsli"
#include "H_DirectionalLight.hlsli"


//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
// 
// vertex shader (default)
//
PS_Input VSmain(VS_Input input)
{
    PS_Input output = (PS_Input)0;
    output.position = mul(float4(input.position, 1), matWVP);

    float4 N = normalize(mul(float4(input.normal, 0), world));
    float4 L = normalize(-light_dir);

    // 平行光源
    float diffuseShade = 0.5f;
    output.color = mat_color * max(0, dot(L, N) * diffuseShade + (1 - diffuseShade));
    output.color.a = mat_color.a;


    output.texcoord = input.texcoord;
    return output;
}

PS_Input VSmainS(VS_InputS input)
{
    PS_Input output = (PS_Input)0;
    output.position = mul(float4(input.position, 1), matWVP);

    float4 N = normalize(mul(float4(input.normal, 0), world));
    float4 L = normalize(-light_dir);

    // 平行光源
    float diffuseShade = 0.5f;
    output.color = mat_color * max(0, dot(L, N) * diffuseShade + (1 - diffuseShade));
    output.color.a = mat_color.a;


    output.texcoord = input.texcoord;
    return output;
}


// 
// vertex shader (skinned_mesh)
//
PS_Input VSmainSkinning(VS_InputS input)
{
    PS_Input output = (PS_Input)0;



    // スキニング処理
    float4 p = { 0, 0, 0, 0 };
    float4 n = { 0, 0, 0, 0 };
    int i = 0;
    for (i = 0; i < 4; i++)
    {
        p += (input.bone_weights[i] * mul(float4(input.position, 1), bone_transforms[input.bone_indices[i]]));
        n += (input.bone_weights[i] * mul(float4(input.normal, 0), bone_transforms[input.bone_indices[i]]));
    }
    p.w = 1.0f;
    n.w = 0.0f;
    float4 N = normalize(mul(n, world));
    float4 L = normalize(-light_dir);


    // 平行光源
    float diffuseShade = 0.5f;
    output.color = mat_color * max(0, dot(L, N) * diffuseShade + (1 - diffuseShade));
    output.color.a = mat_color.a;

    output.position = mul(p, matWVP);
    output.texcoord = input.texcoord;
    return output;
}

//
// pixel shader
//
float4 PSmain(PS_Input input) : SV_TARGET
{
    return diffuse_texture.Sample(decal_sampler, input.texcoord) * input.color;
}