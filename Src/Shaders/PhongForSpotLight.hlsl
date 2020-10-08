#include "H_DataFormat.hlsli"
#include "H_Functions.hlsli"
//#include "H_SpotLight.hlsli"
#include "H_Light.hlsli"
#include "H_BumpMapping.hlsli"

//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1.0);
	// ワールド変換座標
	float4 worldPos = mul(P, world);
	// 射影空間に変換(列優先)
	P = mul(P, matWVP);
	// ワールド法線算出
	float3 N = mul(input.normal, (float3x3)world);
	N = normalize(N);//正規化


	// 出力値設定.
	output.position = P;
	//	output.Color = input.Color;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_pos = worldPos.xyz;
	output.w_normal = N;

	return output;
}

PS_InputBump VSmainBump(VS_Input input)
{
	PS_InputBump output = (PS_InputBump)0;

	float4 P = float4(input.position, 1.0);

	output.position = mul(P, matWVP);

	float3 wPos = mul(P, world).xyz;

	float4 N = float4(input.normal, 0.0f);
	float3 wN = mul(N, world).xyz;
	wN = normalize(wN);

	float3 vN = wN;
	float3 vB = { 0, 1, -0.001f };
	float3 vT;
	vB = normalize(vB);
	vT = normalize(cross(vB, vN));
	vB = normalize(cross(vN, vT));

	output.color = input.color;

	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_pos = wPos;

	output.v_tan = vT;
	output.v_bin = vB;
	output.v_normal = vN;

	return output;
}


//
// vertex shader(SkinnendMesh)
//
PS_Input VSmainS(VS_InputS input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1.0);
	// ワールド変換座標
	float4 worldPos = mul(P, world);
	// 射影空間に変換(列優先)
	P = mul(P, matWVP);
	// ワールド法線算出
	float3 N = mul(input.normal, (float3x3)world);
	N = normalize(N);//正規化


	// 出力値設定.
	output.position = P;
	//	output.Color = input.Color;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_pos = worldPos.xyz;
	output.w_normal = N;

	return output;
}

PS_Input VSmainSkinning(VS_InputS input)
{
	PS_Input output = (PS_Input)0;


	// スキニング処理
	float4 p = { 0, 0, 0, 0 };
	float4 n = { 0, 0, 0, 0 };
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		p += (input.bone_weights[i] * mul(float4(input.position, 1), bone_transforms[input.bone_indices[i]]));
		n += (input.bone_weights[i] * mul(float4(input.normal, 0), bone_transforms[input.bone_indices[i]]));
	}
	p.w = 1.0f;
	n.w = 0.0f;

	// ワールド変換座標
	float4 worldPos = mul(p, world);

	// ワールド法線算出
	float3 N = mul(n, world).xyz;
	N = normalize(N);//正規化


	// 出力値設定.
	output.position = mul(p, matWVP);
	//	output.Color = input.Color;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_pos = worldPos.xyz;
	output.w_normal = N;

	return output;
}

