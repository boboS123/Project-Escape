#pragma once
#include "maya_precomp.h"

#include "FileStructs.h"
#include "XLibrary/XFile2.h"
#include "XLibrary/XVector.h"

#pragma comment(lib,"escape.lib")

__int64 FileSize(const wchar_t *name);

class __declspec(dllexport) bobCATexporter {

public:

	LPCWSTR materialFile;
	LPCWSTR textureFile;
	LPCWSTR terrainFile;
	LPCWSTR architectureFile;
	LPCWSTR staticmeshFile;
	LPCWSTR instancesFile;
	int vertexFormat = 0;
	
		
			XFile2 *XMaterials;
			XFile2 *XTextures;
			XFile2 *XTerrain;
			XFile2 *XArchitecture;
			XFile2 *XStaticMesh;
		 
			XFile2 *Instances;

			
			XVector<MeshInfo> meshes;
			XVector<InstanceInfo> instances;
			XVector<TextureInfo> TextureHeader;
			XVector<MaterialInfo> MaterialHeader;


			MMatrix mat;
			~bobCATexporter();


			bobCATexporter(LPCWSTR materials, LPCWSTR textures, LPCWSTR terrain, LPCWSTR architecture, LPCWSTR staticmesh, LPCWSTR instances) :
				materialFile(materials), textureFile(textures), terrainFile(terrain), architectureFile(architecture), staticmeshFile(staticmesh),
				instancesFile(instances) {

				//Create files 
				XMaterials = new XFile2(materialFile, GENERIC_WRITE, CREATE_ALWAYS);
				XTextures = new XFile2(textureFile, GENERIC_WRITE, CREATE_ALWAYS);
				XTerrain = new XFile2(materialFile, GENERIC_WRITE, CREATE_ALWAYS);
				XArchitecture = new XFile2(materialFile, GENERIC_WRITE, CREATE_ALWAYS);
				XStaticMesh = new XFile2(staticmeshFile, GENERIC_WRITE, CREATE_ALWAYS);
				Instances = new XFile2(instancesFile, GENERIC_WRITE, CREATE_ALWAYS);


			}

			void CleanupBeforeExport();




			boundingBOX GetAABB(MPointArray &points);
			MString GetMeshMaterialName(MObject &object);

			bool ExportTextures();

			bool ExportMaterials();
			bool ExportMaterials_v2();

			bool ExportStaticMesh();
			bool FindMeshes();




			int ExportAllMeshes(bool compressed, XVector<MeshInfo> &sceneInfo, int vertexFormat);



			/*
			Search for transformation nodes

			*/

			int ExportInstances(bool compressed, XVector<InstanceInfo> &instanceVector);


			bool isGameObject(MFnTransform &instance);
			SceneObjectType getObjectType(MFnTransform &instance);
			void FixMatrix(XMMATRIX &matrix);




private:

	int GetNumMeshes();



	bool bobCATexporter::ExtractMeshDataToBuffer(MFnMesh &mesh, MESHLAYOUT layout, DWORD &size,
		MPointArray &points, MFloatVectorArray normals,
		MFloatVectorArray tangents, MFloatArray u_coords,
		MFloatArray v_coords, char **data, int &numVerts,VERTEX_FORMAT vertexFormat,int bitFieldVertexFormat);



	//Searches the material ID from the database by querying the name of a given surface shader
	int GetSurfaceShaderbyID(XVector<MaterialInfo>& materialDatabase, MFnMesh &object);
	MString GetSurfaceShaderbyString(MFnMesh &object);


	int GenerateAttributes(MFnTransform &object, SceneObjectType type);







};