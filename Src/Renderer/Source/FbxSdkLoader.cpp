#include "FbxSdkLoader.h"
#include "./Utilities/Util.h"
#include "./Utilities/Log.h"

bool FbxSdkLoader::Load(const wchar_t* file_path)
{
	std::string filename;
	StringOp::StringConvert(file_path, filename);

	Log::Info("[FBXSDK_LOADER] Start loading...(%s)", filename.c_str());
	// FBX SDKの初期化
	FbxManager* manager = FbxManager::Create();
	FbxScene* scene = FbxScene::Create(manager, "");
	//	ファイルからシーンに読み込み
	FbxImporter* importer = FbxImporter::Create(manager, "");
	importer->Initialize(filename.c_str());
	importer->Import(scene);
	importer->Destroy();

	// モーション情報取得
	LoadMotion(scene);

	//	モデルを材質ごとに分割
	FbxGeometryConverter fgc(manager);
	fgc.SplitMeshesPerMaterial(scene, true);
	fgc.Triangulate(scene, true);

	//	メッシュ数
	mMeshCount = scene->GetSrcObjectCount<FbxMesh>();
	Log::Info("[FBXSDK_LOADER] Mesh size : %d", mMeshCount);

	// メッシュコンテナのサイズ設定
	mMeshes.resize(mMeshCount);

	for (int m = 0; m < mMeshCount; ++m)
	{
		Log::Info("[FBXSDK_LOADER] Loading mesh (index : %d)", m);

		FbxMesh* fbxmesh = scene->GetSrcObject<FbxMesh>(m);
		int numberOfVertices = fbxmesh->GetPolygonVertexCount();

		MeshData& mesh = mMeshes[m];

		// 現在のメッシュにおける頂点数をセット
		mVertexCount = numberOfVertices;
		Log::Info("[FBXSDK_LOADER] Vertex size : %d", mVertexCount);

		// subsetの数を読みこむ
		mMaterialCount = fbxmesh->GetNode()->GetMaterialCount();
		Log::Info("[FBXSDK_LOADER] Material size : %d", mMaterialCount);
		SetUpSubsets(fbxmesh, mesh);

		// 頂点情報の読み込み
		LoadVertices(fbxmesh, mesh);

		


	}

	scene->Destroy();
	manager->Destroy();

	return true;
}

void FbxSdkLoader::LoadMotion(FbxScene* scene)
{
	FbxArray<FbxString*> names;
	scene->FillAnimStackNameArray(names);

	if (names != NULL)
	{
		// モーションが存在するとき
		FbxTakeInfo* take = scene->GetTakeInfo(names[0]->Buffer());
		FbxLongLong start = take->mLocalTimeSpan.GetStart().Get();
		FbxLongLong stop = take->mLocalTimeSpan.GetStop().Get();
		FbxLongLong fps60 = FbxTime::GetOneFrameValue(FbxTime::eFrames60);
		motionData.startFrame = (int)(start / fps60);
		motionData.frameCount = (int)((stop - start) / fps60);
	}
	else
	{
		motionData.startFrame = 0;
		motionData.frameCount = 0;
	}
}

