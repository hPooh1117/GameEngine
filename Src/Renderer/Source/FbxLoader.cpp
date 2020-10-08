#include "FbxLoader.h"

#include <functional>
#include <iostream>
#include <fstream>
#include <cassert>

#include "FbxMeshInfo.h"
#include "ResourceManager.h"

#include "./Utilities/Log.h"

//--------------------------------------------------------------------------------------------------------------------------------

using namespace fbxsdk;
using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------

std::unordered_map < std::string, std::vector<MyFbxMesh>> FbxLoader::mModelTable;
const unsigned int ERROR_NUMBER = 100;

//--------------------------------------------------------------------------------------------------------------------------------

FbxLoader::FbxLoader()
{
}

//--------------------------------------------------------------------------------------------------------------------------------

bool FbxLoader::LoadFbxFile(
	Microsoft::WRL::ComPtr<ID3D11Device>& device,
	const char* fbxfilename,
	std::vector<MyFbxMesh>& mesh_container)
{




	auto it = mModelTable.find(fbxfilename);
	if (it != mModelTable.end())
	{
		mesh_container = it->second;
		return true;
	}

	std::string filename2(fbxfilename);
	unsigned int length = filename2.size();
	filename2.at(length - 1) = 'A';
	filename2.at(length - 2) = 'A';
	filename2.at(length - 3) = 'A';
	if (LoadAAA(device, filename2, mesh_container)) return true;


	// Create the FBX SDK manager
	FbxManager* manager = FbxManager::Create();

	// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h.
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

	// Create an importer.
	FbxImporter* importer = FbxImporter::Create(manager, "");

	// Initialize the importer.
	bool import_status = false;
	import_status = importer->Initialize(fbxfilename, -1, manager->GetIOSettings());
	if (!import_status)
	{
		assert(importer->GetStatus().GetErrorString());
		return false;
	}

	// Create a new scene so it can be populated by the imported file.
	FbxScene* scene = FbxScene::Create(manager, "");

	// Import the contents of the file into the scene.
	import_status = importer->Import(scene);
	if (!import_status)
	{
		assert(importer->GetStatus().GetErrorString());
		return false;
	}

	// Convert mesh, NURBS and patch into triangle mesh
	fbxsdk::FbxGeometryConverter geometry_converter(manager);
	geometry_converter.Triangulate(scene, /*replace*/true);

	// Fetch node attributes and materials under this node recursively. Currently only mesh.	
	std::vector<FbxNode*> fetched_meshes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node)
	{
		if (node)
		{
			FbxNodeAttribute* fbx_node_attribute = node->GetNodeAttribute();
			if (fbx_node_attribute)
			{
				switch (fbx_node_attribute->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					fetched_meshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
			{
				traverse(node->GetChild(i));
			}
		}
	};
	traverse(scene->GetRootNode());

	mesh_container.resize(fetched_meshes.size());


	for (size_t i = 0; i < fetched_meshes.size(); ++i)
	{
		FbxMesh* fbx_mesh = fetched_meshes.at(i)->GetMesh();
		MyFbxMesh& mesh = mesh_container.at(i);


		LoadBones(fbx_mesh, mesh);

		// get material used by the mesh
		FbxLayerElementMaterial* LEM = fbx_mesh->GetElementMaterial();
		if (LEM != nullptr)
		{
			int material_index = LEM->GetIndexArray().GetAt(0);
			FbxSurfaceMaterial* material = fbx_mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(material_index);
			const FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);


			LoadCGFX(device, mesh, material, fbxfilename);

		}

		// Fetch material properties
		const int  number_of_materials = fbx_mesh->GetNode()->GetMaterialCount();
		mesh.m_subsets.resize(number_of_materials > 0 ? number_of_materials : 1); // Unit18
		for (auto index_of_material = 0; index_of_material < number_of_materials; ++index_of_material)
		{
			const FbxSurfaceMaterial* surface_material = fbx_mesh->GetNode()->GetMaterial(index_of_material);

			const FbxProperty property = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			const FbxProperty factor = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);


			FbxInfo::Subset& subset = mesh.m_subsets.at(index_of_material);


			if (property.IsValid() && factor.IsValid())
			{
				FbxDouble3 color = property.Get<FbxDouble3>();
				double f = factor.Get<FbxDouble>();
				subset.diffuse.color.x = static_cast<float>(color[0] * f);
				subset.diffuse.color.y = static_cast<float>(color[1] * f);
				subset.diffuse.color.z = static_cast<float>(color[2] * f);
				subset.diffuse.color.w = 1.0f;
			}
			if (property.IsValid())
			{
				const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
				if (number_of_textures)
				{
					const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
					if (file_texture)
					{
						const char* texture_filename = file_texture->GetRelativeFileName();

						if (texture_filename)
						{
							wchar_t fbx_unicode[256];
							MultiByteToWideChar(CP_ACP, 0, fbxfilename, strlen(fbxfilename) + 1, fbx_unicode, 1024);
							wchar_t texture_unicode[256];
							MultiByteToWideChar(CP_ACP, 0, file_texture->GetFileName(), strlen(file_texture->GetFileName()) + 1, texture_unicode, 1024);
							ResourceManager::CreateFilenameToRefer(texture_unicode, fbx_unicode, texture_unicode);

							subset.diffuse.texture_filename = texture_unicode;
							// create "diffuse.shader_resource_view" from "filename"
							subset.diffuse.texture = std::make_shared<Texture>();
							subset.diffuse.texture->Load(device, subset.diffuse.texture_filename.c_str());

						}
						else
						{
							subset.diffuse.texture = std::make_shared<Texture>();
							subset.diffuse.texture->Load(device);

						}

					}
				}
			}
		}



		for (auto& subset : mesh.m_subsets)
		{
			if (!subset.diffuse.texture)
			{
				subset.diffuse.texture = std::make_shared<Texture>();
				subset.diffuse.texture->Load(device);
			}
		}



		// Count the faces of each material
		if (number_of_materials > 0)
		{
			// count the faces of each material
			const int number_of_polygons = fbx_mesh->GetPolygonCount();
			for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; ++index_of_polygon)
			{
				const u_int material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
				mesh.m_subsets.at(material_index).index_count += 3;
			}

			// record the offset (how many vertex)
			int offset = 0;
			for (FbxInfo::Subset& subset : mesh.m_subsets)
			{
				subset.index_start = offset;
				offset += subset.index_count;

				// this will be used as counter in the following procedures. reset to zero
				subset.index_count = 0;
			}
		}

		// Fetch mesh transform data
		FbxMatrix global_transform = fbx_mesh->GetNode()->EvaluateGlobalTransform(0);
		for (auto row = 0; row < 4; row++)
		{
			for (auto column = 0; column < 4; column++)
			{
				mesh.m_global_transform.m[row][column] = static_cast<float>(global_transform[row][column]);
			}
		}


		// Fetch mesh data
		std::vector<FbxInfo::Vertex> vertices;	// Vertex buffer
		std::vector<u_int> indices;		// Index buffer
		u_int vertex_count = 0;



		//Unit21
		std::vector<BoneInfluencesPerControlPoint> bone_influences;
		FetchBoneInfluences(fbx_mesh, bone_influences);





		FbxStringList uv_names;
		fbx_mesh->GetUVSetNames(uv_names);

		const FbxVector4* array_of_control_points = fbx_mesh->GetControlPoints();
		const int number_of_polygons = fbx_mesh->GetPolygonCount();

		indices.resize(number_of_polygons * 3);

		fbx_mesh->GenerateTangentsData(0, true);

		for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)
		{
			// Unit18
			// the material for current face
			int index_of_material = 0;
			if (number_of_materials > 0)
			{
				index_of_material = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
			}

			// the place I should hold the vertex attribute index
			FbxInfo::Subset& subset = mesh.m_subsets.at(index_of_material);
			const int index_offset = subset.index_start + subset.index_count;

			const int number_of_vertices = fbx_mesh->GetPolygonSize(index_of_polygon);


			for (int index_of_vertex = 0; index_of_vertex < number_of_vertices; index_of_vertex++)
			{
				FbxInfo::Vertex vertex = {};
				const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
				vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
				vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
				vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);
				//LoadPosition(fbx_mesh, vertices);

				if (fbx_mesh->GetElementNormalCount())
				{
					FbxVector4 normal;
					fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
					//LoadNormal(fbx_mesh, vertices);
				}

				if (fbx_mesh->GetElementUVCount())
				{
					FbxVector2 uv;
					bool unmapped_uv;
					fbx_mesh->GetPolygonVertexUV(index_of_polygon, index_of_vertex, uv_names[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}
				if (fbx_mesh->GetElementTangentCount())
				{
					int currentIndex = index_of_polygon * 3 + index_of_vertex;
						
					FbxGeometryElementTangent* tangents = fbx_mesh->GetElementTangent(0);
					if (tangents != nullptr)
					{
						FbxVector4 t = tangents->GetDirectArray().GetAt(currentIndex);
						vertex.tangent.x = static_cast<float>(t[0]);
						vertex.tangent.y = static_cast<float>(t[1]);
						vertex.tangent.z = static_cast<float>(t[2]);
					}
				}

				vertices.push_back(vertex);
				indices.push_back(vertex_count);
				indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
				vertex_count += 1;



				int vertexid = vertices.size() - 1;
				if (index_of_control_point > 0)
				{
					auto vector_size = bone_influences.at(index_of_control_point).size();
					for (auto i = 0u; i < vector_size; ++i)
					{
						vertices[vertexid].bone_indices[i] = bone_influences.at(index_of_control_point).at(i).index;
						vertices[vertexid].bone_weights[i] = bone_influences.at(index_of_control_point).at(i).weight;

					}
				}

			}
			subset.index_count += 3;
		}

		mesh.mVertices = vertices;
		mesh.mIndices = indices;
	}

	mModelTable.emplace(fbxfilename, mesh_container);

	Save(filename2, mesh_container);

	manager->Destroy();
	return true;
}

