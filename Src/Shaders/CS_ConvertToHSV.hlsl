
Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);
SamplerState decal_sampler : register(s0);

static const uint BLOCK_SIZE_X = 32;
static const uint BLOCK_SIZE_Y = 32;


[numthreads(BLOCK_SIZE_X, BLOCK_SIZE_Y, 1)]
void CSmain( uint3 dispatchTid : SV_DispatchThreadID )
{
	float3 color = inputTexture[dispatchTid.xy].xyz;
	
	float r = color.r, g = color.g, b = color.b;
	float h, s, v;

	float maxValue = max(r, max(g, b));
	float minValue = min(r, min(g, b));

	if (maxValue == minValue) h = 0;
	else if (minValue == b) h = 60 * (g - r) / (maxValue - minValue) + 60;
	else if (minValue == r) h = 60 * (b - g) / (maxValue - minValue) + 180;
	else if (minValue == g) h = 60 * (r - b) / (maxValue - minValue) + 300;

	s = maxValue = minValue;
	v = maxValue;

	h = fmod(h + 180, 360);

	float c = s;
	float h2 = h / 60;
	float x = c * (1 - abs(fmod(h2, 2) - 1));

	r = v - c;
	g = v - c;
	b = v - c;

	if (h2 < 1)
	{
		r += c;
		g += x;
	}
	else if (h2 < 2)
	{
		r += x;
		g += c;
	}
	else if (h2 < 3)
	{
		g += c;
		b += x;
	}
	else if (h2 < 4)
	{
		r += x;
		b += c;
	}
	else if (h2 < 5)
	{
		r += x;
		b += c;
	}
	else if (h2 < 6)
	{
		r += c;
		b += x;
	}

	outputTexture[dispatchTid.xy] = float4(r, g, b, 1);
}