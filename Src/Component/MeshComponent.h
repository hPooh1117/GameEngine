#pragma once
#include "Component.h"

#include <string>
#include <vector>
#include <DirectXMath.h>
#include <unordered_map>

#include "./Engine/DataStructures.h"

//#include "./Renderer/Shader.h"
#include "./Renderer/D3D_Helper.h"
#include "./RenderPass/ShaderIDTable.h"

#include "./Utilities/Vector.h"

class Shader;
class Mesh;
class MeshRenderer;
class ColliderComponent;
class Texture;
class CameraController;
class LightController;

#pragma region OLDIMPL
//class MeshComponent : public Component
//{
//public:
//	using ShaderID = unsigned int;
//	using CameraPtr = std::shared_ptr<CameraController>;
//
//	enum MeshID : unsigned int
//	{
//		EBasicCube,
//		EBasicCylinder,
//		EBasicSphere,
//		EBasicCapsule,
//		EBasicLine,
//
//		EPlane,
//
//		EStaticMesh,
//		ESkinnedMesh,
//
//		MESH_ID_MAX,
//	};
//
//
//private:
//	//std::shared_ptr<Mesh> m_mesh;
//	std::shared_ptr<Mesh>   m_pMesh = nullptr;
//	std::shared_ptr<Shader> m_pShader = nullptr;
//	std::shared_ptr<Shader> m_pShaderForShadow = nullptr;
//
//	MeshID    mMeshId;
//	ShaderID  mShaderID;
//	Vector4   mMatColor;
//	bool      m_bIsSolid;
//	std::shared_ptr<ColliderComponent> m_pColData;
//	std::unordered_map<unsigned int, std::shared_ptr<Texture>> m_pTextureTable;
//
//private:
//	MeshComponent(const std::shared_ptr<Actor>& owner);
//
//public:
//	~MeshComponent();
//
//	static std::shared_ptr<MeshComponent> Initialize(const std::shared_ptr<Actor>& owner);
//
//	virtual bool Create() override;
//	virtual void Destroy() override;
//
//	virtual void Update(float elapsed_time);
//
//	void RenderMesh(D3D::DeviceContextPtr& imm_context, float elapsed_time, const CameraPtr& camera);
//	void RenderShadow(D3D::DeviceContextPtr& imm_context, float elapsed_time, const CameraPtr& p_camera);
//
//	bool Load(int mesh_id, const std::shared_ptr<MeshRenderer>& renderer);
//	bool Load(const char* filename, int mesh_id, const std::shared_ptr<MeshRenderer>& renderer);
//	bool Load(const wchar_t* filename, int mesh_id, const std::shared_ptr<MeshRenderer>& renderer);
//
//	bool AddShaderResource(
//		Microsoft::WRL::ComPtr<ID3D11Device>& pDevice,
//		const wchar_t* filename, 
//		unsigned int slot);
//
//	bool AddMotion(std::string name, const char* filename);
//	void Play(std::string name);
//
//	void        SetAdditionalResource(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context);
//	void        SetTopology(D3D11_PRIMITIVE_TOPOLOGY type);
//	inline void SetShader(const std::shared_ptr<Shader>& shader, bool isForShadow = false) {
//		isForShadow ? m_pShaderForShadow = shader : m_pShader = shader;
//	}
//	inline void SetColor(const Vector4& color) { mMatColor = color; }
//	inline void SetSolidMode() { m_bIsSolid = true; }
//	inline void SetWireframeMode() { m_bIsSolid = false; }
//	inline void SetColliderData(const std::shared_ptr<ColliderComponent> col) { m_pColData = col; }
//	inline std::shared_ptr<Mesh>& GetMesh() { return m_pMesh; }
//	static constexpr int          GetID() { return ComponentID::kRender; }
//	inline int                    GetMeshID() { return mMeshId; }
//	inline bool IsUsingShadow() { return m_pShaderForShadow != nullptr; }
//public:
//};
#pragma endregion

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

enum ShaderUsage
{
	EMain,
	EShader,

	ENUM_SHADER_USAGE_MAX,
};

struct TextureData
{
	unsigned int slot;
	std::wstring filename;
};


class NewMeshComponent : public Component
{
public:
	using MotionTable = std::unordered_map<std::string, std::wstring>;

public:
	//*** CONSTANTS ***
	static const std::string MESH_TYPE_NAMES[MeshTypeID::ENUM_MESH_TYPE_MAX];

private:
	//*** VARIABLES ***
	// DATA TABLE
	std::vector<ShaderType>                        mShaderIDTable;
	std::vector<TextureData>                       mTextureTable;
	std::unordered_map<std::string, std::wstring>  mMotionFileTable;
	std::wstring                                   mMeshFileName;
	unsigned int                                   mFbxType;
	MeshTypeID                                     mMeshID;
	MaterialData                                   mMaterialData;
	
	bool                                           mbIsSolid;
	std::string									   mCurrentMotionKey;
	bool										   mbChangedMotion;
	//*** FUNCTIONS ***

	//------------------------------------------------------
	// コア関数
	//------------------------------------------------------
private:
	NewMeshComponent(const std::shared_ptr<Actor>& p_owner);
public:
	virtual ~NewMeshComponent() = default;
	
	virtual bool Create() override;
	virtual void Destroy() override;
	virtual void Update(float elapsed_time) override;


	void Play(const char* name);
	void DeactivateChangedMotion() { mbChangedMotion = !mbChangedMotion; }

	void ToggleIsSolid() { mbIsSolid = !mbIsSolid; }
	//------------------------------------------------------
	// 登録用関数
	//------------------------------------------------------
	bool RegistMesh(MeshTypeID mesh_type_id, ShaderType shader_id, const wchar_t* mesh_filename, unsigned int coord_system = FbxType::EDefault);
	bool RegistTexture(const wchar_t* tex_filename, unsigned int slot);
	bool RegistMotion(const char* name, const wchar_t* motion_filename);
	bool RegistAdditionalShader(ShaderType shader_id, unsigned int usage);


	MeshTypeID                GetMeshTypeID() { return mMeshID; }
	const DirectX::XMMATRIX&  GetWorldMatrix();
	const DirectX::XMFLOAT4&  GetMaterialColor();
	bool                      GetIsSolid() { return mbIsSolid; }
	ShaderType                GetShaderID(u_int usage);
	const std::wstring&       GetMeshFileName() { return mMeshFileName; }
	unsigned int              GetFbxType() { return mFbxType; }
	std::vector<TextureData>& GetTextureTable() { return mTextureTable; }
	MotionTable&              GetMotionTable() { return mMotionFileTable; }
	const std::string&        GetCurrentName() { return mCurrentMotionKey; }
	bool					  GetWasChangedMotion() { return mbChangedMotion; }
	const MaterialData&		  GetMaterialData() { return mMaterialData; }

	void SetMaterialColor(const Vector4& color);
	void SetBRDFFactors(float metalness, float roughness, const Vector3& specColor = { 1, 1, 1 });
	void RenderUI();

	static std::shared_ptr<NewMeshComponent> Initialize(const std::shared_ptr<Actor>& p_owner);
	static constexpr int                     GetID() { return ComponentID::kRender; }

};