void FbxSdkLoader::LoadVertices(FbxMesh* fbxmesh, MeshData& mesh)
{
	int* index = fbxmesh->GetPolygonVertices();

	//-------------------------------------------------------------------------------
	// Prepare for getting VERTEX_POSITION
	//-------------------------------------------------------------------------------
	FbxVector4* source = fbxmesh->GetControlPoints();
	// メッシュのトランスフォーム
	FbxVector4 T = fbxmesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 R = fbxmesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 S = fbxmesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot);
	FbxAMatrix TRS = FbxAMatrix(T, R, S);

	for (int v = 0; v < fbxmesh->GetControlPointsCount(); ++v)
	{
		source[v] = TRS.MultT(source[v]);
	}

	//-------------------------------------------------------------------------------
	// Prepare for getting VERTEX_NORMAL
	//-------------------------------------------------------------------------------
	FbxArray<FbxVector4> normals;
	fbxmesh->GetPolygonVertexNormals(normals);

	assert(normals.Size() == mVertexCount);


	//-------------------------------------------------------------------------------
	// Prepare for getting VERTEX_TEXCOORD
	//-------------------------------------------------------------------------------
	FbxStringList names;
	fbxmesh->GetUVSetNames(names);
	FbxArray<FbxVector2> uv;
	fbxmesh->GetPolygonVertexUVs(names.GetStringAt(0), uv);

	//-------------------------------------------------------------------------------
	// Prepare for getting VERTEX_COLOR
	//-------------------------------------------------------------------------------
	int vColorLayerCount = fbxmesh->GetElementVertexColorCount();
	int indexCount = 0;
	FbxLayerElementArrayTemplate<int>* indexOfLayerElement = nullptr;
	FbxGeometryElementVertexColor* element = nullptr;
	if (fbxmesh->GetElementVertexColorCount() > 0)
	{
		// 保存形式の取得
		element = fbxmesh->GetElementVertexColor(0);
		FbxGeometryElement::EMappingMode mapMode = element->GetMappingMode();
		FbxGeometryElement::EReferenceMode refMode = element->GetReferenceMode();

		// ポリゴン頂点に対するインデックス参照形式のみ対応
		
		if (mapMode == FbxGeometryElement::eByPolygonVertex)
		{
			if (refMode == FbxGeometryElement::eIndexToDirect)
			{
				indexOfLayerElement = &element->GetIndexArray();
				indexCount = indexOfLayerElement->GetCount();
				assert(indexCount == mVertexCount);
			}
		}
	}


	//-------------------------------------------------------------------------------
	// Prepare for getting VERTEX_TANGENT
	//-------------------------------------------------------------------------------
	fbxmesh->GenerateTangentsData(0, true);
	bool tanFlag = fbxmesh->GetElementTangentCount();
	FbxGeometryElementTangent* tangents = fbxmesh->GetElementTangent(0);
	tanFlag = tangents != nullptr ? true : false;


	//-------------------------------------------------------------------------------
	// Prepare for getting VERTEX_BINORMAL
	//-------------------------------------------------------------------------------
	bool binormalFlag = fbxmesh->GetElementBinormalCount();
	FbxGeometryElementBinormal* binormals = fbxmesh->GetElementBinormal(0);

	mesh.vertices.resize(mVertexCount);
	for (int v = 0; v < mVertexCount; ++v)
	{
		int vIndex = index[v];

		//  POSITION
		mesh.vertices[v].position.x = static_cast<float>(source[vIndex][0]);
		mesh.vertices[v].position.y = static_cast<float>(source[vIndex][1]);
		mesh.vertices[v].position.z = static_cast<float>(source[vIndex][2]);

		// NORMAL
		mesh.vertices[v].normal.x = static_cast<float>(normals[v][0]);
		mesh.vertices[v].normal.y = static_cast<float>(normals[v][1]); 
		mesh.vertices[v].normal.z = static_cast<float>(normals[v][2]);

		// TEXCOORD
		mesh.vertices[v].texcoord.x = static_cast<float>(uv[v][0]);
		mesh.vertices[v].texcoord.y = static_cast<float>(1.0f - uv[v][1]);

		// COLOR
		if (indexCount > 0)
		{
			FbxColor c = element->GetDirectArray().GetAt(indexOfLayerElement->GetAt(v));
			mesh.vertices[v].color.x = static_cast<float>(c.mRed);
			mesh.vertices[v].color.y = static_cast<float>(c.mGreen);
			mesh.vertices[v].color.z = static_cast<float>(c.mBlue);
			mesh.vertices[v].color.w = static_cast<float>(c.mAlpha);
		}

		// TANGENT
		if (tanFlag)
		{
			FbxVector4 t = tangents->GetDirectArray().GetAt(v);
			mesh.vertices[v].tangent.x = static_cast<float>(t[0]);
			mesh.vertices[v].tangent.y = static_cast<float>(t[1]);
			mesh.vertices[v].tangent.z = static_cast<float>(t[2]);
		}

		// BINORMAL
		if (binormalFlag)
		{
			FbxVector4 b = binormals->GetDirectArray().GetAt(v);
			mesh.vertices[v].binormal.x = static_cast<float>(b[0]);
			mesh.vertices[v].binormal.y = static_cast<float>(b[1]);
			mesh.vertices[v].binormal.z = static_cast<float>(b[2]);
		}
	}
}

