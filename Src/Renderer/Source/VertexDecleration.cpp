#include "VertexDecleration.h"
#include <vector>

#include "./Utilities/Log.h"

const std::string VertexDecleration::SEMANTIC_NAMES[VertexDecleration::SEMANTICS_TYPE_MAX] = {
	"POSITION", "NORMAL", "TEXCOORD", "COLOR",
	"WEIGHTS", "BONES", 
	"PARAMETER", "TANGENT", "BINORMAL", 
	"NDC_TRANSFORM", "TEXCOORD_TRANSFORM", "COLOR"
};

const DXGI_FORMAT VertexDecleration::SEMANTIC_FORMATS[VertexDecleration::SEMANTICS_TYPE_MAX] = {
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_UINT,
		
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
};



void VertexDecleration::CreateInputElements(
	Microsoft::WRL::ComPtr<ID3D11Device>& device,
	Microsoft::WRL::ComPtr<ID3D11InputLayout>& input_layout,
	Microsoft::WRL::ComPtr<ID3DBlob>& vsBlob,
	VEDType type)
{
	int typeInInt = static_cast<int>(type);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayouts;

	for (int i = 0; i < static_cast<int>(SemanticsID::ID_NUM_MAX); ++i)
	{
		int bit = static_cast<int>(pow(2, i));
		if (bit & typeInInt)
		{
			D3D11_INPUT_ELEMENT_DESC inputLayout = {};
			inputLayout.SemanticName = SEMANTIC_NAMES[i].c_str();
			inputLayout.SemanticIndex = 0;
			inputLayout.Format = SEMANTIC_FORMATS[i];
			inputLayout.InputSlot = ( bit & typeInInt ) >= pow(2, static_cast<int>(SemanticsID::NDC_TRANSFORM))
				? 1 
				: 0;
			inputLayout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			inputLayout.InputSlotClass = 
				((bit & typeInInt) >= pow(2, static_cast<int>(SemanticsID::NDC_TRANSFORM) )
				? D3D11_INPUT_PER_INSTANCE_DATA 
				: D3D11_INPUT_PER_VERTEX_DATA);
			inputLayout.InstanceDataStepRate = (bit & typeInInt) >= pow(2, static_cast<int>(SemanticsID::NDC_TRANSFORM))
				? 1
				: 0;

			inputLayouts.emplace_back(inputLayout);

			if ( (bit & typeInInt) == pow(2, static_cast<int>(SemanticsID::NDC_TRANSFORM)))  
			{  // NDC_TRANSFORM needs as 4times input element as usual
				for (auto j = 0; j < 3; ++j)
				{
					inputLayout = {};
					inputLayout.SemanticName = SEMANTIC_NAMES[i].c_str();
					inputLayout.SemanticIndex = j + 1;
					inputLayout.Format = SEMANTIC_FORMATS[i];
					inputLayout.InputSlot = 1;
					inputLayout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
					inputLayout.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
					inputLayout.InstanceDataStepRate = 1;

					inputLayouts.emplace_back(inputLayout);
				}
			}
		}
	}

	auto result = device->CreateInputLayout(
		inputLayouts.data(),
		inputLayouts.size(),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		input_layout.GetAddressOf()
	);

	if (FAILED(result))
	{
		Log::Error("[VED] Couldn't Create Input Layout.");
	}
}
