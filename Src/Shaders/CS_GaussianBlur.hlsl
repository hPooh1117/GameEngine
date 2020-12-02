
Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);
SamplerState decal_sampler : register(s0);


// GaussianBlur == Gaussian Filter
// ガウス関数を用いるとぼかしの度合いを１つのパラメータで管理できる。
// テクスチャを N * N の領域にわけて、それぞれに重みをつける。
// まずはhorizontal方向にぼかしー＞vertical方向にぼかし、というように2パスで行う。


// 3x3 Gaussian Blur
//[numthreads(BLOCK_SIZE_X, BLOCK_SIZE_Y, 1)]
//void CSmain( uint3 dispatchTid : SV_DispatchThreadID )
//{
//	float3x3 filter = (1.0 / 16.0) * float3x3(1, 2, 1, 2, 4, 2, 1, 2, 1);
//	
//	float4 ret;
//
//	for (int i = -2; i <= 2; ++i)
//	{
//		for (int j = -2; j <= 2; ++j)
//		{
//			ret += inputTexture[dispatchTid.xy + int2(i, j)] * filter[i + 1][j + 1];
//		}
//	}
//
//	outputTexture[dispatchTid.xy] = ret;
//}


//static const half KERNEL_WEIGHTS[KERNEL_RANGE] =
////----------------------------------------------------------------------------------------------------------------------------------
//#if KERNEL_RANGE == 2
//{ 0.369459, 0.31527 };
//#endif
//#if KERNEL_RANGE == 3
//{ 0.265458, 0.226523, 0.140748 };
//#endif
//#if KERNEL_RANGE == 4
//{ 0.235473, 0.200936, 0.12485, 0.056477 };
//#endif
//#if KERNEL_RANGE == 5
//#if USE_LEARNOPENGL_KERNEL
//{ 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
//#else
//{ 0.22703, 0.193731, 0.120373, 0.054452, 0.017929 };
//#endif
//#endif
//#if KERNEL_RANGE == 6
//{ 0.225096, 0.192081, 0.119348, 0.053988, 0.017776, 0.004259 };
//#endif
//#if KERNEL_RANGE == 7
//{ 0.224762, 0.191796, 0.119171, 0.053908, 0.01775, 0.004253, 0.000741 };
//#endif
//#if KERNEL_RANGE == 8
//{ 0.22472, 0.19176, 0.119148, 0.053898, 0.017747, 0.004252, 0.000741, 0.000094 };
//#endif
//#if KERNEL_RANGE == 9
//{ 0.224716, 0.191757, 0.119146, 0.053897, 0.017746, 0.004252, 0.000741, 0.000094, 0.000009 };
//#endif
//#if KERNEL_RANGE == 10
//{ 0.224716, 0.191756, 0.119146, 0.053897, 0.017746, 0.004252, 0.000741, 0.000094, 0.000009, 0.000001 };
//#endif
//#if KERNEL_RANGE == 11
//{ 0.224716, 0.191756, 0.119146, 0.053897, 0.017746, 0.004252, 0.000741, 0.000094, 0.000009, 0.000001, 0 };
//#endif
////----------------------------------------------------------------------------------------------------------------------------------



#if VERTICAL
#define PIXEL_CACHE_SIZE SCREEN_Y
#else
#define PIXEL_CACHE_SIZE SCREEN_X
#endif

//cbuffer CBuferForBlur : register(b1)
//{
//	uint   gKernelRange;
//	uint   gStrength;
//	uint   gbIsHorizontal;
//	float  gWeights[10];
//}

//groupshared half3 gColorLine[PIXEL_CACHE_SIZE];

