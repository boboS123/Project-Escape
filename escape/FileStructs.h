#ifndef FILESTRUCTS_H
#define FILESTRUCTS_H
#include "stdafx.h"
#include <string>
using namespace std;

struct __declspec(dllexport) boundingBOX {
	XMVECTOR Min;
	XMVECTOR Max;
};


#define vertexFormat_pos2 1
#define vertexFormat_pos3  2
#define vertexFormat_pos4  4
#define vertexFormat_tex2  8
#define vertexFormat_nrm3  16
#define vertexFormat_tgn3  32
#define vertexFormat_bin3  64


enum VERTEX_FORMAT {
	POS2,
	POS3,
	POS4,
	POS4_TEX2,
	POS4_TEX2_NORM3,
	POS4_TEX2_NORM3_TANGENTS3,
	POS4_TEX2_NORM3_TANGENTS3_BINORMALS
};




enum SceneObjectType {
	TREE,
	ROCK,
	VEGETATION,
	ACTIVEPLANT,
	POTION,
	CHEST,
	WEAPON,
	MAGICSCROLL,
	WATERPLANE,
	STUMP,
	GAZEBO,
	BOOK,
	LAMP,
	TABLE,
	WATERWELL,
	BARREL,
	FIREPIT,
	VASE,
	FENCE,
	TOMB,
	LOG,
	COIN,
	FOOD

};



struct ChunkInfo {

	int numLods;
	boundingBOX bounding;
	DWORD location;
	DWORD adressLOD2;
	DWORD adressLOD3;
	DWORD CollisionPackAdress;
	XMMATRIX transform;
	//DWORD size;

};


//Header for the mesh data

struct MeshInfo {
	int id;
	int numVerts;
	int format;
	XMFLOAT3 maxAABB;
	XMFLOAT3 minAABB;

	string name;
	DWORD adress;
	DWORD size;
	DWORD compressed_size;
	VERTEX_FORMAT vertexFormat;
	int bitFieldVertexFormat;
	bool operator==(MeshInfo &elem) {
		if (this->name == elem.name) return 1;
		else return 0;
	}


};




struct InstanceInfo {

	//Instance ID
	int ID;

	//name and other text description for editor and debug
	int descriptionID;


	//Object Type
	SceneObjectType ObjectType;



	//Mesh ID for rendering
	int meshID_LOD1;
	int meshID_LOD2;
	int meshID_LOD3;

	//When to trigger LOD meshes
	int distanceLod2;
	int distanceLod3;

	//Physics data for collision
	int phisicsID_LOD1;
	int phisicsID_LOD2;
	int phisicsID_LOD3;


	//Material ID
	int materialID_LOD1;
	int materialID_LOD2;
	int materialID_LOD3;


	//Transformation matrix
	XMMATRIX transformation;

	//Axis Aligned Bounding Box
	XMFLOAT3 AABB[2];

	//Name of the transform
	string name;



};

struct TextureInfo {
	int id;
	string name;
	DWORD adress;
	DWORD size;

	bool operator==(TextureInfo &tex) {
		if (this->name == tex.name) return 1;
		else return 0;
	}

	bool operator<(TextureInfo &tex1) {
		if (this->name < tex1.name) return 1;
		else return 0;

	}

	bool operator>(TextureInfo &tex1) {
		if (this->name > tex1.name) return 1;
		else return 0;

	}



};



struct MaterialInfo {

	int id;
	string name;
	DWORD adress;
	DWORD size;
	int texture_id_color;
	int texture_id_normal;
	int texture_id_specular;


	bool operator==(MaterialInfo &mat2) {
		if (this->name == mat2.name) return 1;
		else return 0;
	}



};

struct MaterialHeader {
	int ID;
	int colorID;
	int normalID;
	int specularID;
};


struct TextureHeader {
	int ID;
	DWORD adress;
	DWORD size;
};











enum MESHLAYOUT {

	POS4COLOR4,
	POS4UV2,
	POS4_UV2_NORMAL3,
	POS4_UV2_NORMAL3_TANGENT3
};


struct MeshHeader {
	int MeshID;
	float AABB[6];
	int numVerts;
	DWORD meshAdress;
	DWORD MeshSize;
	DWORD CompressedSize;
	int vertexType;
};

#endif