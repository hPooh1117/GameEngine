#pragma once
#include "./Renderer/Plane.h"

#include "./Utilities/Vector.h"

struct Particle
{
public:
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	float   scale;
	float   lifespan;
	float   max_lifespan;
	Vector4 color;


public:
	Particle() :position({}), velocity({}), acceleration({}), scale(1.0f), lifespan(0.0f), max_lifespan(0.0f), color({ 1, 1, 1, 1 })
	{
	}
	Particle(
		const Vector3& position, 
		const Vector3& velocity,
		const Vector3& accel,
		float scale,
		float lifespan,
		const Vector4& color) 
		:position(position), 
		velocity(velocity),
		acceleration(accel),
		scale(scale),
		lifespan(lifespan),
		max_lifespan(lifespan),
		color(color)
	{

	}
	Particle(const Particle& other)
	{
		position = other.position;
		velocity = other.velocity;
		acceleration = other.acceleration;
		scale = other.scale;
		lifespan = other.lifespan;
		max_lifespan = other.max_lifespan;
		color = other.color;
	}

	inline Particle& operator=(const Particle& other)
	{
		position = other.position;
		velocity = other.velocity;
		acceleration = other.acceleration;
		scale = other.scale;
		lifespan = other.lifespan;
		max_lifespan = other.max_lifespan;
		color = other.color;
		return *this;
	}
};