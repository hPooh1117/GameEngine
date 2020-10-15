
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

//--------------------------------------------------------------------------------------------------------------
// BRDF (Bidirectional Reflectance Distribution Function)
//--------------------------------------------------------------------------------------------------------------

// 鏡面反射BRDF -> Cook-Torranceモデル
//
// Fr(v, l) = D(h, a) * G(v, l, a) * F(v, h, f0) / 4(n, v)(n, l)
//
// D(h, a)     : 法線分布関数(Normal Distribution Function) ->Beckman, Phong, GGX(Trowbrdige-Retiz)
// G(v, l, a)  : 幾何減衰...マイクロファセットによる光の減衰 -> Smith関数のSchlick近似とHeight-Correlated Smith関数
// F(v, h, f0) : フレネル反射...視線ベクトルと法線によって反射率が変わる現象 -> Schlickによる近似式


static const float Pi = 3.141592653589f;
static const float InvPi = 0.318309886183871f;
static const float _DielectricF0 = 0.04;

// D項の計算
float Fd_Burley(float ndotv, float ndotl, float ldoth, float roughness)
{
    float fd90 = 0.5 + 2 * ldoth * ldoth * roughness;
    float lightScatter = (1 + (fd90 - 1) * pow(1 - ndotl, 5));
    float viewScatter = (1 + (fd90 - 1) * pow(1 - ndotv, 5));

    float diffuse = lightScatter * viewScatter;

    // diffuse *= UNITY_INV_PI; // 本来はこのDiffuseをπで割るべきだけどUnityではレガシーなライティングとの互換性を保つため割らない

    return diffuse;
}

// V項の計算
float V_SmithGGXCorrelated(float ndotl, float ndotv, float alpha)
{
    float lambdaV = ndotl * (ndotv * (1 - alpha) + alpha);
    float lambdaL = ndotv * (ndotl * (1 - alpha) + alpha);

    return 0.5f / (lambdaV + lambdaL + 0.0001);
}

// D項の計算
float DistributionGGX(float perceptualRoughness, float ndoth, float3 normal, float3 halfDir)
{
    float3 ncrossh = cross(normal, halfDir);
    float a = ndoth * perceptualRoughness;
    float k = perceptualRoughness / (dot(ncrossh, ncrossh) + a * a);
    float d = k * k * 1 / InvPi;
    return min(d, 65504.0f);
}

// F項の計算
float3 F_Schlick(float3 f0, float cos)
{
    return f0 + (1 - f0) * pow(1 - cos, 5);
}

// Diffuse, SpecularをBRDFで算出
float4 BRDF(
    float3 albedo,
    float metalness,
    float perceptualRoughness,
    float3 normal,
    float3 viewDir,
    float3 lightDir,
    float3 lightColor
)
{
    float3 halfDir = normalize(lightDir + viewDir);
    float ndotv = abs(dot(normal, viewDir));
    float ndotl = max(0, dot(normal, lightDir));
    float ndoth = max(0, dot(normal, halfDir));
    float ldoth = max(0, dot(lightDir, halfDir));
    float reflectivity = lerp(_DielectricF0, 1, metalness);
    float3 f0 = lerp(_DielectricF0, albedo, metalness);

    // Diffuse
    float diffuseTerm = Fd_Burley(ndotv, ndotl, ldoth, perceptualRoughness) * ndotl;
    float3 diffuse = albedo * (1 - reflectivity) * lightColor * diffuseTerm;

    // Indirect Diffuse
    //diffuse += albedo * (1 - reflectivity) * indirectDiffuse;

    // Specular
    float alpha = perceptualRoughness * perceptualRoughness;
    float V = V_SmithGGXCorrelated(ndotl, ndotv, alpha);
    float D = DistributionGGX(perceptualRoughness, ndotv, normal, halfDir);
    float3 F = F_Schlick(f0, ldoth); // マイクロファセットベースのスペキュラBRDFではcosにはldothが使われる
    float3 specular = V * D * F * ndotl * lightColor;

    specular *= Pi; // 本来はSpecularにπを掛けるべきではないが、Unityではレガシーなライティングとの互換性を保つため、Diffuseを割らずにSpecularにPIを掛ける
    specular = max(0, specular);

    // Indirect Specular
    //float surfaceReduction = 1.0 / (alpha * alpha + 1.0);
    //float f90 = saturate((1 - perceptualRoughness) + reflectivity);
    //specular += surfaceReduction * indirectSpecular * lerp(f0, f90, pow(1 - ndotv, 5));

    float3 color = diffuse + specular;
    return float4(color, 1);
}
