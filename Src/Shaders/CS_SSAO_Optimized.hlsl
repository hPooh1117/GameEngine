// reference:
// http://hikita12312.hatenablog.com/entry/2017/11/20/000446
//
#include "H_CS_Global.hlsli"


//#define    SAMPLE_NUM g_aTemp[0].x        ///< @brief サンプリング数
//#define    SAMPLE_RADIUS g_aTemp[0].y     ///< @brief サンプリング半径(ワールド空間)
//#define    INTENSITY g_aTemp[0].z         ///< @brief 強度
//#define    EPSILON g_aTemp[0].w           ///< @brief AlchemyAO パラメータ
//#define    BIAS g_aTemp[1].x              ///< @brief AlchemyAO パラメータ
//#define    DEPTH_FACTOR_1 g_aTemp[1].y    ///< @brief プロジェクション行列,パラメータ proj23 or proj32
//#define    DEPTH_FACTOR_2 g_aTemp[1].z    ///< @brief プロジェクション行列,パラメータ proj22
//#define    DEPTH_FACTOR_3 g_aTemp[1].w    ///< @brief プロジェクション行列,パラメータ proj00
//#define    DEPTH_FACTOR_4 g_aTemp[2].x    ///< @brief プロジェクション行列,パラメータ proj11
//#define    ASPECT_RATIO g_aTemp[2].y      ///< @brief プロジェクション行列,アスペクト比
//#define    NEAR_Z g_aTemp[2].z            ///< @brief プロジェクション行列,Near
//#define    FAR_Z g_aTemp[2].w             ///< @brief プロジェクション行列,Far
//#define    TEXTURE_SIZE g_aTemp[3].xy     ///< @brief (CSのみ) テクスチャサイズ
//#define    TEXTURE_SIZE_X g_aTemp[3].x        ///< @brief (CSのみ) テクスチャサイズ
//#define    TEXTURE_SIZE_Y g_aTemp[3].y        ///< @brief (CSのみ) テクスチャサイズ

static const uint BLOCK_SIZE = 32;
static const float BIAS = 0.001;
static const float EPSILON = 0.001;
static const uint MAX_SAMPLES = 16;

cbuffer CBPerAO : register(b5)
{
    row_major float4x4 inv_proj;
    row_major float4x4 inv_view;
    row_major float4x4 proj;
    row_major float4x4 view;

    float2   screenSize;
    float    radius;
    float    power;

    float    kernelSampleSize;
    float    cameraNearZ;
    float    cameraFarZ;
    float    kernelSampleSize_rcp;

    float2   screenSize_rcp;
    float2   noise_scale;
    float4   sample_pos[MAX_SAMPLES];
}


Texture2D<float4> depth_texture : register(t0);
Texture2D<float4> normal_texture : register(t1);
SamplerState decal_sampler : register(s0);

RWTexture2D<float4> outputTexture : register(u0);

// 32x32サイズのブロックを2x2並べたキャッシュ
groupshared float gsDepthCache[BLOCK_SIZE * BLOCK_SIZE * 4];



// 共有メモリにView空間座標を読み込み
void LoadDepthCache(uint2 cacheId, int2 cacheBlockOffset)
{
    // 2x2テクセル範囲を線形サンプル
    float2 fetchUV = (float2(cacheId * 2 + cacheBlockOffset) + 1.0f) * screenSize_rcp;
    fetchUV = clamp(fetchUV, 0.0f, 1.0f);
    gsDepthCache[cacheId.x + cacheId.y * BLOCK_SIZE * 2] = depth_texture.SampleLevel(decal_sampler, fetchUV, 0).r;
}



/// 共有メモリからサンプルUV座標の位置に対応するキャッシュを取得
float GetDepthCache(float2 uv, int2 cacheBlockOffset)
{
    // 2x2テクセルを1つの値としてキャッシュしていることを考慮してキャッシュIDを計算
    uint2 cacheId = uint2(floor(uv * screenSize - cacheBlockOffset));
    cacheId = clamp(cacheId / 2, 0, BLOCK_SIZE * 2 - 1);
    return gsDepthCache[cacheId.x + cacheId.y * BLOCK_SIZE * 2];
}


