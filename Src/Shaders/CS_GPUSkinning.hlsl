// TODO 
// ���_�P���v�Z������@�ŉ��x��Dispatch�����@���L�͂��H
// cf ) https://wickedengine.net/2017/09/09/skinning-in-compute-shader/amp/

StructuredBuffer<Vertex> InBuf0 : register(t0);
//StructuredBuffer<SimpleBuffer> InBuf1 : register(t1);
RWStructuredBuffer<Vertex> OutBuf : register(u0);

[numthreads(64, 64, 1)]
void CSmain(uint3 id : SV_DispatchThreadID)
{
	
}