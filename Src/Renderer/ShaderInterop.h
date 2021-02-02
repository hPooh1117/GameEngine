#pragma once

#include "ConstantBufferMapping.h"

#ifdef __cplusplus

#include "./Utilities/CommonInclude.h"
typedef XMFLOAT2   float2;
typedef XMFLOAT3   float3;
typedef XMFLOAT4   float4;
typedef uint32_t   uint;
typedef XMUINT2    uint2;
typedef XMUINT3    uint3;
typedef XMUINT4    uint4;
typedef XMINT2     int2;
typedef XMINT3     int3;
typedef XMINT4     int4;
typedef XMMATRIX   matrix;
typedef XMFLOAT4X4 float4x4;

#define CB_GETBINDSLOT(name) __CBUFFERBINDSLOT__##name##__
#define CBUFFER(name, slot) static const int CB_GETBINDSLOT(name) = slot; struct alignas(16) name
#define CONSTANTBUFFER(name, type, slot) CBUFFER(name, slot)

#else

#define CBUFFER(name, slot) cbuffer name : register(b ## slot)
#define CONSTANTBUFFER(name, type, slot) ConstantBuffer< type > name : register(b ## slot)

#define RAWBUFFER(name,slot) ByteAddressBuffer name : register(t ## slot)
#define RWRAWBUFFER(name,slot) RWByteAddressBuffer name : register(u ## slot)

#define TYPEDBUFFER(name, type, slot) Buffer< type > name : register(t ## slot)
#define RWTYPEDBUFFER(name, type, slot) RWBuffer< type > name : register(u ## slot)

#define STRUCTUREDBUFFER(name, type, slot) StructuredBuffer< type > name : register(t ## slot)
#define RWSTRUCTUREDBUFFER(name, type, slot) RWStructuredBuffer< type > name : register(u ## slot)
#define ROVSTRUCTUREDBUFFER(name, type, slot) RasterizerOrderedStructuredBuffer< type > name : register(u ## slot)


#define TEXTURE1D(name, type, slot) Texture1D< type > name : register(t ## slot)
#define TEXTURE1DARRAY(name, type, slot) Texture1DArray< type > name : register(t ## slot)
#define RWTEXTURE1D(name, type, slot) RWTexture1D< type > name : register(u ## slot)

#define TEXTURE2D(name, type, slot) Texture2D< type > name : register(t ## slot)
#define TEXTURE2DMS(name, type, slot) Texture2DMS< type > name : register(t ## slot)
#define TEXTURE2DARRAY(name, type, slot) Texture2DArray< type > name : register(t ## slot)
#define RWTEXTURE2D(name, type, slot) RWTexture2D< type > name : register(u ## slot)
#define RWTEXTURE2DARRAY(name, type, slot) RWTexture2DArray< type > name : register(u ## slot)
#define ROVTEXTURE2D(name, type, slot) RasterizerOrderedTexture2D< type > name : register(u ## slot)

#define TEXTURECUBE(name, type, slot) TextureCube< type > name : register(t ## slot)
#define TEXTURECUBEARRAY(name, type, slot) TextureCubeArray< type > name : register(t ## slot)

#define TEXTURE3D(name, type, slot) Texture3D< type > name : register(t ## slot)
#define RWTEXTURE3D(name, type, slot) RWTexture3D< type > name : register(u ## slot)
#define ROVTEXTURE3D(name, type, slot) RasterizerOrderedTexture3D< type > name : register(u ## slot)


#define SAMPLERSTATE(name, slot) SamplerState name : register(s ## slot)
#define SAMPLERCOMPARISONSTATE(name, slot) SamplerComparisonState name : register(s ## slot)


#endif //__cplusplus