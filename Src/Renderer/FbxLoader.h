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
    DirectX::XMFLOAT4X4 m_coordinate_conversion =
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
    static std::unordered_map < std::string, std::vector<MyFbxMesh>> mModelTable;
    unsigned int mNumberOfBones    = 0;
    unsigned int mNumberOfVertices = 0;

    std::unique_ptr<PerfTimer> m_pTimer;
public:
    FbxLoader();
    bool LoadFbxFile(Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const char* fbxfilename, 
        std::vector<MyFbxMesh>& mesh_container);
    //bool LoadFbxFile(D3D::DevicePtr& device,
    //    const std::string& fbxfilename,
    //    std::vector<MyFbxMesh>& mesh_container,
    //    bool dummy
    //);

    void LoadPosition(FbxMesh* mesh, std::vector<FbxInfo::Vertex>& vertices);
    void LoadNormal(FbxMesh* mesh, std::vector<FbxInfo::Vertex>& vertices);
    void LoadUV(FbxMesh* mesh, std::vector<FbxInfo::Vertex>& vertices);
    void LoadVertexColor(FbxMesh* mesh);
    void LoadBones(FbxMesh* mesh, MyFbxMesh& my_mesh);
    int  FindBones(std::string& name, MyFbxMesh& my_mesh);
    void LoadKeyFrames(std::string name, int bone, FbxNode* bone_node, FbxMesh* mesh, MyFbxMesh& my_mesh);
    void LoadMaterial(int index, FbxSurfaceMaterial* material);
    void LoadCGFX(Microsoft::WRL::ComPtr<ID3D11Device>& device, MyFbxMesh& mesh, FbxSurfaceMaterial* material, const std::string& filename);

    bool AddMotion(std::string& name, const char* filename, std::vector<MyFbxMesh>& meshes);

    void Save(const std::string filename, std::vector<MyFbxMesh>& mesh_container);
    void SaveMotion(const std::string& filename, const std::string& key, std::vector<MyFbxMesh>& meshes);

    bool LoadAAA(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::string filename, std::vector<MyFbxMesh>& mesh_container);
    bool LoadMMM(std::string& name, const std::string& filename, std::vector<MyFbxMesh>& meshes);

    void FetchBoneInfluences(
        const FbxMesh* fbx_mesh, 
        std::vector<BoneInfluencesPerControlPoint>& influences);

    //void fetchBoneMatrices(
    //    const FbxMesh* fbx_mesh,
    //    std::vector<FbxInfo::Bone>& skeltal,
    //    FbxTime time);

    //void fetchAnimations(
    //    const FbxMesh* fbx_mesh, 
    //    FbxInfo::SkeltalAnimation& skeltal_animation, 
    //    u_int sampling_rate = 0);

    ~FbxLoader();
};