void FbxSdkLoader::LoadMeshAnimation(FbxMesh* fbxmesh, MeshData& mesh)
{
	FbxNode* node = fbxmesh->GetNode();

	int indexOfBone = mBoneCount;
	mBoneTable[indexOfBone].name = node->GetName();

	// オフセット行列
	mBoneTable[indexOfBone].offset = IDENTITYMATRIX;

	// キーフレーム読み込み
	LoadKeyFrames("default", indexOfBone, node);

	// ウェイト設定
	int numberOfVertex = fbxmesh->GetPolygonVertexCount();
	for (int i = 0; i < numberOfVertex; ++i)
	{
		mVerticesBoneTable.emplace_back();
		mVerticesBoneTable[i].index[0] = indexOfBone;
		mVerticesBoneTable[i].weight[0] = 1.0f;
		mVerticesBoneTable[i].count = 1;
	}
	mBoneCount++;
}

void FbxSdkLoader::LoadSkins(FbxMesh* fbxmesh, BoneData& bone, MeshData& mesh)
{
	// 頂点数取得
	const int numberOfVertices = fbxmesh->GetPolygonVertexCount();

	// スキン情報の有無
	const int numberOfSkins = fbxmesh->GetDeformerCount(FbxDeformer::eSkin);
	if (numberOfSkins <= 0)
	{
		// LoadMeshAnim(fbxmesh);
		return;
	}
	FbxSkin* skin = static_cast<FbxSkin*>(fbxmesh->GetDeformer(0, FbxDeformer::eSkin));

	// ボーン数
	const int numberOfBones = skin->GetClusterCount();

	// 全ボーン情報取得
	for (int b = 0; b < numberOfBones; ++b)
	{
		// ボーン情報取得
		FbxCluster* cluster = skin->GetCluster(b);
		FbxAMatrix trans;
		cluster->GetTransformMatrix(trans);
		
		// ボーン名取得
		const char* name = cluster->GetLink()->GetName();

		// ボーン検索
		bool isNewBone = false;
		int indexOfBone = FindBone(name);
		if (indexOfBone < 0)
		{
			indexOfBone = mBoneCount;
			mBoneCount++;
			isNewBone = true;
		}

		
		if (isNewBone) // 既出ボーンでないなら
		{
			// ボーン情報の登録
			mBoneTable.emplace_back();
			mBoneTable[indexOfBone].name = name;

			// オフセット行列
			FbxAMatrix linkMatrix;
			cluster->GetTransformLinkMatrix(linkMatrix);

			FbxAMatrix offset = linkMatrix.Inverse() * trans;
			FbxDouble* offsetM = (FbxDouble*)offset;

			for (int i = 0; i < 16; ++i)
			{
				mBoneTable[indexOfBone].offset.m[i / 4][i % 4] = (float)offsetM[i];
			}

			LoadKeyFrames("default", indexOfBone, cluster->GetLink());
		}

		// ウェイト読み込み
		int weightCount = cluster->GetControlPointIndicesCount();
		int* weightIndex = cluster->GetControlPointIndices();
		double* weight = cluster->GetControlPointWeights();

		int* indexOfVertices = fbxmesh->GetPolygonVertices();

		for (int i = 0; i < weightCount; ++i)
		{
			int weightIndex2 = weightIndex[i];

			// 前ポリゴンからweightIndex2番目の頂点検索
			for (int v = 0; v < numberOfVertices; ++v)
			{
				if (indexOfVertices[v] != weightIndex2) continue;

				// 頂点にウェイト保存
				int w = mesh.verticesBone[v].count;
				if (w >= 4) continue;

				mesh.verticesBone[v].index[w] = indexOfBone;
				mesh.verticesBone[v].weight[w] = (float)weight[i];
				mesh.verticesBone[v].count++;
			}
		}
	}
}

int FbxSdkLoader::FindBone(const char* name)
{
	int indexOfBone = -1;
	for (auto i = 0; i < mBoneTable.size(); ++i)
	{
		if (mBoneTable[i].name == std::string(name))
		{
			indexOfBone = i;
			break;
		}
	}
	return indexOfBone;
}

