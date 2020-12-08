
inline int HasColorMap(int texture_config)       { return ((texture_config & 1)   > 0 ? 1 : 0); }
inline int HasNormalMap(int texture_config)      { return ((texture_config & 2)   > 0 ? 1 : 0); }
inline int HasHeightMap(int texture_config)      { return ((texture_config & 4)   > 0 ? 1 : 0); }
inline int HasMetallicMap(int texture_config)    { return ((texture_config & 8)   > 0 ? 1 : 0); }
inline int HasRoughnessMap(int texture_config)   { return ((texture_config & 16)  > 0 ? 1 : 0); }
inline int HasAOMap(int texture_config)          { return ((texture_config & 32)  > 0 ? 1 : 0); }
inline int HasSpecularMap(int texture_config)    { return ((texture_config & 64)  > 0 ? 1 : 0); }
inline int HasEnvironmentMap(int texture_config) { return ((texture_config & 128) > 0 ? 1 : 0); }
inline int HasShadowMap(int texture_config)      { return ((texture_config & 256) > 0 ? 1 : 0); }

float3 diffuse(float3 normal, float3 L, float3 C, float3 K)
{
    float D = dot(normal, -L);
    D = max(0, D);
    return K * C * D;
}

float3 blinnPhongSpecular(float3 N, float3 L, float3 C, float3 E, float3 K, float lumen)
{
    float3 H = normalize(-L + E);

    float3 S = dot(H, N);
    S = max(0, S);
    S = pow(S, lumen);
    S = S * K * C;
    return S;
}

float3 CalculatePhongSpecular(float3 N, float3 L, float3 C, float3 E, float3 K, float power)
{
    float3 R = normalize(reflect(L, N));
    float3 S = max(0, dot(R, E));
    S = pow(S, power) * K * C;
    
    return S;
}

float3 fog(float3 color, float3 viewPos, float3 Pos, float3 fogColor, float Near, float Far)
{
    //フォグ計算--線形補間フォグ--
    float Len = length(viewPos - Pos);
    float fogalpha = saturate((Len - Near) / (Far - Near));
    return color.rgb * (1.0 - fogalpha) + fogColor * fogalpha;
}

float3 hemiSphereLight(float3 N, float3 skyColor, float3 groundColor)
{
    //--線形補間--
    float skyBlend = (N.y + 1.0f) * 0.5f;
    float groundBlend = 1.0 - skyBlend;
    return skyColor * skyBlend + groundColor * groundBlend;
}

float3 ApplyNormalMap(float3x3 mat, Texture2D tex, SamplerState ss, float2 texcoord)
{
    float3 N = tex.Sample(ss, texcoord).xyz;
    N = N * 2.0 - 1.0;
    return normalize(mul(mat, N));
}

float ApplyHeightMap(Texture2D tex, SamplerState ss, float2 texcoord)
{
    float H = tex.Sample(ss, texcoord).r;
    return H * 2.0 - 1.0;
}

float3 ToonLight(float3 N, float3 L, float3 C, float3 K)
{
    float D = dot(N, -L);

    if (D < 0.3) D = 0.2;
    else if (D < 0.9) D = 0.6;
    else D = 1.4;

    return K * C * D;
}

float3 ToonDiffuse(float3 N, float3 L, float3 C, float3 K)
{
    float D = dot(N, -L);

    if (D < 0.3) D = 0.2;
    else D = 0.6;

    return K * C * D;

}

float3 ToonSpecular(float3 N, float3 L, float3 C, float3 E, float3 K, float power)
{
    float3 R = reflect(L, N);
    R = normalize(R);
    float S = dot(R, E);
    S = max(0, S);
    S = pow(S, power);
    if (S > 0.5) S = 0.8;
    else S = 0.4;
    return S * K * C;
}

// ワールド座標をライト空間座標に変換
float3 GetShadowTex(float4x4 vp, float3 wPos)
{
    // 正規化デバイス座標系
    float4 wvpPos;
    wvpPos = mul(vp, float4(wPos, 1));
    wvpPos /= wvpPos.w;

    // テクスチャ座標系
    wvpPos.y = -wvpPos.y;
    wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;

    return wvpPos.xyz;
}

// シャドーマップからライト空間座標に変換＆Z値比較
float3 GetShadow(Texture2D st, SamplerState ss, float3 tex, float3 scolor, float bias)
{
    // シャドーマップから深度を取り出す
    float d = st.Sample(ss, tex.xy).r;

    // シャドーマップの深度値と現実の深度の比較
    scolor = (tex.z - d > bias) ? scolor : 1.0;

    return scolor;
}

float3 GetSoftShadow(Texture2D shadow_tex, SamplerComparisonState ss, float3 tex, float3 shadow_color, float bias)
{
    float maxDepthSlope = max(abs(ddx(tex.z)), abs(ddy(tex.z)));

    float slopeScaledBias = 0.01;
    float depthBiasClamp = 0.1f;
    float shadowBias = bias + slopeScaledBias * maxDepthSlope;
    shadowBias = min(shadowBias, depthBiasClamp);

    float d = shadow_tex.SampleCmpLevelZero(ss, tex.xy, tex.z - shadowBias);

    float3 shadowColor = lerp(float3(1, 1, 1), shadow_color, d);

    return shadowColor;
}

float3 HalfLambert(float3 N, float3 L, float3 C, float3 Kd)
{
    float D = dot(N, -L);
    D = max(D, 0) * 0.5 + 0.5;
    D = D * D;

    return Kd * C * D;
}

float3 FurSpecular(float3 N, float3 L, float3 C, float3 E, float3 K, float Power)
{
    float3 R = reflect(L, N);
    R = normalize(R);
    float Cos = dot(R, E);
    float Sin2 = 1 - Cos * Cos;
    Sin2 = pow(abs(Sin2), Power);
    return Sin2 * K * C;
}

inline float3 ReconstructPosition(in float2 uv, in float z, in float4x4 InvVP)
{
    float x = uv.x * 2.0 - 1.0;
    float y = (1.0 - uv.y) * 2.0 - 1.0;
    float4 positionP = float4(x, y, z, 1.0);
    float4 positionV = mul(positionP, InvVP);
    return positionV.xyz / positionV.w;
}

// 0 ~ 1の範囲でrandom float valueを返す。
// seed   : 必ず０より大きい数を入れる。
inline float Rand(inout float seed, in float2 uv)
{
    float result = frac(sin(seed * dot(uv, float2(12.9898, 78.233))) * 43758.5453);
    seed += 1.0f;
    return result;
}


