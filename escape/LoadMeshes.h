#pragma once
#include <DirectXMath.h>
using namespace DirectX;
class __declspec(dllexport) MeshManager {




	//Load a mesh and return the adress of raw data;
	char*  LoadMesh(int ID,bool decompress);
	bool ReadHeader(int max_elements, XMFLOAT4 &playerPos);







};