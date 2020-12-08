// reference:
// http://hikita12312.hatenablog.com/entry/2017/11/20/000446
//
#include "H_CS_Global.hlsli"


//#define    SAMPLE_NUM g_aTemp[0].x        ///< @brief �T���v�����O��
//#define    SAMPLE_RADIUS g_aTemp[0].y     ///< @brief �T���v�����O���a(���[���h���)
//#define    INTENSITY g_aTemp[0].z         ///< @brief ���x
//#define    EPSILON g_aTemp[0].w           ///< @brief AlchemyAO �p�����[�^
//#define    BIAS g_aTemp[1].x              ///< @brief AlchemyAO �p�����[�^
//#define    DEPTH_FACTOR_1 g_aTemp[1].y    ///< @brief �v���W�F�N�V�����s��,�p�����[�^ proj23 or proj32
//#define    DEPTH_FACTOR_2 g_aTemp[1].z    ///< @brief �v���W�F�N�V�����s��,�p�����[�^ proj22
//#define    DEPTH_FACTOR_3 g_aTemp[1].w    ///< @brief �v���W�F�N�V�����s��,�p�����[�^ proj00
//#define    DEPTH_FACTOR_4 g_aTemp[2].x    ///< @brief �v���W�F�N�V�����s��,�p�����[�^ proj11
//#define    ASPECT_RATIO g_aTemp[2].y      ///< @brief �v���W�F�N�V�����s��,�A�X�y�N�g��
//#define    NEAR_Z g_aTemp[2].z            ///< @brief �v���W�F�N�V�����s��,Near
//#define    FAR_Z g_aTemp[2].w             ///< @brief �v���W�F�N�V�����s��,Far
//#define    TEXTURE_SIZE g_aTemp[3].xy     ///< @brief (CS�̂�) �e�N�X�`���T�C�Y
//#define    TEXTURE_SIZE_X g_aTemp[3].x        ///< @brief (CS�̂�) �e�N�X�`���T�C�Y
//#define    TEXTURE_SIZE_Y g_aTemp[3].y        ///< @brief (CS�̂�) �e�N�X�`���T�C�Y

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

// 32x32�T�C�Y�̃u���b�N��2x2���ׂ��L���b�V��
groupshared float gsDepthCache[BLOCK_SIZE * BLOCK_SIZE * 4];



// ���L��������View��ԍ��W��ǂݍ���
void LoadDepthCache(uint2 cacheId, int2 cacheBlockOffset)
{
    // 2x2�e�N�Z���͈͂���`�T���v��
    float2 fetchUV = (float2(cacheId * 2 + cacheBlockOffset) + 1.0f) * screenSize_rcp;
    fetchUV = clamp(fetchUV, 0.0f, 1.0f);
    gsDepthCache[cacheId.x + cacheId.y * BLOCK_SIZE * 2] = depth_texture.SampleLevel(decal_sampler, fetchUV, 0).r;
}



/// ���L����������T���v��UV���W�̈ʒu�ɑΉ�����L���b�V�����擾
float GetDepthCache(float2 uv, int2 cacheBlockOffset)
{
    // 2x2�e�N�Z����1�̒l�Ƃ��ăL���b�V�����Ă��邱�Ƃ��l�����ăL���b�V��ID���v�Z
    uint2 cacheId = uint2(floor(uv * screenSize - cacheBlockOffset));
    cacheId = clamp(cacheId / 2, 0, BLOCK_SIZE * 2 - 1);
    return gsDepthCache[cacheId.x + cacheId.y * BLOCK_SIZE * 2];
}


/// AO�v�Z����View���Position�擾
float3 GetViewPosition(float2 uv, int2 cacheBlockOffset, float4x4 invP)
{
    const float depth = GetDepthCache(uv, cacheBlockOffset);
    return ReconstructPosition(uv, depth, invP);
}



/// @brief ���[���h��Ԕ��������̃����_���ȓ_���T���v����,XY��ԂɎˉe
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


