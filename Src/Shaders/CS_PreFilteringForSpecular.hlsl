static const float PI = 3.141592;
static const float TWO_PI = PI * 2.0;
static const float EPSILON = 0.00001;

static const uint NUM_SAMPLES = 1024;
static const float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

TextureCube inputTex : register(t0);
RWTexture2DArray<float4> outputTex : register(u0);

cbuffer SpecularMapFilterSettings : register(b0)
{
	float roughness;
};

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

// LiとLoの間のハーフベクトルを返す。
float3 SampleGGX(float u1, float u2, float roughness)
{
	float alpha = roughness * roughness;
	float cosTheta = sqrt((1.0 - u2) / (1.0 + (alpha * alpha - 1.0) * u2));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	float phi = TWO_PI * u1;

	return float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

// GGX/Towbridge-Reitz Normal Distribution function
float NdfGGX(float cosLh, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
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
	case 4: ret = float3(uv.x, uv.y, 1.0); break;
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


[numthreads(32, 32, 1)]
void CSmain( uint3 ThreadID : SV_DispatchThreadID )
{
	uint outputWidth, outputHeight, outputDepth;
	outputTex.GetDimensions(outputWidth, outputHeight, outputDepth);
	if (ThreadID.x >= outputWidth || ThreadID.y >= outputHeight)
	{
		return;
	}

	// ミップマップレベル0のキューブマップのディメンションをとる
	float inputWidth, inputHeight, inputLevels;
	inputTex.GetDimensions(0, inputWidth, inputHeight, inputLevels);

	// ミップマップレベル0のキューブマップテクセルと角度を関連付ける。
	float wt = 4.0 * PI / (6 * inputWidth * inputHeight);

	float3 N = GetSamplingVector(ThreadID);
	float3 Lo = N;

	float3 B, T;
	ComputeBasisVectors(N, B, T);

	float3 color = 0;
	float weight = 0;

	// GGX NDF importanceサンプルを使ってEnvironmentMapを畳み込み(Convolve)
	for (uint i = 0; i < NUM_SAMPLES; ++i)
	{
		float2 u = SampleHammersley(i);
		float3 Lh = ConvertTangentToWorld(SampleGGX(u.x, u.y, roughness), N, B, T);

		float3 Li = 2.0 * dot(Lo, Lh) * Lh - Lo;

		float cosLi = dot(N, Li);

		if (cosLi > 0.0)
		{
			// MipMap Filtered Importance Sampling
			float cosLh = max(dot(N, Lh), 0.0);

			// pdf : 確率密度関数
			float pdf = NdfGGX(cosLh, roughness) * 0.25;

			float ws = 1.0 / (NUM_SAMPLES * pdf);

			float mipLevel = max(0.5 * log2(ws / wt) + 1.0, 0.0);

			color += inputTex.SampleLevel(decal_sampler, Li, mipLevel).rgb * cosLi;
			weight += cosLi;
		}
	}
	color /= weight;

	outputTex[ThreadID] = float4(color, 1.0);

}