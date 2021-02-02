static const float PI = 3.141592;
static const float TWO_PI = PI * 2.0;
static const float EPSILON = 0.00001;

static const uint NUM_SAMPLES = 32 * 1024;
static const float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

TextureCube inputTex : register(t0);
RWTexture2DArray<float4> outputTex : register(u0);

SamplerState decal_sampler :register(s0);

// src : http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// Hammersley Point Set : 超一様分布。準モンテカルロの中で最もシンプルなものの１つ。
// 半球の中心から半球表面への方向ベクトル（サンプル）をランダムに、一様的にセットする。
// Van der Corput Sequence : ファンデルコルプト数列
// 単位区間における超一様分布列の１つ。Hammersley Point Setにおいてサンプルのy座標を決定する。

// Van der Corput radical inverse の計算
float RadicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10;
}

// i番目のサンプルについてHammersley point setを行う
float2 SampleHammersley(uint i)
{
	return float2(i * INV_NUM_SAMPLES, RadicalInverse_VdC(i));
}

// シェーディング計算のためにCosine-Weightedサンプルを使用。
// このプロセスは1回のみ計算されるものなので、Compute Shaderでやることはそれほど重要なことではない。
float3 SampleHemisphere(float u1, float u2)
{
	const float u1p = sqrt(max(0.0, 1.0 - u1 * u1));
	return float3(cos(TWO_PI * u2) * u1p, sin(TWO_PI * u2) * u1p, u1);
}

// 現在のフラグメント（ピクセル）の座標に応じて、正規化サンプルを計算
// 
float3 GetSamplingVector(uint3 ThreadID)
{
	float outputWidth, outputHeight, outputDepth;
	outputTex.GetDimensions(outputWidth, outputHeight, outputDepth);

	float2 st = ThreadID.xy / float2(outputWidth, outputHeight);
	float2 uv = 2.0 * float2(st.x, 1.0 - st.y) - 1.0;

	// キューブマップ表面のindexによってベクトルを選択
	float3 ret = (float3)0;
	switch (ThreadID.z)
	{
	case 0: ret = float3(1.0, uv.y, -uv.x); break;
	case 1: ret = float3(-1.0, uv.y, uv.x); break;
	case 2: ret = float3(uv.x, 1.0, -uv.y); break;
	case 3: ret = float3(uv.x, -1.0, uv.y); break;
	case 4: ret = float3(uv.x, uv.y,  1.0); break;
	case 5: ret = float3(-uv.x, uv.y, -1.0); break;
	}
	return normalize(ret);
}

// Tangent Space --> World Space変換のために正規直交基底を計算
void ComputeBasisVectors(const float3 N, out float3 B, out float3 T)
{
	T = cross(N, float3(0.0, 1.0, 0.0));
	T = lerp(cross(N, float3(1.0, 0.0, 0.0)), T, step(EPSILON, dot(T, T)));

	T = normalize(T);
	B = normalize(cross(N, T));
}

// ある点の座標をTangentSpaceからWorldSpaceに変換
float3 ConvertTangentToWorld(const float3 v, const float3 N, const float3 B, const float3 T)
{
	return B * v.x + T * v.y + N * v.z;
}

// 半球における放射照度(Hemispherical Irradiance)のモンテカルロ積分計算
[numthreads(32, 32, 1)]
void CSmain(uint3 ThreadID : SV_DispatchThreadID)
{
	float3 N = GetSamplingVector(ThreadID);

	float3 B, T;
	ComputeBasisVectors(N, B, T);

	float3 irradiance = 0.0;
	for (uint i = 0; i < NUM_SAMPLES; ++i)
	{
		float2 u = SampleHammersley(i);
		float3 Li = ConvertTangentToWorld(SampleHemisphere(u.x, u.y), N, B, T);
		float cosTheta = max(0.0, dot(Li, N));

		irradiance += 2.0 * inputTex.SampleLevel(decal_sampler, Li, 0).rgb * cosTheta;
	}
	irradiance /= float(NUM_SAMPLES);

	outputTex[ThreadID] = float4(irradiance, 1.0);
}