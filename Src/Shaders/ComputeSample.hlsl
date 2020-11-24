struct SimpleBuffer
{
	int i;
	float f;
};


StructuredBuffer<SimpleBuffer> InBuf0 : register(t0);
//StructuredBuffer<SimpleBuffer> InBuf1 : register(t1);
RWStructuredBuffer<SimpleBuffer> OutBuf : register(u0);

[numthreads(64, 1, 1)]
void CSmain(uint3 id : SV_DispatchThreadID)
{
	OutBuf[id.x].i = InBuf0[id.x].i + InBuf0[id.x].i;
	OutBuf[id.x].f = sin(InBuf0[id.x].f) * cos(InBuf0[id.x].f);

	/*GroupMemoryBarrierWithGroupSync();*/
}