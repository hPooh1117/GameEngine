#pragma once
#include <vector>
#include <d3d11.h>
#include <wrl\client.h>
#include <string>
#include <unordered_map>

#include "./Renderer/GraphicsDevice.h"
#include "Particle.h"

class ParticleManager final
{
private:
	std::vector<Particle> m_particles;
	//std::unordered_map<std::wstring, PlaneBatch> m_particle_mesh;
	std::wstring m_texture_filename;
	std::unique_ptr<PlaneBatch> m_plane_batch;

public:
	ParticleManager(Graphics::GraphicsDevice* p_device, const wchar_t* filename);

	void LoadTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t* filename);

	void Emit(const Vector3& position, float scale, float lifespan, unsigned int number = 10);
	void Update(float elapsed_time);

	void Render(
		Graphics::GraphicsDevice* p_device,
		const std::shared_ptr<CameraController>& camera,
		const std::shared_ptr<Shader>& shader);

	inline unsigned int getParticleCounts() { return static_cast<unsigned int>(m_particles.size()); }

	~ParticleManager();
};