#pragma once
#include "./RenderPass/RenderPasses.h"
#include "./RenderPass/ForwardPasses.h"
#include "./RenderPass/DeferredPasses.h"
#include "./RenderPass/PostProcessPasses.h"
#include "./RenderPass/ShadowPasses.h"
#include "./RenderPass/SSAOPasses.h"
#include "./RenderPass/MakeCubeMapPasses.h"
#include "./Engine/UIRenderer.h"
#include "./Renderer/TextureHolder.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/GraphicsDevice.h"
#include "./Engine/Settings.h"
#include <memory>

class Renderer
{
private:
    bool mbLoading = false;
    bool mbIsInitialized = false;

    bool mbDeferredRendering;
    bool mbSSAOActivated;
    bool mbCastingShadow;
    bool mbCubeMap;


    std::unique_ptr<ForwardPass>      mpForwardPass;
    std::unique_ptr<ShadowPass>       mpShadowPass;
    std::unique_ptr<PostProcessPass>  mpPostProcessPass;
    std::unique_ptr<DeferredPass>     mpDefferedPass;
    std::unique_ptr<SSAOPass>         mpSSAOPass;
    std::unique_ptr<MakeCubeMapPass>  mpCubeMapPass;

    std::unique_ptr<UIRenderer>       mpUIRenderer;
    std::unique_ptr<TextureHolder>    mpTextureHolder;
    std::unique_ptr<NewMeshRenderer>  mpMeshRenderer;

public:
	Renderer();
	~Renderer() = default;

    void SetUp(const Settings::Renderer& setting);
    void Initialize(Graphics::GraphicsDevice* device);
    void InitializeAsync(Graphics::GraphicsDevice* device);
    void Update(Graphics::GraphicsDevice* device);
    void Render(Graphics::GraphicsDevice* device, float dt);
    void RenderUI(Graphics::GraphicsDevice* device, float dt);
    void CheckShaderActivated();
    void Clear();

private:
    void RenderUIForSettings(Graphics::GraphicsDevice* device, float dt);
    void RenderUIByRenderPasses(Graphics::GraphicsDevice* device);
    void RenderUIForMainMenuBar(Graphics::GraphicsDevice* device);
    void RenderUIForScene(Graphics::GraphicsDevice* device, float dt);
    
public:
    RenderPass* GetRenderPass(unsigned int render_pass_id);
    NewMeshRenderer* GetMeshRenderer();
    UIRenderer* GetUIRenderer();
    TextureHolder* GetTextureHolderPtr();

    bool IsLoading() { return mbLoading; }
    bool IsInitialized() { return mbIsInitialized; }
    bool IsCastingShadow() { return mbCastingShadow; }
    bool IsSSAOActivated() { return mbSSAOActivated; }
    bool IsDefferedRendering() { return mbDeferredRendering; }
    bool IsMakeCubeMap() { return mbCubeMap; }

};

