#pragma once
#include <memory>

#include "Window.h"

#include "./Renderer/GraphicsDevice.h"


class Application : public Window
{
private:
    std::unique_ptr<Graphics::GraphicsDevice> mpGraphicsDevice;
    

public:
    Application();


    virtual bool Initialize() override;
    
    virtual void Update(float elapsed_time) override;
    virtual void Render(float elapsed_time) override;
    virtual void OnSize() override;



    ~Application();

};
