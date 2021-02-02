#pragma once
#include "D3D_Helper.h"
#include "EnumsForAPI.h"
#include "./Utilities/CommonInclude.h"

namespace Graphics
{
    enum UsageID : uint32_t
    {
        USAGE_DEFAULT,
        USAGE_IMMUTABLE,
        USAGE_DYNAMIC,
        USAGE_STAGING,

        USAGE_COUNT,
    };

    enum BindFlagID : uint32_t
    {
        BIND_VERTEX_BUFFER = 1 << 0,
        BIND_INDEX_BUFFER = 1 << 1,
        BIND_CONSTANT_BUFFER = 1 << 2,
        BIND_SHADER_RESOURCE = 1 << 3,
        BIND_STREAM_OUTPUT = 1 << 4,
        BIND_RENDER_TARGET = 1 << 5,
        BIND_DEPTH_STENCIL = 1 << 6,
        BIND_UNORDERED_ACCESS = 1 << 7,
    };

    enum CPUAccessFlagID
    {
        CPU_ACCESS_WRITE = 1 << 0,
        CPU_ACCESS_READ = 1 << 1,
    };

    enum ResourceMiscFlagID
    {
        RESOURCE_MISC_SHARED = 1 << 0,
        RESOURCE_MISC_TEXTURECUBE = 1 << 1,
        RESOURCE_MISC_INDIRECT_ARGS = 1 << 2,
        RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS = 1 << 3,
        RESOURCE_MISC_BUFFER_STRUCTURED = 1 << 4,
        RESOURCE_MISC_TILED = 1 << 5,
        RESOURCE_MISC_RAY_TRACING = 1 << 6,
    };

    enum IndexBufferFormat
    {
        INDEXFORMAT_16BIT,
        INDEXFORMAT_32BIT,
    };



    struct GPUBufferDesc
    {
        uint32_t ByteWidth = 0;
        uint32_t Usage = USAGE_DEFAULT;
        uint32_t BindFlags = 0;
        uint32_t CPUAccessFlags = 0;
        uint32_t MiscFlags = 0;
        uint32_t StructureByteStride = 0;
        uint32_t Format = FormatID::FORMAT_UNKNOWN;
    };

    struct SubresourceData
    {
        const void* pSysMem = nullptr;
        uint32_t SysMemPitch = 0;
        uint32_t SysMemSlicePitch = 0;
    };

    struct GPUBuffer
    {
        D3D::BufferPtr buffer;
        GPUBufferDesc  desc;

        const GPUBufferDesc& GetDesc() { return desc; }
    };
}