//[numthreads(THREAD_GROUP_SIZE_X, THREAD_GROUP_SIZE_Y, THREAD_GROUP_SIZE_Z)]
//void CSmain(uint3 dispatchTid : SV_DispatchThreadID)
//{
//	// 入力テクスチャを読み込み、SharedMemoryにセーブ。
//	const uint TEXTURE_READ_COUNT = (uint)ceil(min(((float)SCREEN_X) / THREAD_GROUP_SIZE_X, ((float)SCREEN_Y) / THREAD_GROUP_SIZE_Y));
//
//	[unroll] for (uint i = 0; i < TEXTURE_READ_COUNT; ++i)
//	{
//#if HORIZONTAL
//		const uint2 outTexel = dispatchTid.xy + uint2(0, THREAD_GROUP_SIZE_Y * i);
//		if (outTexel.y >= SCREEN_Y) break;
//
//		const int idxColorLine = outTexel.y;
//#else
//		const uint2 outTexel = dispatchTid.xy + uint2(THREAD_GROUP_SIZE_X * i, 0);
//		if (outTexel.x >= SCREEN_X) break;
//
//		const int idxColorLine = outTexel.x;
//#endif
//
//
//		const float2 uv = float2(outTexel.xy) / float2(SCREEN_X, SCREEN_Y);
//		const half4 color = inputTexture.SampleLevel(decal_sampler, uv, 0);
//
//		gColorLine[idxColorLine] = color.rgb;
//		outputTexture[outTexel] = float4(0, 0, 0, 1);
//
//	}
//
//	for (uint passCount = 0; passCount < gStrength; ++passCount)
//	{
//		GroupMemoryBarrierWithGroupSync();
//
//		// Blur
//		[unroll] for (uint px = 0; px < TEXTURE_READ_COUNT; ++px)
//		{
//			uint kernelOffset = 0;
//#if VERTICAL
//			const uint2 outTexel = dispatchTid.xy + uint2(0, THREAD_GROUP_SIZE_Y * px);
//			half3 result = gColorLine[outTexel.y] * KERNEL_WEIGHTS[kernelOffset];
//#else
//			const uint2 outTexel = dispatchTid.xy + uint2(THREAD_GROUP_SIZE_X * px, 0);
//			half3 result = gColorLine[outTexel.x] * KERNEL_WEIGHTS[kernelOffset];
//#endif
//
//			[unroll] for (kernelOffset = 1; kernelOffset < KERNEL_RANGE; ++kernelOffset)
//			{
//#if HORIZONTAL
//				bool bKernelSampleOutOfBounds = (outTexel.x + kernelOffset) >= SCREEN_X;
//				if (bKernelSampleOutOfBounds)
//				{
//					result += gColorLine[SCREEN_X - 1] * KERNEL_WEIGHTS[kernelOffset];
//				}
//				else
//				{
//					result += gColorLine[outTexel.x + kernelOffset] * KERNEL_WEIGHTS[kernelOffset];
//				}
//				bKernelSampleOutOfBounds = ((outTexel.x - kernelOffset) < 0);
//				if (bKernelSampleOutOfBounds)
//				{
//					result += gColorLine[0] * KERNEL_WEIGHTS[kernelOffset];
//				}
//				else
//				{
//					result += gColorLine[outTexel.x - kernelOffset] * KERNEL_WEIGHTS[kernelOffset];
//				}
//#else
//				bool bKernelSampleOutOfBounds = (outTexel.y + kernelOffset) >= SCREEN_Y;
//				if (bKernelSampleOutOfBounds)
//				{
//					result += gColorLine[SCREEN_Y - 1] * KERNEL_WEIGHTS[kernelOffset];
//				}
//				else
//				{
//					result += gColorLine[outTexel.y + kernelOffset] * KERNEL_WEIGHTS[kernelOffset];
//				}
//
//				bKernelSampleOutOfBounds = ((outTexel.y - kernelOffset) < 0);
//				if (bKernelSampleOutOfBounds)
//				{
//					result += gColorLine[0] * KERNEL_WEIGHTS[kernelOffset];
//				}
//				else
//				{
//					result += gColorLine[outTexel.y - kernelOffset] * KERNEL_WEIGHTS[kernelOffset];
//				}
//#endif
//			}
//			outputTexture[outTexel] = half4(result, 0.0f);
//
//
//		}
//
//		// LDSを更新
//		GroupMemoryBarrierWithGroupSync();
//
//		[unroll] for (uint pixel = 0; pixel < TEXTURE_READ_COUNT; ++pixel)
//		{
//			uint2 outTexel;
//#if VERTICAL
//			outTexel = dispatchTid.xy + uint2(0, THREAD_GROUP_SIZE_Y * pixel);
//			gColorLine[outTexel.y] = outputTexture[outTexel].xyz;
//#else
//			outTexel = dispatchTid.xy + uint2(THREAD_GROUP_SIZE_X * pixel, 0);
//			gColorLine[outTexel.x] = outputTexture[outTexel].xyz;
//#endif
//		}
//
//	}
//	outputTexture[dispatchTid.xy] = float4(0, 1, 0, 1);
//}


