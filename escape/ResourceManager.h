#pragma once
#include <DirectXMath.h>
#include <windows.h>
#include "XLibrary/XVector.h"
#include "XLibrary/XFile2.h"
#include "stdafx.h"
#include "LZ4/lz4.h"
#include "FileStructs.h"
#include <memory>
#include <algorithm>
#include "DDSTextureLoader.h"

using namespace DirectX;
#define DDS_HEIGHT 0x00000002 // DDSD_HEIGHT
#define MY_IID_PPV_ARGS IID_PPV_ARGS
enum DDS_RESOURCE_MISC_FLAG
{
	DDS_RESOURCE_MISC_TEXTURECUBE = 0x4L,
};

D3D12_CPU_DESCRIPTOR_HANDLE textureView;
ID3D12Resource** texture;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> textureDescriptionsHeap = nullptr;

XVector<Microsoft::WRL::ComPtr<ID3D12Resource>> textureResourceTmpVec;
XVector<Microsoft::WRL::ComPtr<ID3D12Resource>> UploadHeapTmpVec;

enum FileType {
	DATA_FOLDER,
	INSTANCE_FILE,
	MESH_FILE,
	MATERIAL_FILE,
	TEXTURE_FILE,
	SOUND_FILE,
	GUI_FILE,
	SHADER_FILE

};


enum ObjectType {
	STATIC_MESH
};



struct DirectXResource_ {

	ID3D12Resource* vertexBuffer; 
	ID3D12Resource* indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	char *rawData;
	UINT sizeofVB;
	UINT sizeofIB;
	UINT vertCount;




};

struct instanceObject {

	INT instanceID;


	//flags
	ObjectType objType;
	bool rawDataAvaible;
	int numLods;
	float Lod_1_visibility_range;
	float Lod_2_visibility_range;
	float Lod_3_visibility_range;

	INT rawDataVertexID;
	INT rawDataPhisicsID;

	INT transformationMatrix;

	
	//INT shaderID;

	INT materialID;
	INT meshID;

	UINT objectType;


};



struct RenderReadyObject {

	//Data for the command list
	D3D12_VERTEX_BUFFER_VIEW *vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW *indexBufferView;
	D3D12_GPU_DESCRIPTOR_HANDLE *texture;
	ID3D12Resource* constantBufferUploadHeaps;
	ID3D12RootSignature* rootSignature;
	ID3D12PipelineState* pipelineStateObject;

	D3D_PRIMITIVE_TOPOLOGY topology;

	XMMATRIX transformation;
	int numTriangles;

};



struct TextureResource {

};




class ResourceManager {

public:


	ID3D12Resource* tex = nullptr;

	


	ID3D12Device* device;
	ID3D12GraphicsCommandList* commandList;
	ID3D12DescriptorHeap* mainDescriptorHeap;


	void SetDevice(ID3D12Device* dev) {
		device = dev;
	}

	void SetCommandList(ID3D12GraphicsCommandList* cmdLst) {
		commandList = cmdLst;
	}

	void SetDescrptionHeap(ID3D12DescriptorHeap* hp) {
		mainDescriptorHeap = hp;
	}




	XFile2 *instanceFile;
	XFile2 *meshFile;
	XFile2 *materialFile;
	XFile2 *textureFile;





	int numInstancesTotalFile;
	int numMaterialsTotalFile;
	int numTexturesTotalFile;
	//A vector with scene objects


	XVector<char*> rawData;

	XVector<instanceObject> instanceHeader;
	XVector<MeshHeader> mHeader;
	XVector<MaterialHeader> materials;
	XVector<TextureHeader> textures;
	bool MeshHeaderLoaded;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;


	XVector<ID3D12Resource*> gpuAdresses;
	char *allMeshRawData;

	ID3D12Resource* vgpuAdresses;

	int renderObjects = 0;
	XVector<int> ActiveMeshListID;
	XVector<int> ActiveMaterialListID;
	XVector<int> ActiveTexturesListID;



	XVector<RenderReadyObject> Dx12ActiveObjects;
	



	int getnumInstancesTotalFile(){
		return numInstancesTotalFile;
	}




	INT getHeaderVertexID(int i) {
		return instanceHeader[i].meshID;
	}

	INT getInstancesTextureMap(int i) {
		return instanceHeader[i].materialID;
	}



	INT getInstancesObjectType(int i) {
		return instanceHeader[i].objectType;
	}

	INT getInstancesInstanceID(int i) {
		return instanceHeader[i].instanceID;
	}





	INT getMeshHeaderNumMeshes(int i) {
		return mHeader.count;
	}








