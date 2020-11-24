// src:
// http://www.audentia-gestion.fr/NVIDIA/PDF/High_Quality_Graphic_Effects_using_DX11_en.pdf

SamplerState decal_sampler : register(s0);
Texture2D depth_texture : register(t4);
Texture2D      noise_texture : register(t9);
SamplerState   noise_sampler : register(s9);
RWTexture2D<float4> outputTexture : register(u0);



cbuffer CBPerAO : register(b5)
{
	row_major float4x4 inv_proj;
	row_major float4x4 inv_view;
	row_major float4x4 proj;
	row_major float4x4 inv_viewproj;
	float2   screen_size;
	float2   noise_scale;
	float    radius;
	float    power;
	float    kernelSize;
	float    ambient_bias;
	float4   sample_pos[MAX_SAMPLES];
	float    blurSize;
	float3   dummyVector;
}




[numthreads(1, 1, 1)]
void CSmain( uint3 dispatchTid : SV_DispatchThreadID )
{
	float2 uv = float2(dispatchTid.xy) / screen_size;
	float depth = depth_texture.Sample(border_sampler, uv).r;

	// done above

	float4 projP = float4(input.projPos.xy, depth, 1);
	float4 viewP = mul(projP, inv_proj);
	viewP /= viewP.w;

	//float4 wPos = mul(projP, inv_viewproj);
	//float4 wPos = mul(viewP, inv_view);
	//wPos.xyz /= wPos.w;

	float3 N = normal_texture.Sample(decal_sampler, input.texcoord).xyz;


	float3 randomVector = noise_texture.Sample(noise_sampler, input.texcoord * noise_scale).xyz;
	float3 tangent = normalize(randomVector - N * dot(randomVector, N));
	float3 bitangent = cross(N, tangent);
	row_major float3x3 TBN = float3x3(tangent, bitangent, N);

	float occlusion = 0.0;

	[unroll]
	for (unsigned int i = 0; i < (unsigned int)kernelSize; ++i)
	{
		float3 samplePos = mul(sample_pos[i].xyz, TBN);
		samplePos = samplePos * radius + viewP.xyz;

		float3 sampleDir = normalize(samplePos - viewP.xyz);

		float NdotS = max(dot(sampleDir, N), 0.0);

		float4 offset = mul(float4(samplePos, 1.0), proj);
		offset.xy /= offset.w;

		float sampleDepth = depth_texture.Sample(decal_sampler, float2(offset.x * 0.5 + 0.5, -offset.y * 0.5 + 0.5)).r;
		float4 sampleProjPos = float4(offset.xy, sampleDepth, 1);
		float4 sampleViewPos = mul(sampleProjPos, inv_proj);
		sampleDepth = sampleViewPos.z / sampleViewPos.w;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewP.z - sampleDepth));
		occlusion += rangeCheck * step(sampleDepth, samplePos.z + ambient_bias) * NdotS;
	}

	occlusion = 1.0 - (occlusion / kernelSize);
	float finalOcclusion = pow(abs(occlusion), power);


	float3 diffuse = diffuse_texture.Sample(decal_sampler, input.texcoord).xyz;
	float3 specular = specular_texture.Sample(decal_sampler, input.texcoord).xyz;
	float3 albedo = albedo_texture.Sample(decal_sampler, input.texcoord).xyz;
	float3 shadow = shadow_texture.Sample(decal_sampler, input.texcoord).xyz;
	float3 skybox = skybox_texture.Sample(decal_sampler, input.texcoord).xyz;

	output.ambient = float4(finalOcclusion, finalOcclusion, finalOcclusion, 1.0);

	output.result = float4((diffuse * output.ambient.xyz * albedo * shadow + specular * output.ambient.xyz * albedo * shadow + skybox), 1);

	return output;

}