
// screen_size.x = group_size.x * thread_size_per_group.x
// 1280 = 
// screen_size.y = group_size.y * thread_size_per_group.y

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);
SamplerState decal_sampler : register(s0);

static const uint BLOCK_SIZE_X = 32;
static const uint BLOCK_SIZE_Y = 16;

cbuffer CBufferForCompute : register(b0)
{
	float2 gScreenSize;
	float  gParam0;
	float  gParam1;
};

// Dispatch(x, y, z) ---------------------------->>> Groupを左からx個、上からy個、手前からz個用意する。
[numthreads(BLOCK_SIZE_X, BLOCK_SIZE_Y, 1)]   // 1Groupにつき、左から(x)、上から(y)、手前から(z)を用意する。
void CSmain( 
	uint3 groupId : SV_GroupID,               // Group全体の中で左から(x)、上から(y)、手前(z)から何番目のGroupか
	uint3 groupThreadId : SV_GroupThreadID,   // Group内で左から(x)、上から(y)、手前(z)から何番目のThreadか
	uint  groupIndex : SV_GroupIndex,         // それぞれのGroupのサイズはDispatchのときに決められる。 SV_GroupThreadID.z * GroupSize.x * GroupSize.y + SV_GroupThreadID.y * GroupSize.x + SV_GroupThreadID.x
	uint3 dispatchTid : SV_DispatchThreadID ) // それぞれのThreadについて、ThreadとGroupの両方を考慮したIndexがつけられる。
{
	//float2 uv = float2(dispatchTid.xy) / gScreenSize;
	//float4 pixel = inputTexture.SampleLevel(decal_sampler, uv, 0);
	//pixel.a = 1;

	float4 pixel = inputTexture[dispatchTid.xy];
	pixel.rgb = pixel.r * 0.3 + pixel.g * 0.59 + pixel.b * 0.11;

	outputTexture[dispatchTid.xy] = pixel;
}