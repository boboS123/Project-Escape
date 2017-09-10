#ifndef MAYA_PRECOMP_H
#define MAYA_PRECOMP_H


#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif


#include <maya/MIOStream.h>

#include <maya/MFloatMatrix.h>
#include <maya/MMatrix.h>
#include <maya/MLibrary.h>
#include <maya/MFnDagNode.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFnTransform.h>
#include <maya/MFileIO.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItSelectionList.h>
#include <maya/MEventMessage.h>


#include <maya/MCursor.h>
#include <maya/MEvent.h>
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MImage.h>
#include <maya/MPolyMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MGlobal.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnPointLight.h>
#include <maya/MBoundingBox.h>


#include <maya/MFnComponentListData.h>
#include <maya/MFnPhongShader.h>

//Files from the engine


// required to link the libraries



#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include<stdio.h>
#include<winsock2.h>
#include <mmsystem.h>
using namespace std;

#include <DirectXMath.h>
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <algorithm>



#include "d3dx12.h"
#include <string>
#include <wincodec.h>
#include <iostream>
#include <fstream>


using namespace DirectX;







#pragma comment(lib,"escape.lib")
#pragma comment(lib,"Foundation.lib")

#pragma comment(lib,"OpenMaya.lib")
#pragma comment(lib,"OpenMayaFx.lib")
#pragma comment(lib,"OpenMayaUi.lib")
#pragma comment(lib,"Image.lib")
#pragma comment(lib,"OpenMayaAnim.lib")





#endif