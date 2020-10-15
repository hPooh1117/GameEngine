#include "ParticleManager.h"

using namespace DirectX;

ParticleManager::ParticleManager(
	Microsoft::WRL::ComPtr<ID3D11Device>& device, 
	const wchar_t* filename)
{
	m_plane_batch = std::make_unique<PlaneBatch>(device, filename, 5000);
	m_texture_filename = filename;
}

void ParticleManager::LoadTexture(
	Microsoft::WRL::ComPtr<ID3D11Device>& device, 
	const wchar_t* filename)
{

}

void ParticleManager::Emit(const Vector3& position, float scale, float lifespan, unsigned int number)
{
	for (auto i = 0u; i < number; ++i)
	{
		
		// fountain
		float x = static_cast<float>(rand() % 2000) / 1000.0f - 1.0f;
		float y = static_cast<float>(rand() % 2000) / 1000.0f - 1.0f;
		float z = static_cast<float>(rand() % 2000) / 1000.0f - 1.0f;

		Vector3 vel(0, 9.8f, 0);
		Vector3 accel(z, -9.8f + y, x);


		
		auto it = m_particles.begin();
		bool generateFlag = false;
		while (it != m_particles.end())
		{
			if (it->lifespan <= 0)
			{
				it->position = position;
				it->velocity = vel;
				it->acceleration = accel;
				it->lifespan = lifespan;
				it->max_lifespan = lifespan;
				it->scale = scale;
				it->color.w = 1.0f;
				generateFlag = true;
				break;
			}
			it++;
		}

		if (!generateFlag) m_particles.emplace_back(
			Particle(
				position,
				vel,
				accel,
				scale,
				lifespan, 
				Vector4(1.0f, 1.0f, 1.0f, 1.0f)
			)
		);

		// dust
		//float x = static_cast<float>(rand() % 10000) / 1000.0f - 5.0f;
		//float y = static_cast<float>(rand() % 10000) / 1000.0f - 3.0f;
		//float z = static_cast<float>(rand() % 10000) / 1000.0f - 5.0f;

		//float r = static_cast<float>(rand() % 5000) / 10000.0f + 0.5f;
		//float g = static_cast<float>(rand() % 5000) / 10000.0f + 0.5f;
		//float b = static_cast<float>(rand() % 5000) / 10000.0f + 0.5f;

		//Vector3 vel(0, 3.0, 0);
		//Vector3 accel(z / 3.0f, y, x / 3.0f);

		//auto it = m_particles.begin();
		//bool generateFlag = false;
		//while (it != m_particles.end())
		//{
		//	if (it->lifespan <= 0)
		//	{
		//		it->position = position + Vector3(x, y, z);
		//		it->velocity = vel;
		//		it->acceleration = accel;
		//		it->lifespan = lifespan;
		//		it->max_lifespan = lifespan;
		//		it->scale = scale;
		//		it->color.w = 1.0f;
		//		generateFlag = true;
		//		break;
		//	}
		//	it++;
		//}

		//if (!generateFlag) m_particles.emplace_back(
		//	Particle(
		//		position + Vector3(x, y, z),
		//		vel,
		//		accel,
		//		scale,
		//		lifespan,
		//		Vector4(r, g, b, 1.0f)
		//	)
		//);

	}
}

void ParticleManager::Update(float elapsed_time)
{
	for (auto& particle : m_particles)
	{
		if (particle.lifespan <= 0) continue;

		particle.velocity += particle.acceleration * elapsed_time;
		particle.position += particle.velocity * elapsed_time;
		particle.color.w = particle.lifespan / particle.max_lifespan;
		particle.lifespan--;
	}
}

void ParticleManager::Render(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context,
	const std::shared_ptr<CameraController>& camera, 
	const std::shared_ptr<Shader>& shader)
{
	m_plane_batch->Begin(imm_context, shader);
	for (auto& particle : m_particles)
	{
		if (particle.lifespan <= 0.0f) continue;
		XMMATRIX world = XMMatrixIdentity();
		world *= XMMatrixScaling(particle.scale, particle.scale, particle.scale);
		world *= XMMatrixTranslation(particle.position.x, particle.position.y, particle.position.z);
		m_plane_batch->Render(imm_context, world, camera, particle.color);
	}
	m_plane_batch->End(imm_context);
}

ParticleManager::~ParticleManager()
{
}
