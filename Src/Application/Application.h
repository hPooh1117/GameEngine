#pragma once
#include "Window.h"

class GraphicsEngine;
class GameSystem;
class InputManager;


class Application : public Window
{
private:
    std::unique_ptr<GraphicsEngine> m_pGraphicsEngine;
    //std::unique_ptr<GameSystem> m_pGameSystem;
    

public:
    Application();


    virtual bool Init();
    
    virtual void Update(float elapsed_time);
    virtual void Render(float elapsed_time);




    ~Application();

};