/// AO計算時のView空間Position取得
float3 GetViewPosition(float2 uv, int2 cacheBlockOffset, float4x4 invP)
{
    const float depth = GetDepthCache(uv, cacheBlockOffset);
    return ReconstructPosition(uv, depth, invP);
}



/// @brief ワールド空間半球内部のランダムな点をサンプルし,XY空間に射影
float2 GetRandomHemisphereXY(float2 uv, int seed)
{
    const float r1 = Rand(seed, uv);
    const float r2 = frac(r1 * 2558.54);
    const float r3 = frac(r2 * 1924.19);
    const float r = pow(r1, 1.0 / 3.0) * sqrt(1.0 - r3 * r3);
    const float theta = 2 * PI * r2;
    float s, c;
    sincos(theta, s, c);
    return float2(c, s) * r;
}


/// @brief AOの計算
float CalculateAlchemyAO(float2 uv, int2 cacheBlockOffset = int2(0, 0))
{

    const float3 normal = normal_texture.SampleLevel(decal_sampler, uv, 0).xyz; // FIXME
    const float3 position = GetViewPosition(uv, cacheBlockOffset, inv_proj);

    /// ワールド空間でのサンプリング半径から,スクリーンスペース半径への変換係数
    // DEPTH_FACTOR_4 = 1/tan(fov/2)
    const float ry = proj._11 * radius / position.z; // 画面に垂直な円のY軸半径
    const float2 radiusScale = ry * 0.5f * float2(screenSize.x / screenSize.y, 1.0f); // 楕円スケール,0.5倍はUV空間へのマップ

    // AO値の積分
    float sum = 0.0f;
    for (uint i = 0; i < MAX_SAMPLES; ++i)
    {
        const float2 sampleUV = uv + GetRandomHemisphereXY(uv, i) * radiusScale; // サンプルするUV座標値を計算
        const float3 samplePosition = GetViewPosition(sampleUV, cacheBlockOffset, inv_proj); // view空間でのサンプリング地点
        const float3 v = samplePosition - position;
        sum += max(0.0f, dot(v, normal) + position.z * BIAS) / (dot(v, v) + EPSILON);
    }
    float AO = 2.0f * power * sum / MAX_SAMPLES;

    // 遠くのAOは薄くするための,視錐台内部のz正規化値,これがないと無限遠方がおかしくなる
    const float depthAttenuate = saturate((cameraFarZ - position.z) / (cameraFarZ - cameraNearZ));
    AO *= depthAttenuate;

    return 1.0f - AO;
}

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]        // 1Groupにつき、左から(x)、上から(y)、手前から(z)を用意する。
void CSmain(
    uint3 dispatchTid : SV_DispatchThreadID,   // それぞれのThreadについて、ThreadとGroupの両方を考慮したIndexがつけられる。
    uint3 groupTid : SV_GroupThreadID,         // Group内で左から(x)、上から(y)、手前(z)から何番目のThreadか
    uint3 groupID : SV_GroupID)                // Group全体の中で左から(x)、上から(y)、手前(z)から何番目のGroupか
{                                              
    // キャッシュブロックのオフセット
    // 2x2テクセルごとに線形サンプルして,半径64pixel分のサンプルまで稼ぐ
    const int2 cacheBlockOffset = int2(groupID.xy) * BLOCK_SIZE - BLOCK_SIZE * 3 / 2;

    // ViewPositionのキャッシュ読み込み
    LoadDepthCache(groupTid.xy, cacheBlockOffset);
    LoadDepthCache(groupTid.xy + int2(BLOCK_SIZE, 0), cacheBlockOffset);
    LoadDepthCache(groupTid.xy + int2(0, BLOCK_SIZE), cacheBlockOffset);
    LoadDepthCache(groupTid.xy + int2(BLOCK_SIZE, BLOCK_SIZE), cacheBlockOffset);


    GroupMemoryBarrierWithGroupSync();

    if ((dispatchTid.x >= (uint)screenSize.x) || (dispatchTid.y >= (uint)screenSize.y)) return;

    // AO値の取得
    const float2 uv = (float2(dispatchTid.xy) + 0.5f) * screenSize_rcp;
    float ao = CalculateAlchemyAO(uv, cacheBlockOffset);
    outputTexture[dispatchTid.xy] = float4(ao, ao, ao, 1.0);
    
}
