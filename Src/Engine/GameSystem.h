#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <vector>


class SceneManager;
class GraphicsEngine;
class PerfTimer;

class GameSystem
{
private:
    std::unique_ptr<SceneManager> m_scene_manager;
    std::unique_ptr<PerfTimer>    m_pTimer;
public:
    GameSystem(Microsoft::WRL::ComPtr<ID3D11Device>& device);
    void update(float elapsed_time);
    void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
    ~GameSystem();
};