	void *getInstanceObjectHeader(){

		return &instanceHeader;
	}

	void *getMaterialHeader() {

		return &mHeader;
	}


	void *getMeshHeader() {

		return &materials;
	}


	void *getTestureHeader() {

		return &textures;
	}




	//mtHdr[0].colorID;





	//Unload headers from memory

	int unloadMaterialHeader() {
		materials.Destroy();
	}

	int unloadTextureHeader() {
		textures.Destroy();
	}

	int unloadMeshHeader() {
		mHeader.Destroy();
		if (meshFile->isFileOpen()) {
			meshFile->CloseFile();
		}
	}


	int unloadInstanceHeader() {
		instanceHeader.Destroy();
		
		if (instanceFile->isFileOpen()) {
			instanceFile->CloseFile();
		}

	}



	int LoadInstances() {

		//Open the entity file;
		instanceFile = new XFile2(L"C:\\Workspace\\_FinalGame\\Debug\\data\\instances.bgd", GENERIC_READ, OPEN_ALWAYS);
		instanceFile->OpenFile();

		instanceFile->LoadChunk(0, sizeof(int));
		numInstancesTotalFile = *(int*)instanceFile->ReadFromChunk(sizeof(int));


		instanceHeader.ReserveAndFill(numInstancesTotalFile, numInstancesTotalFile);



		instanceFile->LoadChunk(sizeof(int), numInstancesTotalFile* (sizeof(int) * 4 + sizeof(float) * 22));
		for (int i = 0; i < numInstancesTotalFile; i++) {
			instanceHeader[i].instanceID = *(int*)instanceFile->ReadFromChunk(sizeof(int));
			instanceHeader[i].objectType = *(int*)instanceFile->ReadFromChunk(sizeof(int));
			instanceHeader[i].materialID = *(int*)instanceFile->ReadFromChunk(sizeof(int));
			instanceHeader[i].meshID = *(int*)instanceFile->ReadFromChunk(sizeof(int));



			for (int j = 0; j < 22; j++) {

				*(int*)instanceFile->ReadFromChunk(sizeof(float));

			}



		}


		instanceHeader;




		return numInstancesTotalFile;

	}









	int loadMaterials() {

		materialFile = new XFile2(L"C:\\Workspace\\_FinalGame\\Debug\\data\\materials.bgd", GENERIC_READ, OPEN_ALWAYS);
		materialFile->OpenFile();
		materialFile->LoadChunk(0, sizeof(int));

		numMaterialsTotalFile = *(int*)materialFile->ReadFromChunk(sizeof(int));

		materialFile->LoadChunk(sizeof(int), numMaterialsTotalFile* (sizeof(int) * 4));

		//Manually create the header vector so we don't have to make useless copy process
		materials.ReplaceArrayPointer(materialFile->GetInternalBuffer());
		materials.SetCount(numMaterialsTotalFile);


		//int test = materials[1].id;
		materialFile->CloseFile();
		return numMaterialsTotalFile;
		
	}


	int loadTextureHeader(){

		textureFile = new XFile2(L"C:\\Workspace\\_FinalGame\\Debug\\data\\textures.bgd", GENERIC_READ, OPEN_ALWAYS);
		textureFile->OpenFile();
		textureFile->LoadChunk(0, sizeof(int));
		numTexturesTotalFile = *(int*)textureFile->ReadFromChunk(sizeof(int));

		textureFile->LoadChunk(sizeof(int), numTexturesTotalFile*(sizeof(DWORD) * 2 + sizeof(int)));

		//Manually create the header vector so we don't have to make useless copy process
		textures.ReplaceArrayPointer(textureFile->GetInternalBuffer());
		textures.SetCount(numTexturesTotalFile);
		//textureFile->CloseFile();
		return 1;
	}


