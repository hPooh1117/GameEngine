#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

enum class VEDType : unsigned int
{
    VED_NONE =                0b000000000000,
    VED_DEFAULT =             0b000000001111,
    VED_PRIMITIVE =           0b000000001001,
    VED_SPRITE =              0b000000001101,
    VED_GEOMETRIC_LINE =      0b000000000011,
    VED_GEOMETRIC_PRIMITIVE = 0b000110001111,
    VED_STATIC_MESH =         0b000000001111,
    VED_SKINNED_MESH =        0b000110111111,

    VED_PLANE_BATCH =         0b101000000111,
    VED_GEO_PARTICLE =        0b000001000111,
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

    PARAMETER,
    TANGENT,
    BITANGENT,

    NDC_TRANSFORM,       //Å´ for instance buffer
    TEXCOORD_TRANSFORM,
    COLOR_INSTANCE,


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