#pragma once
#ifdef _DEBUG
#pragma comment(lib, "libfbxsdk-md.lib")
#else
#pragma comment(lib, "libfbxsdk-md.lib")
#endif




#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl\client.h>
#include <vector>
#include <fbxsdk.h>
#include <unordered_map>
#include "FbxMeshInfo.h"

class PerfTimer;

class FbxLoader
{
public:
    DirectX::XMFLOAT4X4 mCoordinateConversion =
    {
        1, 0, 0, 0,
        0, 0, 1, 0,
        0, 1, 0, 0,
        0, 0, 0, 1
    };

public:
    struct BoneInfluence
    {
        int index; // index of bone
        float weight; // weight of bone
    };
    typedef std::vector<BoneInfluence> BoneInfluencesPerControlPoint;

private:
    D3D11_TEXTURE2D_DESC m_texture2D_desc = {};

private:
    //static std::unordered_map < std::string, std::vector<MyFbxMesh>> mModelTable;
    UINT mNumberOfBones     = 0;
    UINT mNumberOfVertices  = 0;
    UINT mNumberOfMaterials = 0;

    std::unique_ptr<PerfTimer> m_pTimer;
public:
    FbxLoader();
    ~FbxLoader();

    bool Load(D3D::DevicePtr& device,
        const char* filename,
        std::vector<MyFbxMesh>& mesh_container);

private:
    bool LoadFbxFile(Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const char* fbxfilename, 
        std::vector<MyFbxMesh>& mesh_container);
    void TrangulateGeometries(FbxManager* manager, FbxScene* scene);
    void SearchMeshNodeRecursively(std::vector<FbxNode*>& nodes, FbxScene* scene);
    void LoadPosition(FbxMesh* mesh, std::vector<FbxInfo::Vertex>& vertices);
    void LoadNormal(FbxMesh* mesh, std::vector<FbxInfo::Vertex>& vertices);
    void LoadUV(FbxMesh* mesh, std::vector<FbxInfo::Vertex>& vertices);
    void LoadVertexColor(FbxMesh* mesh);
    void LoadBones(FbxMesh* mesh, MyFbxMesh& my_mesh);
    int  FindBones(std::string& name, MyFbxMesh& my_mesh);
    void LoadKeyFrames(std::string name, int bone, FbxNode* bone_node, FbxMesh* mesh, MyFbxMesh& my_mesh);
    void LoadMaterial(int index, FbxSurfaceMaterial* material);
    void LoadCGFX(Microsoft::WRL::ComPtr<ID3D11Device>& device, MyFbxMesh& mesh, FbxSurfaceMaterial* material, const std::string& filename);
    void FetchBoneInfluences(
        const FbxMesh* fbx_mesh,
        std::vector<BoneInfluencesPerControlPoint>& influences);

public:
    bool AddMotion(std::string& name, const char* filename, std::vector<MyFbxMesh>& meshes);

    void SerializeAndSaveMeshes(const std::string filename, std::vector<MyFbxMesh>& mesh_container);
    void SerializeAndSaveMotion(const std::string& filename, const std::string& key, std::vector<MyFbxMesh>& meshes);

    bool LoadSerializedMesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::string filename, std::vector<MyFbxMesh>& mesh_container);
    bool LoadSerializedMotion(std::string& name, const std::string& filename, std::vector<MyFbxMesh>& meshes);


    //void fetchBoneMatrices(
    //    const FbxMesh* fbx_mesh,
    //    std::vector<FbxInfo::Bone>& skeltal,
    //    FbxTime time);

    //void fetchAnimations(
    //    const FbxMesh* fbx_mesh, 
    //    FbxInfo::SkeltalAnimation& skeltal_animation, 
    //    u_int sampling_rate = 0);

};

//class NewFbxLoader
//{
//private:
//    FbxInfo::Vertex*   mpVertices;
//    UINT*              mpIndices;
//
//    FbxInfo::Texture*  mpAlbedoMaps;
//    FbxInfo::Texture*  mpNormalMaps;
//    FbxInfo::Texture*  mpHeightMaps;
//    FbxInfo::Texture*  mpMetallicMaps;
//    FbxInfo::Texture*  mpRoughnessMaps;
//    FbxInfo::Texture*  mpAOMaps;
//
//    std::map<const char*, FbxInfo::Motion> mMotionTable;
//
//public:
//    NewFbxLoader() = default;
//    ~NewFbxLoader() = default;
//
//    bool LoadAndGatherFbxMesh(
//        D3D::DevicePtr& p_device,
//        const char* fbxfilename,
//        std::vector<MyFbxMesh>& mesh_container);
//
//};