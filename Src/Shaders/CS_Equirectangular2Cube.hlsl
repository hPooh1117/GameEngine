static const float PI = 3.141592;
static const float TWO_PI = PI * 2.0;

Texture2D inputTex :register(t0);
RWTexture2DArray<float4> outputTex : register(u0);

SamplerState decal_sampler : register(s0);

// ���݂̃t���O�����g�i�s�N�Z���j�̍��W�ɉ����āA���K���T���v�����v�Z
// 
float3 GetSamplingVector(uint3 ThreadID)
{
	float outputWidth, outputHeight, outputDepth;
	outputTex.GetDimensions(outputWidth, outputHeight, outputDepth);

	float2 st = ThreadID.xy / float2(outputWidth, outputHeight);
	float2 uv = 2.0 * float2(st.x, 1.0 - st.y) - 1.0;

	// �L���[�u�}�b�v�\�ʂ�index�ɂ���ăx�N�g����I��
	float3 ret = (float3)0;
	switch (ThreadID.z)
	{
	case 0: ret = float3(1.0, uv.y, -uv.x); break;
	case 1: ret = float3(-1.0, uv.y, uv.x); break;
	case 2: ret = float3(uv.x, 1.0, -uv.y); break;
	case 3: ret = float3(uv.x, -1.0, uv.y); break;
	case 4: ret = float3(uv.x, uv.y, 1.0); break;
	case 5: ret = float3(-uv.x, uv.y, -1.0); break;
	}
	return normalize(ret);
}


[numthreads(32, 32, 1)]
void CSmain( uint3 ThreadID : SV_DispatchThreadID )
{
	float3 v = GetSamplingVector(ThreadID);

	float phi = atan2(v.z, v.x);
	float theta = asin(-v.y);

	float4 color = inputTex.SampleLevel(decal_sampler, float2(phi / -TWO_PI, theta / PI) + float2(0.5, 0.5), 0);

	outputTex[ThreadID] = color;
}