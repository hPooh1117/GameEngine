// Global Function for Compute Shader.

static const float PI = 3.14159265358979323846;
static const float INV_PI = 0.31830988618379067153;
static const float SQRT2 = 1.41421356237309504880;


inline bool IsSaturated(float a) { return a == saturate(a); }
inline bool IsSaturated(float2 a) { return IsSaturated(a.x) && IsSaturated(a.y); }
inline bool IsSaturated(float3 a) { return IsSaturated(a.x) && IsSaturated(a.y) && IsSaturated(a.z); }
inline bool IsSaturated(float4 a) { return IsSaturated(a.x) && IsSaturated(a.y) && IsSaturated(a.z) && IsSaturated(a.w);}

//---------------------------------------------------------------------------------------------------------------------------
// 2D配列インデックスを1D配列インデックスに変換
inline uint Flatten2D(uint2 coord, uint2 dim)
{
	return coord.x + coord.y * dim.x;
}

//---------------------------------------------------------------------------------------------------------------------------
// 1D配列用に変換されたインデックスを2D配列用に戻す
inline uint2 Unflatten2D(uint idx, uint2 dim)
{
	return uint2(idx % dim.x, idx / dim.x);
}

//---------------------------------------------------------------------------------------------------------------------------
// 3D配列インデックスを1D配列インデックスに変換
inline uint Flatten3D(uint3 coord, uint3 dim)
{
	return coord.x + coord.y * dim.x + coord.z * dim.x * dim.y;
}

//---------------------------------------------------------------------------------------------------------------------------
// 1D配列用に変換されたインデックスを3D配列用に戻す
inline uint3 Unflatten3D(uint idx, uint3 dim)
{
	const uint z = idx / (dim.x * dim.y);
	idx -= z * dim.x * dim.y;
	const uint y = idx / dim.x;
	const uint x = idx % dim.x;
	return uint3(x, y, z);
}

//---------------------------------------------------------------------------------------------------------------------------
// depth bufferからワールド空間positionを再構築(行列が射影の逆行列ならビュー空間positionを得る)
// uv      : スクリーン空間座標（0 ~ 1）
// z       : depth値
// InvVP   : depth bufferの値の取り出しに使用したView-Projection行列の逆行列 
inline float3 ReconstructPosition(in float2 uv, in float z, in float4x4 InvVP)
{
	float x = uv.x * 2.0 - 1.0;
	float y = (1.0 - uv.y) * 2.0 - 1.0;
	float4 positionP = float4(x, y, z, 1.0);
	float4 positionV = mul(positionP, InvVP);
	return positionV.xyz / positionV.w;
}

//---------------------------------------------------------------------------------------------------------------------------
// 0 ~ 1の範囲でrandom float valueを返す。
// seed   : 必ず０より大きい数を入れる。
inline float Rand(inout float seed, in float2 uv)
{
	float result = frac(sin(seed * dot(uv, float2(12.9898, 78.233))) * 43758.5453);
	seed += 1.0f;
	return result;
}

//---------------------------------------------------------------------------------------------------------------------------
// 半球サンプル用の2D正規化ベクトル生成関数
// idx : イテレーションインデックス
// num : 全体のイテレーション数
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
// 現在のフラグメント（ピクセル）の座標に応じて、正規化サンプルを計算
// uv          : Cubemap表面上のUVテクスチャ座標 (0 ~ 1)
// faceIndex   : Texture2DArrayにおけるCubemap表面のインデックス (0 ~ 5)
float3 UVtoCubeMap(in float2 uv, in uint faceIndex)
{
	uv = 2.0 * float2(uv.x, 1.0 - uv.y) - 1.0;

	// キューブマップ表面のindexによってベクトルを選択
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

// 正規分布で半球にポイントを作る
float3 HemispherePointUniform(float u, float v)
{
	float phi = v * 2.0 * PI;
	float cosTheta = sqrt(1.0 - u);
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}
//---------------------------------------------------------------------------------------------------------------------------

// Linear depthを射影変換後depthから計算
inline float GetLinearDepth(in float z, in float near, in float far)
{
	float z_n = 2.0 * z - 1.0;
	float lin = 2.0 * far * near / (near + far - z_n * (near - far));
	return lin;
}
//---------------------------------------------------------------------------------------------------------------------------