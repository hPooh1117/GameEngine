#pragma once
#include "Component.h"

#include <string>
#include <vector>
#include <DirectXMath.h>
#include <unordered_map>

#include "./Engine/DataStructures.h"

//#include "./Renderer/Shader.h"
#include "./Renderer/D3D_Helper.h"
#include "./Renderer/TextureHelper.h"

#include "./RenderPass/ShaderIDTable.h"

#include "./Utilities/Vector.h"

class Shader;
class Mesh;
class MeshRenderer;
class ColliderComponent;
class Texture;
class CameraController;
class LightController;


enum MeshTypeID
{
	E_Default,
	E_BasicCube,
	E_BasicCylinder,
	E_BasicSphere,
	E_BasicCapsule,
	E_BasicLine,

	E_Plane,

	E_StaticMesh,
	E_SkinnedMesh,

	ENUM_MESH_TYPE_MAX,
};

enum FbxType
{
	EDefault,
	EUnrealEngine,
	EMaya,

	ENUM_FBXTYPE_MAX,
};

enum class ShaderUsage
{
	EMain,
	EShader,
	ECubeMap,
	ENUM_SHADER_USAGE_MAX,
};

enum class TextureConfig
{
	ENoneMap        = 0,   // Slot
	EColorMap       = 1,   // 0
	ENormalMap      = 2,   // 1
	EHeightMap      = 4,   // 2
	EMetallicMap    = 8,   // 3
	ERoughnessMap   = 16,  // 4
 	EAOMap          = 32,  // 5
	ESpecularMap    = 64,  // 6
	EEnvironmentMap = 128, // 7
	EShadowMap      = 256, // 8
	ECubeMap        = 512, // 9
};

struct TextureData
{
	TextureID id;
	unsigned int slot;
	std::wstring filename;
};


class MeshComponent : public Component
{
public:
	using MotionTable = std::unordered_map<std::string, std::wstring>;

public:
	//*** CONSTANTS ***
	static const std::string MESH_TYPE_NAMES[MeshTypeID::ENUM_MESH_TYPE_MAX];

private:
	//*** VARIABLES ***
	// DATA TABLE
	std::unordered_map<UINT, UINT>                 mShaderIDTable;
	std::vector<TextureData>                       mTextureTable;
	std::unordered_map<std::string, std::wstring>  mMotionFileTable;
	std::wstring                                   mMeshFileName; 
	unsigned int                                   mFbxType;
	UINT                                           mMeshID;
	MaterialData                                   mMaterialData;
	
	bool                                           mbIsSolid;
	std::string									   mCurrentMotionKey;
	bool										   mbChangedMotion;
	bool										   mbIsPBR;
	bool                                           mbDiffuseLight;
	bool                                           mbSpecularLight;
	int                                            mTextureConfig;
	int                                            mAnimeBlendTime;
	//*** FUNCTIONS ***

	//------------------------------------------------------
	// ÉRÉAä÷êî
	//------------------------------------------------------
public:
	MeshComponent(Actor* p_owner);
	virtual ~MeshComponent() = default;
	
	virtual bool Create() override;
	virtual void Destroy() override;
	virtual void Update(float elapsed_time) override;


	void Play(const char* name);
	void DeactivateChangedMotion() { mbChangedMotion = !mbChangedMotion; }

	void ToggleIsSolid() { mbIsSolid = !mbIsSolid; }
	//------------------------------------------------------
	// ìoò^ópä÷êî
	//------------------------------------------------------
	bool RegisterMesh(UINT mesh_type_id, UINT shader_id, const wchar_t* mesh_filename, UINT coord_system = FbxType::EDefault);
	bool RegisterTexture(const wchar_t* tex_filename, TextureConfig textureConfig);
	bool RegisterMotion(const char* name, const wchar_t* motion_filename);
	bool RegisterAdditionalShader(UINT shader_id, ShaderUsage usage);


	UINT                      GetMeshTypeID() { return mMeshID; }
	const DirectX::XMMATRIX   GetWorldMatrix();
	const DirectX::XMFLOAT4&  GetMaterialColor();
	bool                      GetIsSolid() { return mbIsSolid; }
	UINT                      GetShaderID(u_int usage);
	const std::wstring&       GetMeshFileName() { return mMeshFileName; }
	UINT                      GetFbxType() { return mFbxType; }
	std::vector<TextureData>& GetTextureTable() { return mTextureTable; }
	MotionTable&              GetMotionTable() { return mMotionFileTable; }
	const std::string&        GetCurrentName() { return mCurrentMotionKey; }
	bool					  GetWasChangedMotion() { return mbChangedMotion; }
	int                       GetAnimeBlendTime() { return mAnimeBlendTime; }
	const MaterialData&		  GetMaterialData() { return mMaterialData; }

	void SetMaterialColor(const Vector4& color);
	void SetMaterialColor(const Vector3& color);
	void SetBRDFFactors(float metalness, float roughness, const Vector3& specColor = { 1, 1, 1 });
	void RenderUI();

	static constexpr int                     GetID() { return ComponentID::kRender; }

};