#pragma once
#include <wrl\client.h>

#define DX11

#ifdef DX11
#include <d3d11.h>

namespace D3D
{
	using DevicePtr = Microsoft::WRL::ComPtr<ID3D11Device>;
	using DeviceContextPtr = Microsoft::WRL::ComPtr<ID3D11DeviceContext>;

	using RTVPtr = Microsoft::WRL::ComPtr<ID3D11RenderTargetView>;
	using SRVPtr = Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>;
	using UAVPtr = Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>;
	using DSVPtr = Microsoft::WRL::ComPtr<ID3D11DepthStencilView>;

	using SamplerStatePtr = Microsoft::WRL::ComPtr<ID3D11SamplerState>;
	using Texture2DPtr = Microsoft::WRL::ComPtr<ID3D11Texture2D>;
	using DepthStencilPtr = Microsoft::WRL::ComPtr<ID3D11DepthStencilState>;
	using SwapChainPtr = Microsoft::WRL::ComPtr<IDXGISwapChain>;

	using BufferPtr = Microsoft::WRL::ComPtr<ID3D11Buffer>;
	using VSPtr = Microsoft::WRL::ComPtr<ID3D11VertexShader>;
	using HSPtr = Microsoft::WRL::ComPtr<ID3D11HullShader>;
	using DSPtr = Microsoft::WRL::ComPtr<ID3D11DomainShader>;
	using GSPtr = Microsoft::WRL::ComPtr<ID3D11GeometryShader>;
	using CSPtr = Microsoft::WRL::ComPtr<ID3D11ComputeShader>;
	using PSPtr = Microsoft::WRL::ComPtr<ID3D11PixelShader>;
	using BlobPtr = Microsoft::WRL::ComPtr<ID3DBlob>;
	using InputLayoutPtr = Microsoft::WRL::ComPtr<ID3D11InputLayout>;
}
#endif

