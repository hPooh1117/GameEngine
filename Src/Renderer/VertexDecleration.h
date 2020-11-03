#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

enum class VEDType : unsigned int
{
    VED_DEFAULT = 0b001111,
    VED_PRIMITIVE = 0b001001,
    VED_SPRITE = 0b001101,
    VED_GEOMETRIC_PRIMITIVE = 0b000011,
    VED_STATIC_MESH = 0b001111,
    VED_SKINNED_MESH = 0b10000111111,

    VED_PLANE_BATCH = 0b101000111,
    VED_GEO_PARTICLE = 0b1000000111,
    VED_TYPE_NUM_MAX = 8,
};

enum class SemanticsID
{
    POSITION,            //Å´ for vertex buffer
    NORMAL,
    TEXCOORD,
    COLOR,
    WEIGHTS,
    BONES,


    NDC_TRANSFORM,       //Å´ for instance buffer
    TEXCOORD_TRANSFORM,
    COLOR_INSTANCE,

    PARAMETER,
    TANGENT,

    ID_NUM_MAX,       
};


class VertexDecleration
{
public:
    static constexpr int SEMANTICS_TYPE_MAX = static_cast<int>(SemanticsID::ID_NUM_MAX);
    static const std::string SEMANTIC_NAMES[SEMANTICS_TYPE_MAX];
    static const DXGI_FORMAT SEMANTIC_FORMATS[SEMANTICS_TYPE_MAX];
public:
    static void CreateInputElements(
        Microsoft::WRL::ComPtr<ID3D11Device>& device,
        Microsoft::WRL::ComPtr<ID3D11InputLayout>& input_layout, 
        Microsoft::WRL::ComPtr<ID3DBlob>& vsBlob,
        VEDType type);
};