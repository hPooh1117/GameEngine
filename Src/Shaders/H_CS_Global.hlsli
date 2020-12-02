// Global Function for Compute Shader.

static const float PI = 3.14159265358979323846;
static const float INV_PI = 0.31830988618379067153;
static const float SQRT2 = 1.41421356237309504880;


inline bool IsSaturated(float a) { return a == saturate(a); }
inline bool IsSaturated(float2 a) { return IsSaturated(a.x) && IsSaturated(a.y); }
inline bool IsSaturated(float3 a) { return IsSaturated(a.x) && IsSaturated(a.y) && IsSaturated(a.z); }
inline bool IsSaturated(float4 a) { return IsSaturated(a.x) && IsSaturated(a.y) && IsSaturated(a.z) && IsSaturated(a.w);}

//---------------------------------------------------------------------------------------------------------------------------
// 2D�z��C���f�b�N�X��1D�z��C���f�b�N�X�ɕϊ�
inline uint Flatten2D(uint2 coord, uint2 dim)
{
	return coord.x + coord.y * dim.x;
}

//---------------------------------------------------------------------------------------------------------------------------
// 1D�z��p�ɕϊ����ꂽ�C���f�b�N�X��2D�z��p�ɖ߂�
inline uint2 Unflatten2D(uint idx, uint2 dim)
{
	return uint2(idx % dim.x, idx / dim.x);
}

//---------------------------------------------------------------------------------------------------------------------------
// 3D�z��C���f�b�N�X��1D�z��C���f�b�N�X�ɕϊ�
inline uint Flatten3D(uint3 coord, uint3 dim)
{
	return coord.x + coord.y * dim.x + coord.z * dim.x * dim.y;
}

//---------------------------------------------------------------------------------------------------------------------------
// 1D�z��p�ɕϊ����ꂽ�C���f�b�N�X��3D�z��p�ɖ߂�
inline uint3 Unflatten3D(uint idx, uint3 dim)
{
	const uint z = idx / (dim.x * dim.y);
	idx -= z * dim.x * dim.y;
	const uint y = idx / dim.x;
	const uint x = idx % dim.x;
	return uint3(x, y, z);
}

//---------------------------------------------------------------------------------------------------------------------------
// depth buffer���烏�[���h���position���č\�z(�s�񂪎ˉe�̋t�s��Ȃ�r���[���position�𓾂�)
// uv      : �X�N���[����ԍ��W�i0 ~ 1�j
// z       : depth�l
// InvVP   : depth buffer�̒l�̎��o���Ɏg�p����View-Projection�s��̋t�s�� 
inline float3 ReconstructPosition(in float2 uv, in float z, in float4x4 InvVP)
{
	float x = uv.x * 2.0 - 1.0;
	float y = (1.0 - uv.y) * 2.0 - 1.0;
	float4 positionP = float4(x, y, z, 1.0);
	float4 positionV = mul(positionP, InvVP);
	return positionV.xyz / positionV.w;
}

//---------------------------------------------------------------------------------------------------------------------------
// 0 ~ 1�͈̔͂�random float value��Ԃ��B
// seed   : �K���O���傫����������B
inline float Rand(inout float seed, in float2 uv)
{
	float result = frac(sin(seed * dot(uv, float2(12.9898, 78.233))) * 43758.5453);
	seed += 1.0f;
	return result;
}

//---------------------------------------------------------------------------------------------------------------------------
// �����T���v���p��2D���K���x�N�g�������֐�
// idx : �C�e���[�V�����C���f�b�N�X
// num : �S�̂̃C�e���[�V������
inline float2 Hammersley2D(in uint idx, in uint num)
{
	uint bits = idx;
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

	const float radicalInverse_Vdc = float(bits) * 2.3283064365386963e-10;
	
	return float2(float(idx) / float(num), radicalInverse_Vdc);
}

//---------------------------------------------------------------------------------------------------------------------------
// ���݂̃t���O�����g�i�s�N�Z���j�̍��W�ɉ����āA���K���T���v�����v�Z
// uv          : Cubemap�\�ʏ��UV�e�N�X�`�����W (0 ~ 1)
// faceIndex   : Texture2DArray�ɂ�����Cubemap�\�ʂ̃C���f�b�N�X (0 ~ 5)
float3 UVtoCubeMap(in float2 uv, in uint faceIndex)
{
	uv = 2.0 * float2(uv.x, 1.0 - uv.y) - 1.0;

	// �L���[�u�}�b�v�\�ʂ�index�ɂ���ăx�N�g����I��
	float3 ret = (float3)0;
	switch (faceIndex)
	{
	case 0: ret = float3(1.0, uv.y, -uv.x); break;
	case 1: ret = float3(-1.0, uv.y, uv.x); break;
	case 2: ret = float3(uv.x, 1.0, -uv.y); break;
	case 3: ret = float3(uv.x, -1.0, uv.y); break;
	case 4: ret = float3(uv.x, uv.y, 1.0); break;
	case 5: ret = float3(-uv.x, uv.y, -1.0); break;
	}
	return normalize(ret);
}

//---------------------------------------------------------------------------------------------------------------------------

// ���K���z�Ŕ����Ƀ|�C���g�����
float3 HemispherePointUniform(float u, float v)
{
	float phi = v * 2.0 * PI;
	float cosTheta = sqrt(1.0 - u);
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}
//---------------------------------------------------------------------------------------------------------------------------

// Linear depth���ˉe�ϊ���depth����v�Z
inline float GetLinearDepth(in float z, in float near, in float far)
{
	float z_n = 2.0 * z - 1.0;
	float lin = 2.0 * far * near / (near + far - z_n * (near - far));
	return lin;
}
//---------------------------------------------------------------------------------------------------------------------------