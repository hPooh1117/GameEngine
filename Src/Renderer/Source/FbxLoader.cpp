#include "FbxLoader.h"

#include <functional>
#include <iostream>
#include <fstream>
#include <cassert>

#include "FbxMeshInfo.h"
#include "ResourceManager.h"

#include "./Utilities/Log.h"
#include "./Utilities/PerfTimer.h"

//--------------------------------------------------------------------------------------------------------------------------------

using namespace fbxsdk;

//--------------------------------------------------------------------------------------------------------------------------------

//std::unordered_map < std::string, std::vector<MyFbxMesh>> FbxLoader::mModelTable;
const unsigned int ERROR_NUMBER = 100;

//--------------------------------------------------------------------------------------------------------------------------------

FbxLoader::FbxLoader()
	:m_pTimer(std::make_unique<PerfTimer>())
{
}

//--------------------------------------------------------------------------------------------------------------------------------

bool FbxLoader::Load(D3D::DevicePtr& device, const char* filename, std::vector<MyFbxMesh>& mesh_container)
{
	// シリアル化されたデータが存在すれば読み込み
	std::string filename2(filename);
	unsigned int length = filename2.size();
	filename2.at(length - 1) = 'A';
	filename2.at(length - 2) = 'A';
	filename2.at(length - 3) = 'A';
	if (LoadSerializedMesh(device, filename2, mesh_container)) return true;

	// シリアル化されたデータがなければ.fbxを読み込み
	LoadFbxFile(device, filename, mesh_container);

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool FbxLoader::LoadFbxFile(
	Microsoft::WRL::ComPtr<ID3D11Device>& device,
	const char* fbxfilename,
	std::vector<MyFbxMesh>& mesh_container)
{
	// FBX SDK Managerを生成
	Log::Info("[FBX] Start up FbxManager.");
	FbxManager* manager = FbxManager::Create();
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));
	FbxImporter* importer = FbxImporter::Create(manager, "");
	bool bIsImported = false;
	bIsImported = importer->Initialize(fbxfilename, -1, manager->GetIOSettings());
	if (!bIsImported)
	{
		assert(importer->GetStatus().GetErrorString());
		return false;
	}


	// シーンデータをインポート
	FbxScene* scene = FbxScene::Create(manager, "");
	bIsImported = importer->Import(scene);
	if (!bIsImported)
	{
		assert(importer->GetStatus().GetErrorString());
		return false;
	}
	Log::Info("[FBX] Created FbxScene");


	// ポリゴンを三角形に
	TrangulateGeometries(manager, scene);

	std::vector<FbxNode*> fetchedMeshes;
	// メッシュデータを含むノードを検索	
	SearchMeshNodeRecursively(fetchedMeshes, scene);


	// メッシュ情報を取り出す
	Log::Info("[FBX] ***** Start Fetching  Meshes (Size : %d) *****", fetchedMeshes.size());
	m_pTimer->Reset();
	m_pTimer->Start();
	mesh_container.resize(fetchedMeshes.size());
	for (size_t i = 0; i < fetchedMeshes.size(); ++i)
	{
		Log::Info("[FBX] Loading MESH info ( %d / %d )", i, fetchedMeshes.size());

		FbxMesh* fbxMesh = fetchedMeshes.at(i)->GetMesh();
		MyFbxMesh& mesh = mesh_container.at(i);


		LoadBones(fbxMesh, mesh);

		// マテリアル取り出し(CGFX)
		FbxLayerElementMaterial* LEM = fbxMesh->GetElementMaterial();
		if (LEM != nullptr)
		{
			int materialIndex = LEM->GetIndexArray().GetAt(0);
			FbxSurfaceMaterial* material = fbxMesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(materialIndex);
			const FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

			Log::Info("[FBX] Loading CGFX file.");
			LoadCGFX(device, mesh, material, fbxfilename);
		}

		// マテリアル取り出し
		mNumberOfMaterials = fbxMesh->GetNode()->GetMaterialCount();
		mesh.mSubsets.resize(mNumberOfMaterials > 0 ? mNumberOfMaterials : 1);
		for (auto index = 0u; index < mNumberOfMaterials; ++index)
		{
			Log::Info("[FBX] Loading MATERIAL info ( %d / %d )", index, mNumberOfMaterials);
			const FbxSurfaceMaterial* surfaceMaterial = fbxMesh->GetNode()->GetMaterial(index);

			// Diffuseのプロパティを探す
			const fbxsdk::FbxProperty property = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

			// Diffuseの重み (Factor) を探す
			const FbxProperty factor = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);


			FbxInfo::Subset& subset = mesh.mSubsets.at(index);


			if (property.IsValid() && factor.IsValid())
			{
				Log::Info("[FBX] FbxSurfaceMaterial::sDiffuse & FbxSurfaceMaterial::sDiffuseFactor is Found.");
				FbxDouble3 color = property.Get<FbxDouble3>();
				double f = factor.Get<FbxDouble>();
				subset.diffuse.color.x = static_cast<float>(color[0] * f);
				subset.diffuse.color.y = static_cast<float>(color[1] * f);
				subset.diffuse.color.z = static_cast<float>(color[2] * f);
				subset.diffuse.color.w = 1.0f;
			}
			if (property.IsValid())
			{
				Log::Info("[FBX] FbxSurfaceMaterial::sDiffuse is Found.");
				Log::Info("[FBX] FbxSurfaceMaterial::sDiffuseFactor doesn't exist.");

				const int numberOfTextures = property.GetSrcObjectCount<FbxFileTexture>();
				Log::Info("[FBX] Found Texture infomation. (size : %d)", numberOfTextures);
				if (numberOfTextures)
				{
					const FbxFileTexture* fileTexture = property.GetSrcObject<FbxFileTexture>();
					if (fileTexture)
					{
						const char* texture_filename = fileTexture->GetRelativeFileName();
						
						subset.diffuse.textures[FbxInfo::Material::EColor] = std::make_shared<Texture>();

						if (texture_filename)
						{
							wchar_t fbxUnicode[256];
							MultiByteToWideChar(CP_ACP, 0, fbxfilename, strlen(fbxfilename) + 1, fbxUnicode, 1024);
							wchar_t textureUnicode[256];
							MultiByteToWideChar(CP_ACP, 0, fileTexture->GetFileName(), strlen(fileTexture->GetFileName()) + 1, textureUnicode, 1024);
							ResourceManager::CreateFilenameToRefer(textureUnicode, fbxUnicode, textureUnicode);

							subset.diffuse.texFileTable[FbxInfo::Material::EColor] = textureUnicode;
							subset.diffuse.textures[FbxInfo::Material::EColor]->Load(device, subset.diffuse.texFileTable[FbxInfo::Material::EColor].c_str());
						}
						else
						{
							Log::Info("[FBX] Doesn't exist Texture File Name. Start creating DUMMY.");
							subset.diffuse.textures[FbxInfo::Material::EColor]->Load(device);
						}
					}
				}
			}
		}



		for (auto& subset : mesh.mSubsets)
		{
			if (!subset.diffuse.textures[FbxInfo::Material::EColor])
			{
				Log::Info("[FBX] Couldn't create Texture. Start creating DUMMY.");
				subset.diffuse.textures[FbxInfo::Material::EColor] = std::make_shared<Texture>();
				subset.diffuse.textures[FbxInfo::Material::EColor]->Load(device);
			}
		}



		if (mNumberOfMaterials > 0)
		{
			// 各マテリアルのポリゴン数取得
			const int numberOfPolygons = fbxMesh->GetPolygonCount();
			for (int index = 0; index < numberOfPolygons; ++index)
			{
				const u_int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(index);
				mesh.mSubsets.at(materialIndex).indexCount += 3;
			}

			// オフセットを記録
			int offset = 0;
			for (FbxInfo::Subset& subset : mesh.mSubsets)
			{
				subset.indexStart = offset;
				offset += subset.indexCount;

				// 後の頂点情報の取り出しの時に使う値のため0クリア。
				subset.indexCount = 0;
			}
		}

		// メッシュトランスフォーム情報
		FbxMatrix global_transform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
		for (auto row = 0; row < 4; row++)
		{
			for (auto column = 0; column < 4; column++)
			{
				mesh.mGlobalTransform.m[row][column] = static_cast<float>(global_transform[row][column]);
			}
		}


		// メッシュデータ取得
		std::vector<FbxInfo::Vertex> vertices;	
		std::vector<u_int> indices;		
		u_int vertex_count = 0;



		//　ボーンの影響度を取得
		std::vector<BoneInfluencesPerControlPoint> boneInfluences;
		FetchBoneInfluences(fbxMesh, boneInfluences);


		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);

		// コントロールポイントを利用してポリゴン、頂点情報を取得する。
		const FbxVector4* arrayOfControlPoints = fbxMesh->GetControlPoints();
		const int numberOfPolygons = fbxMesh->GetPolygonCount();

		indices.resize(numberOfPolygons * 3);

		fbxMesh->GenerateTangentsData(0, true);

		for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; indexOfPolygon++)
		{

			// the material for current face
			int indexOfMaterial = 0;
			if (mNumberOfMaterials > 0)
			{
				indexOfMaterial = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);
			}

			// the place I should hold the vertex attribute index
			FbxInfo::Subset& subset = mesh.mSubsets.at(indexOfMaterial);
			const int indexOffset = subset.indexStart + subset.indexCount;

			const int numberOfVertices = fbxMesh->GetPolygonSize(indexOfPolygon);

			for (int indexOfVertex = 0; indexOfVertex < numberOfVertices; indexOfVertex++)
			{
				FbxInfo::Vertex vertex = {};
				const int indexOfControlPoint = fbxMesh->GetPolygonVertex(indexOfPolygon, indexOfVertex);
				vertex.position.x = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][0]);
				vertex.position.y = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][1]);
				vertex.position.z = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][2]);
				//LoadPosition(fbxMesh, vertices);

				if (fbxMesh->GetElementNormalCount())
				{
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(indexOfPolygon, indexOfVertex, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
					//LoadNormal(fbxMesh, vertices);
				}

				if (fbxMesh->GetElementUVCount())
				{
					FbxVector2 uv;
					bool unmapped_uv;
					fbxMesh->GetPolygonVertexUV(indexOfPolygon, indexOfVertex, uvNames[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}
				if (fbxMesh->GetElementTangentCount())
				{
					int currentIndex = indexOfPolygon * 3 + indexOfVertex;
						
					FbxGeometryElementTangent* tangents = fbxMesh->GetElementTangent(0);
					if (tangents != nullptr)
					{
						FbxVector4 t = tangents->GetDirectArray().GetAt(currentIndex);
						vertex.tangent.x = static_cast<float>(t[0]);
						vertex.tangent.y = static_cast<float>(t[1]);
						vertex.tangent.z = static_cast<float>(t[2]);
					}
				}
				if (fbxMesh->GetElementBinormalCount())
				{
					int currentIndex = indexOfPolygon * 3 + indexOfVertex;

					FbxGeometryElementBinormal* binormals = fbxMesh->GetElementBinormal(0);
					if (binormals != nullptr)
					{
						FbxVector4 b = binormals->GetDirectArray().GetAt(currentIndex);
						vertex.binormal.x = static_cast<float>(b[0]);
						vertex.binormal.y = static_cast<float>(b[1]);
						vertex.binormal.z = static_cast<float>(b[2]);
					}
				}

				vertices.push_back(vertex);
				indices.push_back(vertex_count);
				indices.at(indexOffset + indexOfVertex) = static_cast<u_int>(vertex_count);
				vertex_count += 1;



				int vertexid = vertices.size() - 1;
				if (indexOfControlPoint > 0)
				{
					auto vector_size = boneInfluences.at(indexOfControlPoint).size();
					for (auto i = 0u; i < vector_size; ++i)
					{
						vertices[vertexid].bone_indices[i] = boneInfluences.at(indexOfControlPoint).at(i).index;
						vertices[vertexid].bone_weights[i] = boneInfluences.at(indexOfControlPoint).at(i).weight;

					}
				}

			}
			subset.indexCount += 3;
		}

		mesh.mVertices = vertices;
		mesh.mIndices = indices;
		mNumberOfVertices += vertices.size();
	}
	m_pTimer->Stop();
	Log::Info("[FBX] ***** Finish Fetching  Meshes in %.2f s *****", m_pTimer->GetDeltaTime());


	// シリアル化してセーブ
	std::string file(fbxfilename);
	UINT l = file.size();
	file.at(l - 1) = 'A';
	file.at(l - 2) = 'A';
	file.at(l - 3) = 'A';
	SerializeAndSaveMeshes(file, mesh_container);

	manager->Destroy();
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::TrangulateGeometries(FbxManager* manager, FbxScene* scene)
{
	fbxsdk::FbxGeometryConverter geometryConverter(manager);
	geometryConverter.Triangulate(scene, true);
	Log::Info("[FBX] Finish Triangulated.");
}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::SearchMeshNodeRecursively(std::vector<FbxNode*>& nodes, FbxScene* scene)
{
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node)
	{
		if (node)
		{
			FbxNodeAttribute* fbxNodeAttribute = node->GetNodeAttribute();
			if (fbxNodeAttribute)
			{
				switch (fbxNodeAttribute->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					nodes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
			{
				traverse(node->GetChild(i));
			}
		}
	};
	Log::Info("[FBX] ***** Start searching the node of Meshes *****", m_pTimer->GetDeltaTime());
	m_pTimer->Start();
	traverse(scene->GetRootNode());
	m_pTimer->Stop();
	Log::Info("[FBX] ***** Found the node of Meshes in %.2f   *****\n", m_pTimer->GetDeltaTime());

}

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
		my_mesh.mMotions["default"].frameSize = static_cast<unsigned int>((end_time - start_time) / fps60);

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
	FbxInfo::Motion& M = my_mesh.mMotions[name];
	
	if (M.frameSize <= 0)
	{
		return;
	}

	M.keys.emplace_back();
	M.keys.at(bone).resize(M.frameSize);

	double time = my_mesh.m_start_frame * (1.0 / 60.0);

	FbxTime fbxTime;
	for (auto frame = 0u; frame < M.frameSize; ++frame)
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
					mesh.mSubsets.emplace_back();
					FbxInfo::Subset& subset = mesh.mSubsets.back();
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
						MultiByteToWideChar(CP_ACP, 0, filename.c_str(), strlen(filename.c_str()) + 1, fbx_unicode, 2048);
						wchar_t texture_unicode[256];
						MultiByteToWideChar(CP_ACP, 0, tex->GetFileName(), strlen(tex->GetFileName()) + 1, texture_unicode, 2048);
						ResourceManager::CreateFilenameToRefer(texture_unicode, fbx_unicode, texture_unicode);

						subset.diffuse.texFileTable[FbxInfo::Material::EColor] = texture_unicode;

						subset.diffuse.textures[FbxInfo::Material::EColor] = std::make_shared<Texture>();
						subset.diffuse.textures[FbxInfo::Material::EColor]->Load(device, subset.diffuse.texFileTable[FbxInfo::Material::EColor].c_str());
					}

				}
			}
			//if (src == FbxSurfaceMaterial::sDiffuseFactor)
			//{
			//	for (auto& subset : mesh.mSubsets)
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
	Log::Info("[FBX][MOTION] Start Fetching additional motion.");
	std::string filename2(filename);
	unsigned int length = filename2.size();
	filename2.at(length - 1) = 'M';
	filename2.at(length - 2) = 'M';
	filename2.at(length - 3) = 'M';
	if (LoadSerializedMotion(name, filename2, meshes)) return true;

	Log::Info("[FBX][MOTION] Start Up FbxManager.");
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

	Log::Info("[FBX][MOTION] ***** Start searching the node of Meshes *****", m_pTimer->GetDeltaTime());
	m_pTimer->Start();
	traverse(scene->GetRootNode());
	m_pTimer->Stop();
	Log::Info("[FBX][MOTION] ***** Found the node of Meshes in %.2f   *****\n", m_pTimer->GetDeltaTime());

	//FbxMesh *fbxMesh = fetchedMeshes.at(0)->GetMesh();  // Currently only one mesh.
	//meshes.resize(fetchedMeshes.size());
	Log::Info("[FBX] ***** Start Fetching  Meshes (Size : %d) *****", fetched_meshes.size());
	m_pTimer->Reset();
	m_pTimer->Start();
	for (size_t i = 0; i < fetched_meshes.size(); ++i)
	{
		FbxMesh* fbx_mesh = fetched_meshes.at(i)->GetMesh();
		MyFbxMesh& mesh = meshes.at(i);
		mesh.m_start_frame = startFrame;
		mesh.mMotions[name].frameSize = numFrame;

		FbxNode* root = scene->GetRootNode();

		for (auto b = 0u; b < mesh.m_bone_data.size(); ++b)
		{
			FbxNode* bone = root->FindChild(mesh.m_bone_data.at(b).name.c_str());

			if (bone == NULL) continue;

			LoadKeyFrames(name, b, bone, fbx_mesh, mesh);
		}
	}
	m_pTimer->Stop();
	Log::Info("[FBX] ***** Finish Fetching  Meshes in %.2f s *****", m_pTimer->GetDeltaTime());

	scene->Destroy();
	manager->Destroy();
	for (int i = 0; i < number_of_animations; i++)
	{
		delete names[i];
	}
	SerializeAndSaveMotion(filename2, name, meshes);

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void FbxLoader::SerializeAndSaveMeshes(const std::string filename, std::vector<MyFbxMesh>& mesh_container)
{
	auto fout = std::ofstream();
	auto flag = std::ios::out | std::ios::binary | std::ios::trunc;

	// start to write file
	fout.open(filename, flag);
	if (!fout)
	{
		Log::Error("[FBX LOADER] Could not Save Mesh.");
		return;
	}
	Log::Info("[FBX LOADER] Start Saving Mesh");
	Log::Info("[FBX LOADER] Name : %s", filename);

	// SerializeAndSaveMeshes the number of meshes
	unsigned int numberOfMeshes = mesh_container.size();

	fout.write((char*)&numberOfMeshes, sizeof(unsigned int));
	// SerializeAndSaveMeshes the meshes
	for (auto& mesh : mesh_container)
	{
		// SerializeAndSaveMeshes the number of vertices
		unsigned int numberOfVertices = mesh.mVertices.size();

		fout.write((char*)&numberOfVertices, sizeof(unsigned int));

		// SerializeAndSaveMeshes the vertices
		for (auto& vertex : mesh.mVertices)
		{
			fout.write((char*)&vertex, sizeof(FbxInfo::Vertex));
		}

		Log::Info("[FBX LOADER] Save Complete!! VERTICES (Size : %d)", numberOfVertices);

		// SerializeAndSaveMeshes the number of indices
		unsigned int numberOfIndices = mesh.mIndices.size();

		fout.write((char*)&numberOfIndices, sizeof(unsigned int));

		// SerializeAndSaveMeshes the indices
		for (auto& index : mesh.mIndices)
		{
			fout.write((char*)&index, sizeof(u_int));
		}

		Log::Info("[FBX LOADER] Save Complete!! INDICES (Size : %d)", numberOfIndices);

		// SerializeAndSaveMeshes the global transform
		fout.write((char*)&mesh.mGlobalTransform, sizeof(DirectX::XMFLOAT4X4));

		Log::Info("[FBX LOADER] Save Complete!! GLOBAL_TRANSFORM_MATRIX");

		// SerializeAndSaveMeshes the number of subsets
		unsigned int numberOfSubsets = mesh.mSubsets.size();
		fout.write((char*)&numberOfSubsets, sizeof(unsigned int));

		if (numberOfSubsets)
		{
			// SerializeAndSaveMeshes the Subset::subset
			for (auto& subset : mesh.mSubsets)
			{
				fout.write((char*)&subset.indexStart, sizeof(u_int));
				fout.write((char*)&subset.indexCount, sizeof(u_int));

				// SerializeAndSaveMeshes the length of texture filename 
				unsigned int lengthOfName = 0;
				if (subset.diffuse.texFileTable[FbxInfo::Material::EColor].size())
				{
					lengthOfName = subset.diffuse.texFileTable[FbxInfo::Material::EColor].size();
				}
				fout.write((char*)&lengthOfName, sizeof(unsigned int));

				// SerializeAndSaveMeshes the texture file name
				if (lengthOfName)
				{
					fout.write((char*)(subset.diffuse.texFileTable[FbxInfo::Material::EColor].data()), (sizeof(wchar_t)) * lengthOfName);

					Log::Info("[FBX LOADER] Save Complete!! TEXTURE (Name : %s)", subset.diffuse.texFileTable[FbxInfo::Material::EColor]);
				}

				// SerializeAndSaveMeshes the color information
				fout.write((char*)&subset.diffuse.color, sizeof(DirectX::XMFLOAT4));
			}


		}
		// SerializeAndSaveMeshes the number of bone data
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
		auto it = mesh.mMotions.find("default");

		if (it != mesh.mMotions.end())
		{
			unsigned int lengthOfMotionName = it->first.size();
			fout.write((char*)&lengthOfMotionName, sizeof(unsigned int));

			fout.write((char*)it->first.data(), sizeof(char)* lengthOfMotionName);

			fout.write((char*)&it->second.frameSize, sizeof(unsigned int));

			//fout.write((char*)&it->second.SAMPLE_TIME, sizeof(float));

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

void FbxLoader::SerializeAndSaveMotion(const std::string& filename, const std::string& key, std::vector<MyFbxMesh>& meshes)
{
	auto fout = std::ofstream();
	auto flag = std::ios::out | std::ios::binary | std::ios::trunc;

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
		auto it = mesh.mMotions.find(key);

		if (it != mesh.mMotions.end())
		{
			FbxInfo::Motion& motion = it->second;
			
			fout.write((char*)&motion.frameSize, sizeof(unsigned int));

			//fout.write((char*)&motion.SAMPLE_TIME, sizeof(float));

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

bool FbxLoader::LoadSerializedMesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::string filename, std::vector<MyFbxMesh>& mesh_container)
{
	auto fin = std::ifstream();
	auto flag = std::ios::in | std::ios::binary;

	// start to write file
	fin.open(filename, flag);
	if (!fin)
	{
		Log::Info("[FBX][AAA] Refered File doesn't exist. (%s)\n", filename);
		return false;
	}

	// fetch the number of meshes
	unsigned int numberOfMeshes;
	fin.read((char*)&numberOfMeshes, sizeof(unsigned int));

	Log::Info("[FBX][AAA] Start Loading MESHES (Size : %d)", numberOfMeshes);
	mesh_container.resize(numberOfMeshes);

	PerfTimer timer;
	timer.Start();
	// fetch the meshes
	unsigned int count = 0;
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
		fin.read((char*)&(mesh.mGlobalTransform), sizeof(DirectX::XMFLOAT4X4));

		// fetch the number of subsets
		unsigned int numberOfSubsets;
		fin.read((char*)&numberOfSubsets, sizeof(unsigned int));


		if (numberOfSubsets)
		{
			mesh.mSubsets.resize(numberOfSubsets);

			// fetch the Subset::subset
			for (auto& subset : mesh.mSubsets)
			{
				fin.read((char*)&subset.indexStart, sizeof(u_int));
				fin.read((char*)&subset.indexCount, sizeof(u_int));
				// fetch the length of texture filename
				unsigned int lengthOfName;
				fin.read((char*)&lengthOfName, sizeof(unsigned int));

				subset.diffuse.textures[FbxInfo::Material::EColor] = std::make_shared<Texture>();
				// fetch the texture file name
				if (lengthOfName)
				{
					subset.diffuse.texFileTable[FbxInfo::Material::EColor].resize(lengthOfName);
					fin.read((char*)(subset.diffuse.texFileTable[FbxInfo::Material::EColor].data()), sizeof(wchar_t) * lengthOfName);

					// TODO : ワイド文字列の引数あり出力ができていない。

					std::wstring output = L"[INFO]: [FBX][AAA] Complete!! Loaded TEXTURE( Name : " + subset.diffuse.texFileTable[FbxInfo::Material::EColor] + L" )";
					std::wcout << output << std::endl;
					OutputDebugString(output.c_str());
					//Log::InfoW(L"[FBX LOADER] Complete Loading!! TEXTURE( Name : %ws )", subset.diffuse.texture_filename);

					subset.diffuse.textures[FbxInfo::Material::EColor]->Load(device, subset.diffuse.texFileTable[FbxInfo::Material::EColor].c_str());
				}
				else
				{
					subset.diffuse.textures[FbxInfo::Material::EColor]->Load(device);

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

			fin.read((char*)&motion.frameSize, sizeof(unsigned int));

			//fin.read((char*)&motion.SAMPLE_TIME, sizeof(float));

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

				mesh.mMotions.emplace(motionName, motion);
			}
		}
		Log::Info("[FBX][AAA] Loaded MESH ( %d / %d )",++count, numberOfMeshes);
	}
	timer.Stop();
	Log::Info("[FBX][AAA] Complete loading Meshes in %.2f s", timer.GetDeltaTime());

	fin.close();

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool FbxLoader::LoadSerializedMotion(std::string& name, const std::string& filename, std::vector<MyFbxMesh>& meshes)
{
	auto fin = std::ifstream();
	auto flag = std::ios::in | std::ios::binary;

	// start to write file
	fin.open(filename, flag);
	if (!fin)
	{
		Log::Error("[FBX][MMM] Refered File doesn't exist. (%s)", filename);
		return false;
	}

	m_pTimer->Reset();
	m_pTimer->Start();
	unsigned int count = 0;
	for (auto& mesh : meshes)
	{
		FbxInfo::Motion motion = {};

		fin.read((char*)&motion.frameSize, sizeof(unsigned int));

		//fin.read((char*)&motion.SAMPLE_TIME, sizeof(float));

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

		mesh.mMotions.emplace(name, motion);
		Log::Info("[FBX][MMM] Loaded MOTION ( %d / %d )", ++count, meshes.size());
	}
	m_pTimer->Stop();
	Log::Info("[FBX][MMM] Loaded MMM File in %.2f s. (%s)", m_pTimer->GetDeltaTime(), filename);

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

//void FbxLoader::fetchBoneMatrices(const FbxMesh* fbxMesh, std::vector<FbxInfo::Bone>& skeltal, FbxTime time)
//{
//	const int number_of_deformers = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
//	for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer)
//	{
//		FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));
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
//			reference_global_current_position = fbxMesh->GetNode()->EvaluateGlobalTransform(time);
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

//void FbxLoader::fetchAnimations(const FbxMesh* fbxMesh, FbxInfo::SkeltalAnimation& skeltal_animation, u_int sampling_rate)
//{
//	// Get the list of all the animation stack.
//	FbxArray<FbxString*> array_of_animation_stack_names;
//	fbxMesh->GetScene()->FillAnimStackNameArray(array_of_animation_stack_names);
//
//	// Get the number of animations.
//	int number_of_animations = array_of_animation_stack_names.Size();
//
//	if (number_of_animations > 0)
//	{
//		// Get the FbxTimer per animation's frame.
//		FbxTime::EMode time_mode = fbxMesh->GetScene()->GetGlobalSettings().GetTimeMode();
//		FbxTime frame_time;
//		frame_time.SetTime(0, 0, 0, 1, 0, time_mode);
//
//		sampling_rate = sampling_rate > 0 ? sampling_rate : static_cast<u_int>(frame_time.GetFrameRate(time_mode));
//		float SAMPLE_TIME = 1.0f / sampling_rate;
//		skeltal_animation.SAMPLE_TIME = SAMPLE_TIME;
//		skeltal_animation.animeTick = 0.0f;
//
//		FbxString* animation_stack_name = array_of_animation_stack_names.GetAt(0);
//		FbxAnimStack* current_animation_stack = fbxMesh->GetScene()->FindMember<FbxAnimStack>(animation_stack_name->Buffer());
//		fbxMesh->GetScene()->SetCurrentAnimationStack(current_animation_stack);
//
//		FbxTakeInfo* take_info = fbxMesh->GetScene()->GetTakeInfo(animation_stack_name->Buffer());
//		FbxLongLong start_time = take_info->mLocalTimeSpan.GetStart().Get();
//		FbxLongLong end_time = take_info->mLocalTimeSpan.GetStop().Get();
//		FbxLongLong fps60 = FbxTime::GetOneFrameValue(FbxTime::eFrames60);
//
//		skeltal_animation.number_of_frame = static_cast<int>((end_time - start_time) / fps60);
//
//		FbxTime sampling_step;
//		sampling_step.SetTime(0, 0, 1, 0, 0, time_mode);
//		sampling_step = static_cast<FbxLongLong>(sampling_step.Get() * SAMPLE_TIME);
//
//		for (FbxTime current_time = start_time; current_time < end_time; current_time += sampling_step)
//		{
//			FbxInfo::Skeltal skeltal;
//			fetchBoneMatrices(fbxMesh, skeltal, current_time);
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

//bool NewFbxLoader::LoadAndGatherFbxMesh(
//	D3D::DevicePtr& p_device,
//	const char* fbxfilename,
//	std::vector<MyFbxMesh>& mesh_container)
//{
//
//
//	return true;
//}
