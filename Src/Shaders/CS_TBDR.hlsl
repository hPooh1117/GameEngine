//src :
// https://johanmedestrom.wordpress.com/2016/03/30/tiled-deferred-shading-in-opengl-using-compute-shaders/


Texture2D<float4> gbufferTexture0 : register(t0);
Texture2D<float4> gbufferTexture1 : register(t1);
Texture2D<float4> gbufferTexture2 : register(t2);
Texture2D<float4> depthTexture : register(t3);

RWTexture2D<float4> outputTexture : register(u0);

static const uint BLOCK_SIZE = 16;


groupshared uint minDepthInt;
groupshared uint maxDepthInt;



[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CSmain(
	uint3 groupId : SV_GroupID,
	uint3 groupThreadId : SV_GroupThreadID,
	uint groupIndex : SV_GroupIndex,
	uint3 dispatchThreadId : SV_DispatchThreadID )
{
	float depth = depthTexture.Load(uint3(texcoord, 0)).r;
	uint depthInt = asuint(depth);




}