void FbxSdkLoader::LoadKeyFrames(std::string name, int bone, FbxNode* bone_node)
{
	motionData.keys.emplace_back(new XMFLOAT4X4[motionData.frameCount + 1]);

	double time = motionData.startFrame * (1.0 / 60.0);
	FbxTime T;

	for (int f = 0; f < motionData.frameCount; ++f)
	{
		T.SetSecondDouble(time);

		// T秒の姿勢行列をGet
		FbxMatrix m = bone_node->EvaluateGlobalTransform(T);
		FbxDouble* mat = (FbxDouble*)m;

		for (int i = 0; i < 16; ++i)
		{
			motionData.keys[bone][f].m[i / 4][i % 4] = (float)mat[i];
		}

		time += 1.0 / 60.0;
	}
}

void FbxSdkLoader::SetUpSubsets(FbxMesh* fbxmesh, MeshData& mesh)
{
	// メッシュ内のサブセットコンテナを初期化
	mesh.subsets.resize(mMaterialCount > 0 ? mMaterialCount : 1);

	// 各サブセットのポリゴン数を取得する。
	const int numberOfPolygons = fbxmesh->GetPolygonCount();
	for (int i = 0; i < numberOfPolygons; ++i)
	{
		const uint32_t subsetIndex = fbxmesh->GetElementMaterial()->GetIndexArray().GetAt(i);
		mesh.subsets[subsetIndex].indexCount += 3;
	}

	// 各サブセットのオフセットを記録
	int offset = 0;
	for (auto& subset : mesh.subsets)
	{
		subset.indexStart = offset;
		offset += subset.indexCount;
		subset.indexCount = 0;
	}

	mesh.indices.resize(numberOfPolygons * 3);
	Log::Info("[FBXSDK_LOADER] Polygon size : %d", numberOfPolygons);

	uint32_t vertexCount = 0;
	for (int i = 0; i < numberOfPolygons; ++i)
	{
		Log::Info("[FBXSDK_LOADER] polygon index : %d", i);
		const int indexOfMaterial = fbxmesh->GetElementMaterial()->GetIndexArray().GetAt(i);
		MeshSubset& subset = mesh.subsets.at(indexOfMaterial);
		const int indexOffset = subset.indexStart + subset.indexCount;
		const int numberOfVertices = fbxmesh->GetPolygonSize(i);

		for (int v = 0; v < numberOfVertices; ++v)
		{
			mesh.indices.at(indexOffset + v) = vertexCount;
			vertexCount++;
		}
		subset.indexCount += 3;
	}
	Log::Info("[FBXSDK_LOADER] Finish setting subsets and indices.");
}

//uint vertex_count;
//indices.resize(numberOfPolygons * 3);
//
//
//for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; indexOfPolygon++)
//{
//	// the material for current face
//	int indexOfMaterial = 0;
//	if (mNumberOfMaterials > 0)
//	{
//		indexOfMaterial = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);
//	}
//
//	// the place I should hold the vertex attribute index
//	FbxInfo::Subset& subset = mesh.mSubsets.at(indexOfMaterial);
//	const int indexOffset = subset.indexStart + subset.indexCount;
//
//	const int numberOfVertices = fbxMesh->GetPolygonSize(indexOfPolygon);
//
//	for (int indexOfVertex = 0; indexOfVertex < 3; indexOfVertex++)
//	{
//		const int indexOfControlPoint = fbxMesh->GetPolygonVertex(indexOfPolygon, indexOfVertex);
//
//		vertices.push_back(vertex);
//		indices.push_back(vertex_count);
//		indices.at(indexOffset + indexOfVertex) = static_cast<u_int>(vertex_count);
//		vertex_count += 1;
//
//
//
//		int vertexid = vertices.size() - 1;
//		if (indexOfControlPoint > 0)
//		{
//			auto vector_size = boneInfluences.at(indexOfControlPoint).size();
//			for (auto i = 0u; i < vector_size; ++i)
//			{
//				vertices[vertexid].bone_indices[i] = boneInfluences.at(indexOfControlPoint).at(i).index;
//				vertices[vertexid].bone_weights[i] = boneInfluences.at(indexOfControlPoint).at(i).weight;
//
//			}
//		}
//
//	}
//	subset.indexCount += 3;
//}

