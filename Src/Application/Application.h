#pragma once
#include <memory>

#include "Window.h"

class GraphicsEngine;


class Application : public Window
{
private:
    std::unique_ptr<GraphicsEngine> m_pGraphicsEngine;
    //std::unique_ptr<GameSystem> m_pGameSystem;
    

public:
    Application();


    virtual bool Init() override;
    
    virtual void Update(float elapsed_time) override;
    virtual void Render(float elapsed_time) override;
    virtual void OnSize() override;



    ~Application();

};
