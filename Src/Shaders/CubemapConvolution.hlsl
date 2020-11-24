#include "H_DataFormat.hlsli"

Texture2D environment_texture : register(t0);
SamplerState decal_sampler : register(s0);

static const float PI = 3.14159265359;

PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1);
	
	float4 N = float4(input.normal, 0);
	N = normalize(mul(N, world));

	output.position = mul(P, matWVP);
	output.color = input.color;
	output.texcoord = input.texcoord;
	output.w_pos = input.position;
	output.w_normal = N.xyz;

	return output;
}

float4 PSmain(PS_Input input) : SV_TARGET
{
	float3 normal = normalize(input.w_pos);
	float3 irradiance = (float3)0;

	// Convolution
	float3 up = float3(0, 1, 0);
	float3 right = cross(up, normal);
	up = cross(normal, right);

	const float sampleDelta = 0.025;
	int nrSamples = 0;

	[unroll]
	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian
			float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

			// tangent space to world
			float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			irradiance += environment_texture.Sample(decal_sampler, sampleVec.xy).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	
	return float4(irradiance, 1.0);
}