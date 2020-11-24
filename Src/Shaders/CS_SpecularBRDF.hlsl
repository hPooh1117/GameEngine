static const float PI = 3.141592;
static const float TWO_PI = PI * 2.0;
static const float EPSILON = 0.00001;

static const uint NUM_SAMPLES = 64 * 1024;
static const float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

// LookUp Texture
RWTexture2D<float2> LUT : register(u0);


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

float GaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

float GaSchlickGGX_IBL(float cosLi, float cosLo, float roughness)
{
	float r = roughness;
	float k = (r * r) / 2.0;
	return GaSchlickG1(cosLi, k) * GaSchlickG1(cosLo, k);
}

[numthreads(32, 32, 1)]
void CSmain( uint2 ThreadID : SV_DispatchThreadID )
{
	// LUTのディメンションをget
	float outputWidth, outputHeight;
	LUT.GetDimensions(outputWidth, outputHeight);

	// 積分用パラメータ
	float cosLo = ThreadID.x / outputWidth;
	float roughness = ThreadID.y / outputHeight;

	// ゼロ除算
	cosLo = max(cosLo, EPSILON);


	float3 Lo = float3(sqrt(1.0 - cosLo * cosLo), 0.0, cosLo);

	// Cook-Torrance BRDFの事前積分計算。
	float DFG1 = 0;
	float DFG2 = 0;
	for (uint i = 0; i < NUM_SAMPLES; ++i)
	{
		float2 u = SampleHammersley(i);

		float3 Lh = SampleGGX(u.x, u.y, roughness);

		float3 Li = 2.0 * dot(Lo, Lh) * Lh - Lo;

		float cosLi = Li.z;
		float cosLh = Lh.z;
		float cosLoLh = max(dot(Lo, Lh), 0.0);

		if (cosLi > 0.0)
		{
			float G = GaSchlickGGX_IBL(cosLi, cosLo, roughness);
			float Gv = G * cosLoLh / (cosLh * cosLo);
			float Fc = pow(1.0 - cosLoLh, 5);

			DFG1 += (1 - Fc) * Gv;
			DFG2 += Fc * Gv;
		}
	}

	LUT[ThreadID] = float2(DFG1, DFG2) * INV_NUM_SAMPLES;
}