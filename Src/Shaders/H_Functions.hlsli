
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
    //�t�H�O�v�Z--���`��ԃt�H�O--
    float Len = length(viewPos - Pos);
    float fogalpha = saturate((Len - Near) / (Far - Near));
    return color.rgb * (1.0 - fogalpha) + fogColor * fogalpha;
}

float3 hemiSphereLight(float3 N, float3 skyColor, float3 groundColor)
{
    //--���`���--
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

// ���[���h���W�����C�g��ԍ��W�ɕϊ�
float3 GetShadowTex(float4x4 vp, float3 wPos)
{
    // ���K���f�o�C�X���W�n
    float4 wvpPos;
    wvpPos = mul(vp, float4(wPos, 1));
    wvpPos /= wvpPos.w;

    // �e�N�X�`�����W�n
    wvpPos.y = -wvpPos.y;
    wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;

    return wvpPos.xyz;
}

// �V���h�[�}�b�v���烉�C�g��ԍ��W�ɕϊ���Z�l��r
float3 GetShadow(Texture2D st, SamplerState ss, float3 tex, float3 scolor, float bias)
{
    // �V���h�[�}�b�v����[�x�����o��
    float d = st.Sample(ss, tex.xy).r;

    // �V���h�[�}�b�v�̐[�x�l�ƌ����̐[�x�̔�r
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

// ���ʔ���BRDF -> Cook-Torrance���f��
//
// Fr(v, l) = D(h, a) * G(v, l, a) * F(v, h, f0) / 4(n, v)(n, l)
//
// D(h, a)     : �@�����z�֐�(Normal Distribution Function) ->Beckman, Phong, GGX(Trowbrdige-Retiz)
// G(v, l, a)  : �􉽌���...�}�C�N���t�@�Z�b�g�ɂ����̌��� -> Smith�֐���Schlick�ߎ���Height-Correlated Smith�֐�
// F(v, h, f0) : �t���l������...�����x�N�g���Ɩ@���ɂ���Ĕ��˗����ς�錻�� -> Schlick�ɂ��ߎ���


static const float Pi = 3.141592653589f;
static const float InvPi = 0.318309886183871f;
static const float _DielectricF0 = 0.04;

// D���̌v�Z
float Fd_Burley(float ndotv, float ndotl, float ldoth, float roughness)
{
    float fd90 = 0.5 + 2 * ldoth * ldoth * roughness;
    float lightScatter = (1 + (fd90 - 1) * pow(1 - ndotl, 5));
    float viewScatter = (1 + (fd90 - 1) * pow(1 - ndotv, 5));

    float diffuse = lightScatter * viewScatter;

    // diffuse *= UNITY_INV_PI; // �{���͂���Diffuse���΂Ŋ���ׂ�������Unity�ł̓��K�V�[�ȃ��C�e�B���O�Ƃ̌݊�����ۂ��ߊ���Ȃ�

    return diffuse;
}

// V���̌v�Z
float V_SmithGGXCorrelated(float ndotl, float ndotv, float alpha)
{
    float lambdaV = ndotl * (ndotv * (1 - alpha) + alpha);
    float lambdaL = ndotv * (ndotl * (1 - alpha) + alpha);

    return 0.5f / (lambdaV + lambdaL + 0.0001);
}

// D���̌v�Z
float DistributionGGX(float perceptualRoughness, float ndoth, float3 normal, float3 halfDir)
{
    float3 ncrossh = cross(normal, halfDir);
    float a = ndoth * perceptualRoughness;
    float k = perceptualRoughness / (dot(ncrossh, ncrossh) + a * a);
    float d = k * k * 1 / InvPi;
    return min(d, 65504.0f);
}

// F���̌v�Z
float3 F_Schlick(float3 f0, float cos)
{
    return f0 + (1 - f0) * pow(1 - cos, 5);
}

// Diffuse, Specular��BRDF�ŎZ�o
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
    float3 F = F_Schlick(f0, ldoth); // �}�C�N���t�@�Z�b�g�x�[�X�̃X�y�L����BRDF�ł�cos�ɂ�ldoth���g����
    float3 specular = V * D * F * ndotl * lightColor;

    specular *= Pi; // �{����Specular�Ƀ΂��|����ׂ��ł͂Ȃ����AUnity�ł̓��K�V�[�ȃ��C�e�B���O�Ƃ̌݊�����ۂ��߁ADiffuse�����炸��Specular��PI���|����
    specular = max(0, specular);

    // Indirect Specular
    //float surfaceReduction = 1.0 / (alpha * alpha + 1.0);
    //float f90 = saturate((1 - perceptualRoughness) + reflectivity);
    //specular += surfaceReduction * indirectSpecular * lerp(f0, f90, pow(1 - ndotv, 5));

    float3 color = diffuse + specular;
    return float4(color, 1);
}
