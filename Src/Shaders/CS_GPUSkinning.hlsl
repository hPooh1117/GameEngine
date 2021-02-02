// cf ) https://wickedengine.net/2017/09/09/skinning-in-compute-shader/amp/

//�@ �ȉ���3��p�ӁiC++�AHLSL���ꂼ��Ή��������́j
// �Eposition + normal + wind ��ByteAddressBuffer(uint32_t)
// �Etangent��ByteAddressBuffer(uint32_t)
// �Ebone_indices + bone_weights��ByteAddressBuffer(uint32_t)
//  
//�A normal + wind �� tangent �� bone_indices + bone_weights�͂��ꂼ��P��uint32_t�ϐ��Ɏ��܂�悤�ɕϊ��iC++���j
//  uint32_t normal_wind = 0;
//  normal_wind |= (uint32_t)((normal.x * 0.5f + 0.5f) * 255.0f) << 0;
//  normal_wind |= (uint32_t)((normal.y * 0.5f + 0.5f) * 255.0f) << 8;
//  normal_wind |= (uint32_t)((normal.z * 0.5f + 0.5f) * 255.0f) << 16;
//  normal_wind |= (uint32_t)wind << 24;
//
//  uint64_t indices, weights;
//  indices |= (uint64_t)boneIndices.x << 0;
//  indices |= (uint64_t)boneIndices.y << 16;
//  indices |= (uint64_t)boneIndices.z << 32;
//  indices |= (uint64_t)boneIndices.w << 48;
//
//  weights |= (uint64_t)(boneWeights.x * 65535.0f) << 0;
//  weights |= (uint64_t)(boneWeights.y * 65535.0f) << 16;
//  weights |= (uint64_t)(boneWeights.z * 65535.0f) << 32;
//  weights |= (uint64_t)(boneWeights.w * 65535.0f) << 48;
//
//�B 

// groupshared��GroupMemoryBarriorWithGroupSync()�ɂ��ďډ�
// cf ) http://memeplex.blog.shinobi.jp/directx11/-%E3%82%B3%E3%83%B3%E3%83%94%E3%83%A5%E3%83%BC%E3%83%88%E3%82%B7%E3%82%A7%E3%83%BC%E3%83%80%E3%83%BC-%20groupshared%E3%81%A8

StructuredBuffer<Vertex> InBuf0 : register(t0);
//StructuredBuffer<SimpleBuffer> InBuf1 : register(t1);
RWStructuredBuffer<Vertex> OutBuf : register(u0);

[numthreads(64, 64, 1)]
void CSmain(uint3 id : SV_DispatchThreadID)
{
	
}