//bool FbxLoader::LoadFbxFile(
//	D3D::DevicePtr&               device, 
//	const std::string&            fbxfilename,
//	std::vector<MyFbxMesh>&       mesh_container,
//	bool                          dummy)
//{
//	auto it = mModelTable.find(fbxfilename);
//	if (it != mModelTable.end())
//	{
//		mesh_container = it->second;
//		return true;
//	}
//
//	// 最適化ファイル確認
//	std::string filename2(fbxfilename);
//	unsigned int length = filename2.size();
//	filename2.at(length - 1) = 'A';
//	filename2.at(length - 2) = 'A';
//	filename2.at(length - 3) = 'A';
//	//if (LoadAAA(device, filename2, mesh_container)) return true;
//
//	FbxManager* manager = FbxManager::Create();
//	FbxScene* scene = FbxScene::Create(manager, "");
//
//	// ファイルからシーンに読み込み
//	FbxImporter* importer = FbxImporter::Create(manager, "");
//	importer->Initialize(fbxfilename.c_str());
//	importer->Import(scene);
//	importer->Destroy();
//
//	//***********************************************************
//
//	// モデルを材質ごとに分割
//	FbxGeometryConverter fgc(manager);
//	fgc.SplitMeshesPerMaterial(scene, true);
//	fgc.Triangulate(scene, true);
//
//	mNumberOfBones = 0;
//
//	const unsigned int numberOfMeshes = scene->GetSrcObjectCount<FbxMesh>();
//
//	// 頂点数計算
//	unsigned int numberofAllVertices = 0;
//	for (auto m = 0; m < numberOfMeshes; ++m)
//	{
//		FbxMesh* mesh = scene->GetSrcObject<FbxMesh>(m);
//		unsigned int numberOfPolygonVertices = mesh->GetPolygonVertexCount();
//		numberofAllVertices += numberOfPolygonVertices;
//	}
//
//	mesh_container.resize(numberOfMeshes);
//
//	return true;
//}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::LoadPosition(FbxMesh* mesh, std::vector<FbxInfo::Vertex>& vertices)
{
	int numberOfVertices = mesh->GetPolygonVertexCount();
	
	int* index = mesh->GetPolygonVertices();

	FbxVector4* source = mesh->GetControlPoints();

	// メッシュのトランスフォーム
	FbxVector4 T = mesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 R = mesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 S = mesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot);
	FbxAMatrix TRS = FbxAMatrix(T, R, S);

	// 全頂点変換
	for (auto v = 0; v < mesh->GetControlPointsCount(); ++v)
	{
		source[v] = TRS.MultT(source[v]);
	}

	// 頂点座標読み込み
	for (auto v = 0; v < numberOfVertices; ++v)
	{
		int vindex = index[v];
		vertices.at(v).position.x = static_cast<float>(source[vindex][0]);
		vertices.at(v).position.y = static_cast<float>(source[vindex][1]);
		vertices.at(v).position.z = static_cast<float>(source[vindex][2]);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::LoadNormal(FbxMesh* mesh, std::vector<FbxInfo::Vertex>& vertices)
{
	FbxArray<FbxVector4> normal;
	mesh->GetPolygonVertexNormals(normal);
	for (auto v = 0; v < normal.Size(); ++v)
	{
		vertices[v].normal.x = static_cast<float>(normal[v][0]);
		vertices[v].normal.y = static_cast<float>(normal[v][1]);
		vertices[v].normal.z = static_cast<float>(normal[v][2]);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::LoadUV(FbxMesh* mesh, std::vector<FbxInfo::Vertex>& vertices)
{

}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::LoadVertexColor(FbxMesh* mesh)
{
}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::LoadBones(FbxMesh* mesh, MyFbxMesh& my_mesh)
{
	// Get the list of all the animation stack.
	FbxArray<FbxString*> array_of_animation_stack_names;
	mesh->GetScene()->FillAnimStackNameArray(array_of_animation_stack_names);


	// Get the number of animations.
	int number_of_animations = array_of_animation_stack_names.Size();

	if (number_of_animations > 0)
	{
		FbxString* animation_stack_name = array_of_animation_stack_names.GetAt(0);
		FbxAnimStack* current_animation_stack = mesh->GetScene()->FindMember<FbxAnimStack>(animation_stack_name->Buffer());
		mesh->GetScene()->SetCurrentAnimationStack(current_animation_stack);


		FbxTakeInfo* take_info = mesh->GetScene()->GetTakeInfo(animation_stack_name->Buffer());
		FbxLongLong start_time = take_info->mLocalTimeSpan.GetStart().Get();
		FbxLongLong end_time = take_info->mLocalTimeSpan.GetStop().Get();
		FbxLongLong fps60 = FbxTime::GetOneFrameValue(FbxTime::eFrames60);

		my_mesh.m_start_frame = static_cast<int>(start_time / fps60);
		my_mesh.m_motions["default"].number_of_frames = static_cast<int>((end_time - start_time) / fps60);

	}


	int numberOfVertices = mesh->GetPolygonVertexCount();


	const int number_of_skins = mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int index_of_skin = 0; index_of_skin < number_of_skins; ++index_of_skin)
	{
		FbxSkin* skin = static_cast<FbxSkin*>(mesh->GetDeformer(index_of_skin, FbxDeformer::eSkin));

		const int number_of_bones = skin->GetClusterCount();
		my_mesh.m_bone_data.resize(number_of_bones);

		

		for (int index_of_bone = 0; index_of_bone < number_of_bones; ++index_of_bone)
		{
			FbxInfo::BoneData& bone = my_mesh.m_bone_data.at(index_of_bone);

			FbxCluster* cluster = skin->GetCluster(index_of_bone);

			std::string boneName = cluster->GetLink()->GetName();
#ifdef _DEBUG
			Log::Info("[FBX LOADER] Fetched Bone. ( BoneName : %s )", boneName);
#endif
			//this matrix transforms coordinates of the initial pose from mesh space to global space
			FbxAMatrix transform;
			cluster->GetTransformMatrix(transform);

			bool isNewBone = false;
			int bone_no = FindBones(boneName, my_mesh);
			if (bone_no < 0)
			{
				bone_no = index_of_bone;
				mNumberOfBones++;
				isNewBone = true;
			}
			if (isNewBone)
			{
				bone.name = boneName;

				//this matrix transforms coordinates of the initialpose from bone space to global space
				FbxAMatrix linkMatrix;
				cluster->GetTransformLinkMatrix(linkMatrix);

				FbxAMatrix offset = linkMatrix.Inverse() * transform;

				for (int i = 0; i < 4; ++i)
				{
					for (int j = 0; j < 4; ++j)
					{
						bone.offset.m[i][j] = static_cast<float>(offset.Get(i, j));
					}
				}

				LoadKeyFrames("default", bone_no, cluster->GetLink(), mesh, my_mesh);
			}

			
		}
	}

	for (int i = 0; i < number_of_animations; i++)
	{
		delete array_of_animation_stack_names[i];
	}

}

//--------------------------------------------------------------------------------------------------------------------------------

int FbxLoader::FindBones(std::string& name, MyFbxMesh& my_mesh)
{
	int bone = -1;
	for (auto i = 0u; i < my_mesh.m_bone_data.size(); ++i)
	{
		if (name == my_mesh.m_bone_data[i].name)
		{
			bone = i;
			break;
		}
	}
	return bone;
}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::LoadKeyFrames(std::string name, int bone, FbxNode* bone_node, FbxMesh* mesh, MyFbxMesh& my_mesh)
{
	FbxInfo::Motion& M = my_mesh.m_motions[name];
	
	if (M.number_of_frames <= 0)
	{
		return;
	}

	M.keys.emplace_back();
	M.keys.at(bone).resize(M.number_of_frames);

	double time = my_mesh.m_start_frame * (1.0 / 60.0);

	FbxTime fbxTime;
	for (int frame = 0; frame < M.number_of_frames; ++frame)
	{
		fbxTime.SetSecondDouble(time);

		FbxMatrix b = bone_node->EvaluateGlobalTransform(fbxTime);
		FbxMatrix m = mesh->GetNode()->EvaluateGlobalTransform(fbxTime).Inverse();
		FbxMatrix mat = m * b;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				M.keys.at(bone).at(frame).m[i][j] = static_cast<float>(mat.Get(i, j));
			}
		}

		time += 1.0 / 60.0;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::LoadMaterial(int index, FbxSurfaceMaterial* material)
{
}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::LoadCGFX(
	Microsoft::WRL::ComPtr<ID3D11Device>& device,
	MyFbxMesh& mesh, FbxSurfaceMaterial* material,
	const std::string& filename)
{
	auto implementation = GetImplementation(material, FBXSDK_IMPLEMENTATION_CGFX);
	if (implementation == nullptr) return;

	auto rootTable = implementation->GetRootTable();
	auto entryCount = rootTable->GetEntryCount();

	for (auto i = 0u; i < entryCount; ++i)
	{
		auto entry = rootTable->GetEntry(i);

		auto fbxProperty = material->FindPropertyHierarchical(entry.GetSource());
		if (!fbxProperty.IsValid())
		{
			fbxProperty = material->RootProperty.FindHierarchical(entry.GetSource());
		}


		auto textureCount = fbxProperty.GetSrcObjectCount<FbxTexture>();
		if (textureCount > 0)
		{
			std::string src = entry.GetSource();
			if (src == "Maya|DiffuseTexture")
			{
				for (auto k = 0; k < textureCount; ++k)
				{
					mesh.m_subsets.emplace_back();
					FbxInfo::Subset& subset = mesh.m_subsets.back();
					//FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
					//subset.diffuse.color.x *= static_cast<float>(color[0]);
					//subset.diffuse.color.y *= static_cast<float>(color[1]);
					//subset.diffuse.color.z *= static_cast<float>(color[2]);
					unsigned int srcCount = fbxProperty.GetSrcObjectCount<FbxFileTexture>();
					for (auto j = 0u; j < srcCount; ++j)
					{
						auto tex = fbxProperty.GetSrcObject<FbxFileTexture>(j);
						std::string texName = tex->GetFileName();
						texName = texName.substr(texName.find_last_of('/') + 1);
						Log::Info("[FBX LOADER] Fetched Texture ( Name : %s )", texName);

						wchar_t fbx_unicode[256];
						MultiByteToWideChar(CP_ACP, 0, filename.c_str(), strlen(filename.c_str()) + 1, fbx_unicode, 1024);
						wchar_t texture_unicode[256];
						MultiByteToWideChar(CP_ACP, 0, tex->GetFileName(), strlen(tex->GetFileName()) + 1, texture_unicode, 1024);
						ResourceManager::CreateFilenameToRefer(texture_unicode, fbx_unicode, texture_unicode);

						subset.diffuse.texture_filename = texture_unicode;

						subset.diffuse.texture = std::make_shared<Texture>();
						subset.diffuse.texture->Load(device, subset.diffuse.texture_filename.c_str());
					}

				}
			}
			//if (src == FbxSurfaceMaterial::sDiffuseFactor)
			//{
			//	for (auto& subset : mesh.m_subsets)
			//	{
			//		double f = material->FindPropertyHierarchical(entry.GetSource()).Get<FbxDouble>();
			//		subset.diffuse.color.x *= f;
			//		subset.diffuse.color.y *= f;
			//		subset.diffuse.color.z *= f;
			//	}
			//}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

bool FbxLoader::AddMotion(std::string& name, const char* filename, std::vector<MyFbxMesh>& meshes)
{
	std::string filename2(filename);
	unsigned int length = filename2.size();
	filename2.at(length - 1) = 'M';
	filename2.at(length - 2) = 'M';
	filename2.at(length - 3) = 'M';
	if (LoadMMM(name, filename2, meshes)) return true;


	FbxManager* manager = FbxManager::Create();
	FbxScene* scene = FbxScene::Create(manager, "");

	FbxImporter* importer = FbxImporter::Create(manager, "");
	importer->Initialize(filename);
	importer->Import(scene);
	importer->Destroy();

	FbxArray<FbxString*> names;
	scene->FillAnimStackNameArray(names);
	
	int startFrame = 0;
	int numFrame = 0;

	if (names != NULL)
	{
		FbxTakeInfo* take = scene->GetTakeInfo(names[0]->Buffer());
		FbxLongLong start = take->mLocalTimeSpan.GetStart().Get();
		FbxLongLong stop = take->mLocalTimeSpan.GetStop().Get();
		FbxLongLong fps60 = FbxTime::GetOneFrameValue(FbxTime::eFrames60);
		startFrame = static_cast<int>(start / fps60);
		numFrame = static_cast<int>((stop - start) / fps60);
	}
	int number_of_animations = names.Size();

	

	// Fetch node attributes and materials under this node recursively. Currently only mesh.	
	std::vector<FbxNode*> fetched_meshes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node)
	{
		if (node)
		{
			FbxNodeAttribute* fbx_node_attribute = node->GetNodeAttribute();
			if (fbx_node_attribute)
			{
				switch (fbx_node_attribute->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					fetched_meshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
			{
				traverse(node->GetChild(i));
			}
		}
	};
	traverse(scene->GetRootNode());

	//FbxMesh *fbx_mesh = fetched_meshes.at(0)->GetMesh();  // Currently only one mesh.
	//meshes.resize(fetched_meshes.size());
	for (size_t i = 0; i < fetched_meshes.size(); ++i)
	{
		FbxMesh* fbx_mesh = fetched_meshes.at(i)->GetMesh();
		MyFbxMesh& mesh = meshes.at(i);
		mesh.m_start_frame = startFrame;
		mesh.m_motions[name].number_of_frames = numFrame;

		FbxNode* root = scene->GetRootNode();

		for (auto b = 0u; b < mesh.m_bone_data.size(); ++b)
		{
			FbxNode* bone = root->FindChild(mesh.m_bone_data.at(b).name.c_str());

			if (bone == NULL) continue;

			LoadKeyFrames(name, b, bone, fbx_mesh, mesh);
		}
	}
	scene->Destroy();
	manager->Destroy();
	for (int i = 0; i < number_of_animations; i++)
	{
		delete names[i];
	}
	SaveMotion(filename2, name, meshes);

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::Save(const std::string filename, std::vector<MyFbxMesh>& mesh_container)
{
	auto fout = std::ofstream();
	auto flag = ios::out | ios::binary | ios::trunc;

	// start to write file
	fout.open(filename, flag);
	if (!fout)
	{
		Log::Error("[FBX LOADER] Could not Save Mesh.");
		return;
	}
	Log::Info("[FBX LOADER] Start Saving Mesh");
	Log::Info("[FBX LOADER] Name : %s", filename);

	// Save the number of meshes
	unsigned int numberOfMeshes = mesh_container.size();

	fout.write((char*)&numberOfMeshes, sizeof(unsigned int));
	// Save the meshes
	for (auto& mesh : mesh_container)
	{
		// Save the number of vertices
		unsigned int numberOfVertices = mesh.mVertices.size();

		fout.write((char*)&numberOfVertices, sizeof(unsigned int));

		// Save the vertices
		for (auto& vertex : mesh.mVertices)
		{
			fout.write((char*)&vertex, sizeof(FbxInfo::Vertex));
		}

		Log::Info("[FBX LOADER] Save Complete!! VERTICES (Size : %d)", numberOfVertices);

		// Save the number of indices
		unsigned int numberOfIndices = mesh.mIndices.size();

		fout.write((char*)&numberOfIndices, sizeof(unsigned int));

		// Save the indices
		for (auto& index : mesh.mIndices)
		{
			fout.write((char*)&index, sizeof(u_int));
		}

		Log::Info("[FBX LOADER] Save Complete!! INDICES (Size : %d)", numberOfIndices);

		// Save the global transform
		fout.write((char*)&mesh.m_global_transform, sizeof(DirectX::XMFLOAT4X4));

		Log::Info("[FBX LOADER] Save Complete!! GLOBAL_TRANSFORM_MATRIX");

		// Save the number of subsets
		unsigned int numberOfSubsets = mesh.m_subsets.size();
		fout.write((char*)&numberOfSubsets, sizeof(unsigned int));

		if (numberOfSubsets)
		{
			// Save the Subset::subset
			for (auto& subset : mesh.m_subsets)
			{
				fout.write((char*)&subset.index_start, sizeof(u_int));
				fout.write((char*)&subset.index_count, sizeof(u_int));

				// Save the length of texture filename 
				unsigned int lengthOfName = 0;
				if (subset.diffuse.texture_filename.size())
				{
					lengthOfName = subset.diffuse.texture_filename.size();
				}
				fout.write((char*)&lengthOfName, sizeof(unsigned int));

				// Save the texture file name
				if (lengthOfName)
				{
					fout.write((char*)(subset.diffuse.texture_filename.data()), (sizeof(wchar_t)) * lengthOfName);

					Log::Info("[FBX LOADER] Save Complete!! TEXTURE (Name : %s)", subset.diffuse.texture_filename);
				}

				// Save the color information
				fout.write((char*)&subset.diffuse.color, sizeof(DirectX::XMFLOAT4));
			}


		}
		// Save the number of bone data
		unsigned int numberOfBoneData = mesh.m_bone_data.size();
		fout.write((char*)&numberOfBoneData, sizeof(unsigned int));

		if (numberOfBoneData)
		{
			for (auto& bonedata : mesh.m_bone_data)
			{
				fout.write((char*)&bonedata.transform, sizeof(DirectX::XMFLOAT4X4));

				unsigned int lengthOfName = 0;

				if (bonedata.name.size())
				{
					lengthOfName = bonedata.name.size();
				}
				fout.write((char*)&lengthOfName, sizeof(unsigned int));

				if (lengthOfName)
				{
					fout.write((char*)bonedata.name.data(), sizeof(char) * lengthOfName);

					Log::Info("[FBX LOADER] Save Complete!! BONE (Name : %s)", bonedata.name);
				}


				fout.write((char*)&bonedata.offset, sizeof(DirectX::XMFLOAT4X4));

			}
		}

		// fetch only "default" motions
		auto it = mesh.m_motions.find("default");

		if (it != mesh.m_motions.end())
		{
			unsigned int lengthOfMotionName = it->first.size();
			fout.write((char*)&lengthOfMotionName, sizeof(unsigned int));

			fout.write((char*)it->first.data(), sizeof(char)* lengthOfMotionName);

			fout.write((char*)&it->second.number_of_frames, sizeof(int));

			fout.write((char*)&it->second.sampling_time, sizeof(float));

			unsigned int numberOfKeys = it->second.keys.size();

			fout.write((char*)&numberOfKeys, sizeof(unsigned int));
			
			if (numberOfKeys)
			{
				for (auto& key : it->second.keys)
				{
					unsigned int numberOfKeyFrames = key.size();

					fout.write((char*)&numberOfKeyFrames, sizeof(unsigned int));

					if (numberOfKeyFrames)
					{
						for (auto& frame : key)
						{
							fout.write((char*)&frame, sizeof(DirectX::XMFLOAT4X4));
						}

					}
				}

				Log::Info("[FBX LOADER] Save Complete!! MOTION");

			}

		}
		else
		{
			unsigned int dummy = 0;
			fout.write((char*)&dummy, sizeof(unsigned int));
		}


	}
	fout.close();
}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::SaveMotion(const std::string& filename, const std::string& key, std::vector<MyFbxMesh>& meshes)
{
	auto fout = std::ofstream();
	auto flag = ios::out | ios::binary | ios::trunc;

	// start to write file
	fout.open(filename, flag);
	if (!fout)
	{
		Log::Error("[FBX LOADER] Could not Save Mesh.");
		return;
	}

	Log::Info("[FBX LOADER] Start Saving Mesh");
	Log::Info("[FBX LOADER] Name : %s", filename);

	

	for (auto& mesh : meshes)
	{
		auto it = mesh.m_motions.find(key);

		if (it != mesh.m_motions.end())
		{
			FbxInfo::Motion& motion = it->second;
			
			fout.write((char*)&motion.number_of_frames, sizeof(int));

			fout.write((char*)&motion.sampling_time, sizeof(float));

			unsigned int numberOfKeys = motion.keys.size();
			fout.write((char*)&numberOfKeys, sizeof(unsigned int));

			if (numberOfKeys)
			{
				for (auto& key : motion.keys)
				{
					unsigned int numberOfKeyFrames = key.size();

					fout.write((char*)&numberOfKeyFrames, sizeof(unsigned int));

					if (numberOfKeyFrames)
					{
						for (auto& frame : key)
						{
							fout.write((char*)&frame, sizeof(DirectX::XMFLOAT4X4));
						}
					}
				}

				Log::Info("[FBX LOADER] Save Complete!! MOTION");

			}
		}
	}

	fout.close();
}

//--------------------------------------------------------------------------------------------------------------------------------

bool FbxLoader::LoadAAA(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::string filename, std::vector<MyFbxMesh>& mesh_container)
{
	auto fin = std::ifstream();
	auto flag = ios::in | ios::binary;

	// start to write file
	fin.open(filename, flag);
	if (!fin)
	{
		Log::Error("[FBX LOADER] Refered File doesn't exist. (%s)", filename);

		return false;
	}

	// fetch the number of meshes
	unsigned int numberOfMeshes;
	fin.read((char*)&numberOfMeshes, sizeof(unsigned int));

	mesh_container.resize(numberOfMeshes);

	// fetch the meshes
	for (auto& mesh : mesh_container)
	{
		// fetch the number of vertices
		unsigned int numberOfVertices;
		fin.read((char*)&numberOfVertices, sizeof(unsigned int));

		mesh.mVertices.resize(numberOfVertices);

		// fetch the vertices
		for (auto& vertex : mesh.mVertices)
		{
			fin.read((char*)&vertex, sizeof(FbxInfo::Vertex));
		}
		// fetch the number of indices
		unsigned int numberOfIndices;
		fin.read((char*)&numberOfIndices, sizeof(unsigned int));

		mesh.mIndices.resize(numberOfIndices);

		// fetch the indices
		for (auto& index : mesh.mIndices)
		{
			fin.read((char*)&index, sizeof(u_int));
		}


		// fetch the global transform
		fin.read((char*)&(mesh.m_global_transform), sizeof(DirectX::XMFLOAT4X4));

		// fetch the number of subsets
		unsigned int numberOfSubsets;
		fin.read((char*)&numberOfSubsets, sizeof(unsigned int));


		if (numberOfSubsets)
		{
			mesh.m_subsets.resize(numberOfSubsets);

			// fetch the Subset::subset
			for (auto& subset : mesh.m_subsets)
			{
				fin.read((char*)&subset.index_start, sizeof(u_int));
				fin.read((char*)&subset.index_count, sizeof(u_int));
				// fetch the length of texture filename
				unsigned int lengthOfName;
				fin.read((char*)&lengthOfName, sizeof(unsigned int));

				subset.diffuse.texture = std::make_shared<Texture>();
				// fetch the texture file name
				if (lengthOfName)
				{
					subset.diffuse.texture_filename.resize(lengthOfName);
					fin.read((char*)(subset.diffuse.texture_filename.data()), sizeof(wchar_t) * lengthOfName);

					// TODO : ワイド文字列の引数あり出力ができていない。

					std::wstring output = L"[Info]: [FBX LOADER] Complete!! Loaded TEXTURE( Name : " + subset.diffuse.texture_filename + L" )";
					std::wcout << output << std::endl;
					OutputDebugString(output.c_str());
					//Log::InfoW(L"[FBX LOADER] Complete Loading!! TEXTURE( Name : %ws )", subset.diffuse.texture_filename);

					subset.diffuse.texture->Load(device, subset.diffuse.texture_filename.c_str());
				}
				else
				{
					subset.diffuse.texture->Load(device);

				}
				// fetch the color information
				fin.read((char*)&(subset.diffuse.color), sizeof(DirectX::XMFLOAT4));
			}

		}

		// fetch the number of bone data
		unsigned int numberOfBoneData = 0;
		fin.read((char*)&numberOfBoneData, sizeof(unsigned int));

		if (numberOfBoneData)
		{
			mesh.m_bone_data.resize(numberOfBoneData);

			for (auto& bonedata : mesh.m_bone_data)
			{
				fin.read((char*)&bonedata.transform, sizeof(DirectX::XMFLOAT4X4));

				unsigned int lengthOfName = 0;
				fin.read((char*)&lengthOfName, sizeof(unsigned int));

				if (lengthOfName)
				{
					bonedata.name.resize(lengthOfName);
					fin.read((char*)bonedata.name.data(), sizeof(char) * lengthOfName);
				}
				fin.read((char*)&bonedata.offset, sizeof(DirectX::XMFLOAT4X4));
			}
		}

		// fetch only "default" motions
		unsigned int lengthOfMotionName = 0;
		
		fin.read((char*)&lengthOfMotionName, sizeof(unsigned int));

		std::string motionName;
		if (lengthOfMotionName)
		{
			motionName.resize(lengthOfMotionName);
			fin.read((char*)motionName.data(), sizeof(char) * lengthOfMotionName);

			FbxInfo::Motion motion;

			fin.read((char*)&motion.number_of_frames, sizeof(int));

			fin.read((char*)&motion.sampling_time, sizeof(float));

			unsigned int numberOfKeys = 0;

			fin.read((char*)&numberOfKeys, sizeof(unsigned int));

			if (numberOfKeys)
			{
				motion.keys.resize(numberOfKeys);
				for (auto& key : motion.keys)
				{
					unsigned int numberOfKeyFrames = 0;

					fin.read((char*)&numberOfKeyFrames, sizeof(unsigned int));

					if (numberOfKeyFrames)
					{
						key.resize(numberOfKeyFrames);
						for (auto& frame : key)
						{
							fin.read((char*)&frame, sizeof(DirectX::XMFLOAT4X4));
						}

					}
				}

				mesh.m_motions.emplace(motionName, motion);
			}
		}
	}
	fin.close();

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool FbxLoader::LoadMMM(std::string& name, const std::string& filename, std::vector<MyFbxMesh>& meshes)
{
	auto fin = std::ifstream();
	auto flag = ios::in | ios::binary;

	// start to write file
	fin.open(filename, flag);
	if (!fin)
	{
		Log::Error("[FBX LOADER] Refered File doesn't exist. (%s)", filename);
		return false;
	}

	for (auto& mesh : meshes)
	{
		FbxInfo::Motion motion = {};

		fin.read((char*)&motion.number_of_frames, sizeof(int));

		fin.read((char*)&motion.sampling_time, sizeof(float));

		unsigned int numberOfKeys = 0;
		fin.read((char*)&numberOfKeys, sizeof(unsigned int));

		if (numberOfKeys)
		{
			motion.keys.resize(numberOfKeys);
			for (auto& key : motion.keys)
			{
				unsigned int numberOfKeyFrames = 0;
				fin.read((char*)&numberOfKeyFrames, sizeof(unsigned int));

				if (numberOfKeyFrames)
				{
					key.resize(numberOfKeyFrames);
					for (auto& frame : key)
					{
						fin.read((char*)&frame, sizeof(DirectX::XMFLOAT4X4));
					}
				}
			}
		}

		mesh.m_motions.emplace(name, motion);
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::FetchBoneInfluences(const FbxMesh* fbx_mesh, std::vector<BoneInfluencesPerControlPoint>& influences)
{
	const int number_of_control_points = fbx_mesh->GetControlPointsCount();
	influences.resize(number_of_control_points);

	const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer)
	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));

		const int number_of_clusters = skin->GetClusterCount();
		for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster)
		{
			FbxCluster* cluster = skin->GetCluster(index_of_cluster);

			const int number_of_control_point_indices = cluster->GetControlPointIndicesCount();
			const int* array_of_control_point_indices = cluster->GetControlPointIndices();
			const double* array_of_control_point_weights = cluster->GetControlPointWeights();


			for (int i = 0; i < number_of_control_point_indices; ++i)
			{
				BoneInfluencesPerControlPoint& influences_per_control_point = influences.at(array_of_control_point_indices[i]);
				BoneInfluence influence;
				influence.index = index_of_cluster;
				influence.weight = static_cast<float>(array_of_control_point_weights[i]);
				influences_per_control_point.push_back(influence);

			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

//void FbxLoader::fetchBoneMatrices(const FbxMesh* fbx_mesh, std::vector<FbxInfo::Bone>& skeltal, FbxTime time)
//{
//	const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
//	for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer)
//	{
//		FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));
//
//		const int number_of_clusters = skin->GetClusterCount();
//		skeltal.resize(number_of_clusters);
//
//		static int count = 0;
//
//		for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster)
//		{
//			FbxInfo::Bone& bone = skeltal.at(index_of_cluster);
//
//			FbxCluster* cluster = skin->GetCluster(index_of_cluster);
//
//			bone.name = cluster->GetLink()->GetName();
//#ifdef _DEBUG
//			Log::Info("[FBX LOADER] Fetched Bone.");
//#endif
//			//this matrix transforms coordinates of the initial pose from mesh space to global space
//			FbxAMatrix reference_global_init_position;
//			cluster->GetTransformMatrix(reference_global_init_position);
//
//			//this matrix transforms coordinates of the initialpose from bone space to global space
//			FbxAMatrix cluster_global_init_position;
//			cluster->GetTransformLinkMatrix(cluster_global_init_position);
//
//			//this matrix transforms coordinates of the current pose from bone space to global space
//			FbxAMatrix cluster_global_current_position;
//			cluster_global_current_position = cluster->GetLink()->EvaluateGlobalTransform(time);
//
//			//this matrix transforms coordinates of the current pose from mesh space to global space
//			FbxAMatrix reference_global_current_position;
//			reference_global_current_position = fbx_mesh->GetNode()->EvaluateGlobalTransform(time);
//
//			//this matrix transforms coordinates of the Column Major scheme. When a FbxMatrix represents a transformation
//			//(translation, rotation and scale), the last row of the matrix represents the translation part of the
//			//transformation.
//			FbxAMatrix transform =
//				reference_global_current_position.Inverse() *
//				cluster_global_current_position *
//				cluster_global_init_position.Inverse() *
//				reference_global_init_position;
//
//			for (auto i = 0; i < 4; ++i)
//			{
//				for (auto j = 0; j < 4; ++j)
//				{
//					bone.transform.m[i][j] = static_cast<float>(transform.Get(i, j));
//				}
//			}
//		}
//		count++;
//	}
//
//}

//--------------------------------------------------------------------------------------------------------------------------------

//void FbxLoader::fetchAnimations(const FbxMesh* fbx_mesh, FbxInfo::SkeltalAnimation& skeltal_animation, u_int sampling_rate)
//{
//	// Get the list of all the animation stack.
//	FbxArray<FbxString*> array_of_animation_stack_names;
//	fbx_mesh->GetScene()->FillAnimStackNameArray(array_of_animation_stack_names);
//
//	// Get the number of animations.
//	int number_of_animations = array_of_animation_stack_names.Size();
//
//	if (number_of_animations > 0)
//	{
//		// Get the FbxTimer per animation's frame.
//		FbxTime::EMode time_mode = fbx_mesh->GetScene()->GetGlobalSettings().GetTimeMode();
//		FbxTime frame_time;
//		frame_time.SetTime(0, 0, 0, 1, 0, time_mode);
//
//		sampling_rate = sampling_rate > 0 ? sampling_rate : static_cast<u_int>(frame_time.GetFrameRate(time_mode));
//		float sampling_time = 1.0f / sampling_rate;
//		skeltal_animation.sampling_time = sampling_time;
//		skeltal_animation.animation_tick = 0.0f;
//
//		FbxString* animation_stack_name = array_of_animation_stack_names.GetAt(0);
//		FbxAnimStack* current_animation_stack = fbx_mesh->GetScene()->FindMember<FbxAnimStack>(animation_stack_name->Buffer());
//		fbx_mesh->GetScene()->SetCurrentAnimationStack(current_animation_stack);
//
//		FbxTakeInfo* take_info = fbx_mesh->GetScene()->GetTakeInfo(animation_stack_name->Buffer());
//		FbxLongLong start_time = take_info->mLocalTimeSpan.GetStart().Get();
//		FbxLongLong end_time = take_info->mLocalTimeSpan.GetStop().Get();
//		FbxLongLong fps60 = FbxTime::GetOneFrameValue(FbxTime::eFrames60);
//
//		skeltal_animation.number_of_frame = static_cast<int>((end_time - start_time) / fps60);
//
//		FbxTime sampling_step;
//		sampling_step.SetTime(0, 0, 1, 0, 0, time_mode);
//		sampling_step = static_cast<FbxLongLong>(sampling_step.Get() * sampling_time);
//
//		for (FbxTime current_time = start_time; current_time < end_time; current_time += sampling_step)
//		{
//			FbxInfo::Skeltal skeltal;
//			fetchBoneMatrices(fbx_mesh, skeltal, current_time);
//			skeltal_animation.push_back(skeltal);
//		}
//	}
//	for (int i = 0; i < number_of_animations; i++)
//	{
//		delete array_of_animation_stack_names[i];
//	}
//
//}

//--------------------------------------------------------------------------------------------------------------------------------

FbxLoader::~FbxLoader()
{
}


//--------------------------------------------------------------------------------------------------------------------------------
