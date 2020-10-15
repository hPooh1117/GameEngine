#pragma once
#include <wrl\client.h>

#define DX11

#ifdef DX11
#include <d3d11.h>

namespace D3D
{
	using DevicePtr = Microsoft::WRL::ComPtr<ID3D11Device>;
	using DeviceContextPtr = Microsoft::WRL::ComPtr<ID3D11DeviceContext>;

	using RenderTargetVPtr = Microsoft::WRL::ComPtr<ID3D11RenderTargetView>;
	using ShaderResouceVPtr = Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>;
	using SamplerStatePtr = Microsoft::WRL::ComPtr<ID3D11SamplerState>;
	using Texture2DPtr = Microsoft::WRL::ComPtr<ID3D11Texture2D>;
	using DepthStencilVPtr = Microsoft::WRL::ComPtr<ID3D11DepthStencilView>;
	using DepthStencilPtr = Microsoft::WRL::ComPtr<ID3D11DepthStencilState>;

	using BufferPtr = Microsoft::WRL::ComPtr<ID3D11Buffer>;
	using VShaderPtr = Microsoft::WRL::ComPtr<ID3D11VertexShader>;
	using PShaderPtr = Microsoft::WRL::ComPtr<ID3D11PixelShader>;
}
#endif