	int loadmeshHeader() {


		meshFile = new XFile2(L"C:\\Workspace\\_FinalGame\\Debug\\data\\mesh.bgd", GENERIC_READ, OPEN_ALWAYS);
		meshFile->OpenFile();

		meshFile->LoadChunk(0, sizeof(int));
		int numMeshesOnFile = *(int*)meshFile->ReadFromChunk(sizeof(int));


		meshFile->LoadChunk(sizeof(int), (numMeshesOnFile* (sizeof(int) + 6 * (sizeof(float)) + sizeof(int) + sizeof(DWORD) * 3) + sizeof(int) ));

		mHeader.ReplaceArrayPointer(meshFile->GetInternalBuffer());
		mHeader.SetCount(numMeshesOnFile);

		int test1 = mHeader[0].CompressedSize;

		int test2 = mHeader[1].MeshSize;
		int test4 = mHeader[1].MeshID;
		int format = mHeader[0].vertexType;


		//Load 1st mesh



		DWORD test_adress = mHeader[0].meshAdress;
		DWORD compressedSize = mHeader[0].CompressedSize;
		DWORD numVerts_test = mHeader[0].numVerts;
		DWORD actual_size = mHeader[0].MeshSize;




		float aabb = mHeader[0].AABB[0];
		float aabb1 = mHeader[0].AABB[1];
		float aabb2 = mHeader[0].AABB[2];
		float aabb3 = mHeader[0].AABB[3];
		float aabb4 = mHeader[0].AABB[4];
		float aabb5 = mHeader[0].AABB[5];

		//Load the mesh
		meshFile->LoadChunk(mHeader[0].meshAdress, mHeader[0].CompressedSize);

		char* UncompressedMesh = new char[mHeader[0].MeshSize];

		int result = LZ4_decompress_fast(meshFile->GetInternalBuffer(), UncompressedMesh, mHeader[0].MeshSize);


		if (result < 0) {
			MessageBox(NULL, L"LZ4 Decompression Failed", L"Failure", NULL);
		}


	


		return 1;
	}







	//Internal stuff
	char *loadMeshRaw(bool decompress, int ID, char *adressInMemory) {

// 		if (!MeshHeaderLoaded) {
// 			return nullptr;
// 		}



		meshFile->LoadChunk(mHeader[ID].meshAdress, mHeader[ID].CompressedSize);

		adressInMemory = new char[mHeader[ID].MeshSize];

		int result = LZ4_decompress_fast(meshFile->GetInternalBuffer(), adressInMemory, mHeader[ID].MeshSize);

		if (result < 0) {
			MessageBox(NULL, L"LZ4 Decompression Failed", L"Failure", NULL);
		}

		//pass the adress
		return adressInMemory;


	}


	D3D12_VERTEX_BUFFER_VIEW UploadBufferToGPU(char* rawData, int vBufferSize) {
		
		

		//a GPU heap
		HRESULT hr = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
			D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
											// from the upload heap to this heap
			nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
			IID_PPV_ARGS(&vgpuAdresses));
		
		
		if (FAILED(hr))
		{

			MessageBox(NULL, L"	vgpuAdresses failed  Failed", L"failed", NULL);
			//		Running = false;
			//	return false;
		}

		// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
		vgpuAdresses->SetName(L"Vertex Buffer Resource Heap");



		// create upload heap
		// upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
		// We will upload the vertex buffer using this heap to the default heap
		ID3D12Resource* vBufferUploadHeap;

