#pragma once
#include "Scene.h"
#include <memory>

#include "./Application/Helper.h"

class Sprite;

class SceneA : public Scene
{
public:
    SceneA(SceneManager* manager, Graphics::GraphicsDevice* p_device);
    
    virtual void InitializeScene() override;
    virtual void Update(float elapsed_time) override;
    virtual void PreCompute(Graphics::GraphicsDevice* p_graphics) override;
    virtual void Render(Graphics::GraphicsDevice* p_graphics, float elapsed_time) override;
    virtual void RenderUI() override;

private:
    void Init(Graphics::GraphicsDevice* p_device);
public:
    ~SceneA();


};