/// @brief AO�̌v�Z
float CalculateAlchemyAO(float2 uv, int2 cacheBlockOffset = int2(0, 0))
{

    const float3 normal = normal_texture.SampleLevel(decal_sampler, uv, 0).xyz; // FIXME
    const float3 position = GetViewPosition(uv, cacheBlockOffset, inv_proj);

    /// ���[���h��Ԃł̃T���v�����O���a����,�X�N���[���X�y�[�X���a�ւ̕ϊ��W��
    // DEPTH_FACTOR_4 = 1/tan(fov/2)
    const float ry = proj._11 * radius / position.z; // ��ʂɐ����ȉ~��Y�����a
    const float2 radiusScale = ry * 0.5f * float2(screenSize.x / screenSize.y, 1.0f); // �ȉ~�X�P�[��,0.5�{��UV��Ԃւ̃}�b�v

    // AO�l�̐ϕ�
    float sum = 0.0f;
    for (uint i = 0; i < MAX_SAMPLES; ++i)
    {
        const float2 sampleUV = uv + GetRandomHemisphereXY(uv, i) * radiusScale; // �T���v������UV���W�l���v�Z
        const float3 samplePosition = GetViewPosition(sampleUV, cacheBlockOffset, inv_proj); // view��Ԃł̃T���v�����O�n�_
        const float3 v = samplePosition - position;
        sum += max(0.0f, dot(v, normal) + position.z * BIAS) / (dot(v, v) + EPSILON);
    }
    float AO = 2.0f * power * sum / MAX_SAMPLES;

    // ������AO�͔������邽�߂�,�����������z���K���l,���ꂪ�Ȃ��Ɩ������������������Ȃ�
    const float depthAttenuate = saturate((cameraFarZ - position.z) / (cameraFarZ - cameraNearZ));
    AO *= depthAttenuate;

    return 1.0f - AO;
}

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]        // 1Group�ɂ��A������(x)�A�ォ��(y)�A��O����(z)��p�ӂ���B
void CSmain(
    uint3 dispatchTid : SV_DispatchThreadID,   // ���ꂼ���Thread�ɂ��āAThread��Group�̗������l������Index��������B
    uint3 groupTid : SV_GroupThreadID,         // Group���ō�����(x)�A�ォ��(y)�A��O(z)���牽�Ԗڂ�Thread��
    uint3 groupID : SV_GroupID)                // Group�S�̂̒��ō�����(x)�A�ォ��(y)�A��O(z)���牽�Ԗڂ�Group��
{                                              
    // �L���b�V���u���b�N�̃I�t�Z�b�g
    // 2x2�e�N�Z�����Ƃɐ��`�T���v������,���a64pixel���̃T���v���܂ŉ҂�
    const int2 cacheBlockOffset = int2(groupID.xy) * BLOCK_SIZE - BLOCK_SIZE * 3 / 2;

    // ViewPosition�̃L���b�V���ǂݍ���
    LoadDepthCache(groupTid.xy, cacheBlockOffset);
    LoadDepthCache(groupTid.xy + int2(BLOCK_SIZE, 0), cacheBlockOffset);
    LoadDepthCache(groupTid.xy + int2(0, BLOCK_SIZE), cacheBlockOffset);
    LoadDepthCache(groupTid.xy + int2(BLOCK_SIZE, BLOCK_SIZE), cacheBlockOffset);


    GroupMemoryBarrierWithGroupSync();

    if ((dispatchTid.x >= (uint)screenSize.x) || (dispatchTid.y >= (uint)screenSize.y)) return;

    // AO�l�̎擾
    const float2 uv = (float2(dispatchTid.xy) + 0.5f) * screenSize_rcp;
    float ao = CalculateAlchemyAO(uv, cacheBlockOffset);
    outputTexture[dispatchTid.xy] = float4(ao, ao, ao, 1.0);
    
}
