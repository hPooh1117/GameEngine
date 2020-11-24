#include "FinalPass.h"

#include "./Engine/GameSystem.h"

#include "./Renderer/DepthStencilView.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/Shader.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Texture.h"
#include "./Renderer/RenderTarget.h"

#include "./Utilities/ImGuiSelf.h"
#include "./Utilities/MyArrayFromVector.h"
#include "./Utilities/Log.h"

FinalPass::FinalPass()
{
}

void FinalPass::Initialize(D3D::DevicePtr& device)
{
	mpScreen = std::make_unique<Sprite>(device);

	InitializeCommonShader(device);
	mpDSV->Initialize(device);

}

void FinalPass::RenderResult(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{


	mpScreen->RenderScreen(p_graphics->GetImmContextPtr(), mpShaderTable.at(ShaderID::ESprite).get(), Vector2(0.5f * ENGINE.mCurrentWidth, 0.5f * ENGINE.mCurrentHeight), Vector2(ENGINE.mCurrentWidth, ENGINE.mCurrentHeight));
}

void FinalPass::RenderUI()
{
}