PS_InputBump VSmainSBump(VS_InputS input)
{
	PS_InputBump output = (PS_InputBump)0;

	float4 P = float4(input.position, 1.0);

	output.position = mul(P, matWVP);

	float3 wPos = mul(P, world).xyz;

	float4 N = float4(input.normal, 0.0f);
	float3 wN = mul(N, world).xyz;
	wN = normalize(wN);

	float3 vN = wN;
	float3 vB = { 0, 1, -0.001f };
	float3 vT;
	vB = normalize(vB);
	vT = normalize(cross(vB, vN));
	vB = normalize(cross(vN, vT));

	output.color = input.color;

	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_pos = wPos;

	output.v_tan = vT;
	output.v_bin = vB;
	output.v_normal = vN;

	return output;
}

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
//
// pixel shader
//
float4 PSmain(PS_Input input) : SV_TARGET
{
	float3 L = normalize(light_dir.xyz);
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	// 環境光
	float3 Ambient = ambient_color.rgb;//
	float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);
	float3 C = light_color.rgb;		//入射光(色と強さ)
	// 法線
	float3 N = normalize(input.w_normal);


	float3 Kd = float3(1, 1, 1);	//反射率
	float3 D = diffuse(N, L, C, Kd);


	// 鏡面反射
	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);


	//スポットライト
	float3 SpotLight;
	float3 SpotLightColor;
	float influence;
	float3 SpotDiffuse =(float3)0;
	float3 SpotSpecular = (float3)0;

	for (int i = 0; i < SPOTMAX; ++i)
	{
		if (spot_lights[i].type == 0.0f) continue;

		SpotLight = input.w_pos - spot_lights[i].pos.xyz;
		float d = length(SpotLight);

		float r = spot_lights[i].range;
		if (d > r) continue;

		float3 SpotFront = normalize(spot_lights[i].dir.xyz);
		SpotLight = normalize(SpotLight);

		float angle = dot(SpotLight, SpotFront);
		float area = spot_lights[i].inner_corn - spot_lights[i].outer_corn;
		
		float influence2 = spot_lights[i].inner_corn - angle;
		influence2 = saturate(1.0 - influence2 / area);
		influence = 1;
		SpotLightColor = spot_lights[i].color.rgb;
		SpotDiffuse += diffuse(N, SpotLight, SpotLightColor, Kd) * influence * influence * influence2;
		SpotSpecular += blinnPhongSpecular(N, SpotLight, SpotLightColor, E, Ks, 20) * influence * influence * influence2;
	}


	return color * input.color * float4(Ambient + D + S + SpotDiffuse + SpotSpecular, 1.0f);
}

float4 PSmainBump(PS_InputBump input) : SV_TARGET
{
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	float3 L = normalize(light_dir.xyz);
	// 環境光	
	float3 A = ambient_color.rgb;
	float3 C = light_color.rgb;

	// 接空間軸
	float3 vx = normalize(input.v_tan);
	float3 vy = normalize(input.v_bin);
	float3 vz = normalize(input.v_normal);

	float2 Tex = input.texcoord;

	// 視線接空間変換
	float3x3 inv_mat = { vx,vy,vz };
	float3 inv_E = normalize(mul(inv_mat, E));

	// 視差計算
	float H = height_texture.Sample(height_sampler, Tex).r;
	H = H * 2.0f - 1.0;
	Tex.x -= inv_E.x * H * viewOffset;
	Tex.y -= inv_E.y * H * viewOffset;


	// テクスチャ色取得
	float4 color = diffuse_texture.Sample(decal_sampler, Tex);
	float3 N = normal_texture.Sample(normal_sampler, Tex).xyz;
	N = N * 2.0f - 1.0f;

	// 法線ワールド変換
	float3x3 mat = { vx,vy,vz };
	N = normalize(mul(N,mat));

	// 拡散反射
	float3 Kd = float3(1,1,1);
	float3 D = diffuse(N, L, C, Kd);

	// 鏡面反射
	float3 Ks = float3(1,1,1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);


	//スポットライト
	float3 SpotLight;
	float3 SpotLightColor;
	float influence;
	float3 SpotDiffuse = (float3)0;
	float3 SpotSpecular = (float3)0;

	for (int i = 0; i < SPOTMAX; ++i)
	{
		if (spot_lights[i].type == 0.0f) continue;

		SpotLight = input.w_pos - spot_lights[i].pos.xyz;
		float d = length(SpotLight);

		float r = spot_lights[i].range;
		if (d > r) continue;

		float3 SpotFront = normalize(spot_lights[i].dir.xyz);
		SpotLight = normalize(SpotLight);

		float angle = dot(SpotLight, SpotFront);
		float area = spot_lights[i].inner_corn - spot_lights[i].outer_corn;

		float influence2 = spot_lights[i].inner_corn - angle;
		influence2 = saturate(1.0 - influence2 / area);
		influence = 1;
		SpotLightColor = spot_lights[i].color.rgb;
		SpotDiffuse += diffuse(N, SpotLight, SpotLightColor, Kd) * influence * influence * influence2;
		SpotSpecular += blinnPhongSpecular(N, SpotLight, SpotLightColor, E, Ks, 20) * influence * influence * influence2;
	}


	return color * input.color * float4(A + D + S + SpotDiffuse + SpotSpecular, 1.0f);
}