#if GAUSSIAN_LEVEL == 0

// Sigma : 1.34, KernelSize : 9
static const int GAUSS_KERNEL = 9;
static const float gaussianWeightsNormalized[GAUSS_KERNEL] = {
	0.004112,
	0.026563,
	0.100519,
	0.223215,
	0.29118,
	0.223215,
	0.100519,
	0.026563,
	0.004112
};
static const int gaussianOffsets[GAUSS_KERNEL] = { -4,-3,-2,-1,0,1,2,3,4 };


#else
// Sigma : 6.9, KernelSize : 33
static const int GAUSS_KERNEL = 33;
static const float gaussianWeightsNormalized[GAUSS_KERNEL] = {
	0.004013,
	0.005554,
	0.007527,
	0.00999,
	0.012984,
	0.016524,
	0.020594,
	0.025133,
	0.030036,
	0.035151,
	0.040283,
	0.045207,
	0.049681,
	0.053463,
	0.056341,
	0.058141,
	0.058754,
	0.058141,
	0.056341,
	0.053463,
	0.049681,
	0.045207,
	0.040283,
	0.035151,
	0.030036,
	0.025133,
	0.020594,
	0.016524,
	0.012984,
	0.00999,
	0.007527,
	0.005554,
	0.004013
};
static const int gaussianOffsets[GAUSS_KERNEL] = {
	-16,
	-15,
	-14,
	-13,
	-12,
	-11,
	-10,
	-9,
	-8,
	-7,
	-6,
	-5,
	-4,
	-3,
	-2,
	-1,
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
};
#endif

static const int TILE_BORDER = GAUSS_KERNEL / 2;
static const int CACHE_SIZE = TILE_BORDER + KERNEL_DIMENSION + TILE_BORDER;
groupshared float4 color_cache[CACHE_SIZE];

[numthreads(KERNEL_DIMENSION, 1, 1)]
void CSmain(uint3 Gid : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
	float2 direction = float2(0.0, 0.0);
	uint2 tile_start = Gid.xy;
#if HORIZONTAL
	tile_start.x *= KERNEL_DIMENSION;
	direction.x = 1;
#else
	tile_start.y *= KERNEL_DIMENSION;
	direction.y = 1;
#endif

	int i;
	for (i = groupIndex; i < CACHE_SIZE; i += KERNEL_DIMENSION)
	{
		const float2 uv = (tile_start + 0.5f + direction * (i - TILE_BORDER)) * float2(rcp(SCREEN_X), rcp(SCREEN_Y));
		color_cache[i] = inputTexture.SampleLevel(decal_sampler, uv, 0);
	}

	GroupMemoryBarrierWithGroupSync();

	const uint2 pixel = tile_start + groupIndex * direction;
	if (pixel.x >= SCREEN_X || pixel.y >= SCREEN_Y)
	{
		return;
	}

	const uint center = TILE_BORDER + groupIndex;

	float4 color = 0;

	for (i = 0; i < GAUSS_KERNEL; ++i)
	{
		const uint sam = center + gaussianOffsets[i];
		const float4 color2 = color_cache[sam];

		color += color2 * gaussianWeightsNormalized[i];
	}

	outputTexture[pixel] = color;
}