		hr = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
			D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
			nullptr,
			IID_PPV_ARGS(&vBufferUploadHeap));
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Upload vertex buffer  Failed", L"failed", NULL);
			//Running = false;
			//return false;
		}
		vBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

		// store vertex buffer in upload heap
		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(rawData); // pointer to our vertex array
		vertexData.RowPitch = vBufferSize; // size of all our triangle vertex data
		vertexData.SlicePitch = vBufferSize; // also the size of our triangle vertex data

											 // we are now creating a command with the command list to copy the data from
											 // the upload heap to the default heap
		UpdateSubresources(commandList, vgpuAdresses, vBufferUploadHeap, 0, 0, 1, &vertexData);

		// transition the vertex buffer data from copy destination state to vertex buffer state
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vgpuAdresses, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));


		vertexBufferView.BufferLocation = vgpuAdresses->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(Vertex2);
		vertexBufferView.SizeInBytes = vBufferSize;

		return vertexBufferView;
	}


	char *loadTexture(int ID) {

		textureFile->OpenFile();
		int test = textures[ID].size;
		textureFile->LoadChunk(textures[ID].adress, textures[ID].size);
		return textureFile->GetInternalBuffer();

	}



	void LoadScene() {

	//	int num = LoadInstances();

















		int totalResourcesSize = 0;
		int activeMeshesSize = 0;
		int activeTexturesSize = 0;


		//We start by loading the meshes for the instances available in the scene
		//We sort the needed meshes in a sorted vector and we avoid duplicates

		for (int i = 0; i < instanceHeader.count; i++) {
			ActiveMeshListID.InsertSorted(instanceHeader[i].meshID);

		}





		//For every instance present we can have a different material even if the mesh is repeated
		//We sort the materials and we avoid duplicates
		for (int i = 0; i < instanceHeader.count; i++) {
			ActiveMaterialListID.InsertSorted(instanceHeader[i].materialID);

		}





		//For every material loaded we make a list of needed textures
		//we sort the list and we avoid duplicates

		
		for (int i = 0; i < ActiveMaterialListID.count; i++) {


			if (materials[ActiveMaterialListID[i]].colorID != -1) {
				ActiveTexturesListID.InsertSorted(materials[ActiveMaterialListID[i]].colorID);
			}



			if (materials[ActiveMaterialListID[i]].normalID != -1) {
				ActiveTexturesListID.InsertSorted(materials[ActiveMaterialListID[i]].normalID);

			}


			if (materials[i].specularID != -1) {
				ActiveTexturesListID.InsertSorted(materials[ActiveMaterialListID[i]].specularID);

			}

		}


		
		//After making the lists we start loading the resources
		///////////////////////////////////////////////////////







		//ActiveMeshListID = meshes that need to be loaded

		for (int i = 0; i < ActiveMeshListID.count; i++) {

			
			char *myMesh = nullptr;
			myMesh = loadMeshRaw(1, ActiveMeshListID[i], myMesh);
			vertexBufferView = UploadBufferToGPU(myMesh, mHeader[ActiveMeshListID[i]].MeshSize);
			//delete myMesh;


			RenderReadyObject obj1;
			obj1.vertexBufferView = &vertexBufferView;
			obj1.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			obj1.numTriangles = mHeader[ActiveMeshListID[i]].numVerts;
			obj1.transformation = XMMatrixIdentity();
			//obj1.texture



			Dx12ActiveObjects.Insert(obj1);
			activeMeshesSize += mHeader[ActiveMeshListID[i]].MeshSize;

		}




		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = ActiveTexturesListID.count;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		HRESULT create = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&textureDescriptionsHeap));




		//ActiveTexturesListID = textures that need to be loaded

		for (int i = 0; i < ActiveTexturesListID.count; i++) {

			Microsoft::WRL::ComPtr<ID3D12Resource> textureResourceTmp;
			Microsoft::WRL::ComPtr<ID3D12Resource>  UploadHeapTmp;

			HRESULT loadFromMemory = CreateDDSTextureFromMemory12(device,
				commandList, (const uint8_t*)loadTexture(ActiveTexturesListID[i]), textures[ActiveTexturesListID[i]].size,
				textureResourceTmp, UploadHeapTmp);


			textureResourceTmpVec.Insert(textureResourceTmp);
			UploadHeapTmpVec.Insert(UploadHeapTmp);


			CD3DX12_CPU_DESCRIPTOR_HANDLE handleToDescriptor(textureDescriptionsHeap->GetCPUDescriptorHandleForHeapStart());
			UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			//move the descriptor
			handleToDescriptor.Offset(i, descriptorSize);

			auto woodCrateTex2 = textureResourceTmpVec[i];

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2 = {};
			srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc2.Format = woodCrateTex2->GetDesc().Format;
			srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc2.Texture2D.MostDetailedMip = 0;
			srvDesc2.Texture2D.MipLevels = woodCrateTex2->GetDesc().MipLevels;
			srvDesc2.Texture2D.ResourceMinLODClamp = 0.0f;

			device->CreateShaderResourceView(woodCrateTex2.Get(), &srvDesc2, handleToDescriptor);



		}





		//Iterate and search only for objects in the proximity of the player






		int txt = activeTexturesSize;


		//A buffer for all meshes that needs to be loaded
		allMeshRawData = new char[activeMeshesSize];




	}




	// Load, unload create and destroy objects based on player's position
	void UpdateScene(XMFLOAT3 &playerPos, float distanceView) {










	}





	//Render loaded objects that are "render ready"
	void RenderScene(XMFLOAT3 &playerPos, float distanceView) {


		for (int i = 0; i < renderObjects; i++) {
	
		

		//	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
		//	commandList->IASetVertexBuffers(0, 1, &mgr.vertexBufferView); // set the vertex buffer (using the vertex buffer view)
			


		
		
		}






	}








	


	//VertexBuffer
	int CreateSceneBuffer();





	LPCWSTR DataFolder;
	
	LPCWSTR InstanceFile;
	LPCWSTR MeshFile;
	LPCWSTR MaterialFile;
	LPCWSTR TextureFile;
	LPCWSTR SoundFile;
	LPCWSTR guiFile;
	LPCWSTR shaderFile;


	//variables for loading and unloading data
	XMFLOAT3 loadingResDistance;
	XMFLOAT3 unloadResDistance;




	//load raw data












};