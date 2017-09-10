/*

Coded by Balan Bogdan





Maya Plug-in main file.
Dependencies: DirectX 12 library, escape engine system


*/






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

#include <maya/MPolyMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MGlobal.h>
#include <maya/MFnNumericAttribute.h>

#include <maya/MFnPhongShader.h>
#include "plugin.h"


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




#pragma comment(lib,"Foundation.lib")
#pragma comment(lib,"OpenMaya.lib")
#pragma comment(lib,"OpenMayaFx.lib")
#pragma comment(lib,"OpenMayaUi.lib")
#pragma comment(lib,"Image.lib")
#pragma comment(lib,"OpenMayaAnim.lib")

#include <maya/MSimple.h>


#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif


bobCATexporter myExporter(L"C:\\Workspace\\_FinalGame\\Debug\\data\\materials.bgd",
	L"C:\\Workspace\\_FinalGame\\Debug\\data\\textures.bgd",
	L"C:\\Workspace\\_bin\\Debug\\terrain.bgd",
	L"C:\\Workspace\\bin\\data\\arch.bgd",
	L"C:\\Workspace\\_FinalGame\\Debug\\data\\mesh.bgd",
	L"C:\\Workspace\\_FinalGame\\Debug\\data\\instances.bgd");




class SetObjectType : public MPxCommand {
public:
	virtual MStatus doIt(const MArgList & args);
	static void* creator() {
		return new SetObjectType;

	}



};




MStatus SetObjectType::doIt(const MArgList & args) {

	//

	int type = args.asInt(0);
	MSelectionList selected;
	MGlobal::getActiveSelectionList(selected);

	//cerr << "Whatsapp" << endl;

	for (int i = 0; i < selected.length(); ++i)
	{

		MObject obj;
		selected.getDependNode(i, obj);
		MFnDagNode fn(obj);

		MFnDagNode myDagNode(obj);




		switch (type)
		{
		case(0):

			//Make it as game object ready for export
			MFnNumericAttribute isGameObject;
			MObject gameObjAtt = isGameObject.create("gameObject", "gameObject", MFnNumericData::kBoolean, 1);
			fn.addAttribute(gameObjAtt);


			//Object type creation
			MFnNumericAttribute objectType;
			MObject objectTypeAtt = objectType.create("objectType", "objectType", MFnNumericData::kInt, 0);
			fn.addAttribute(objectTypeAtt);



			MFnNumericAttribute keyID;
			MObject keyAtt = keyID.create("keyID,0=unlocked", "keyID", MFnNumericData::kInt, 0);
			fn.addAttribute(keyAtt);

			MFnNumericAttribute trapped;
			MObject trapAtt = trapped.create("trapped,0=notrap", "trapLevel", MFnNumericData::kInt, 0);
			fn.addAttribute(trapAtt);

			MFnNumericAttribute pickableLevel;
			MObject pickAtt = pickableLevel.create("Pickable,0=notpickable", "pickLevel", MFnNumericData::kInt, 0);
			fn.addAttribute(pickAtt);


			break;



		}



	}




	MStatus blah;
	blah.setSuccess();
	return blah;

}





class exportAllScene : public MPxCommand {
public:
	virtual MStatus doIt(const MArgList& args);
	static void* creator() {
		return new exportAllScene;

	}
};


// All we need to do is supply the doIt method
// which in this case only prints "Hello" followed
// by the first argument given in the command line.
//



MCallbackId g_Callback;
MCallbackId gMesh;
MString  pisi;



EXPORT MStatus initializePlugin(MObject obj)
{
	MStatus		status;
	MFnPlugin	fnPlugin(obj, "XPlugin", "0.1", "Any");

	cout << "bobCAT Plugin 0.1 loaded" << " Compiled " << __DATE__ << " at " << __TIME__ << endl;



	status = fnPlugin.registerCommand("exportall",
		exportAllScene::creator);



	status = fnPlugin.registerCommand("SetObjectType",
		SetObjectType::creator);






	return MS::kSuccess;


}








MStatus exportAllScene::doIt(const MArgList& args) {

// 	for (int i = 0; i < SceneObjects.count; i++) {
// 
// 
// 		MFnDagNode fn(SceneObjects[i].object);
// 		cerr << fn.name().asChar() << endl;
// 	}
// 
// 



 	myExporter.ExportTextures();
 	myExporter.ExportMaterials_v2();
 	myExporter.ExportAllMeshes(1, myExporter.meshes);
 	myExporter.ExportInstances(1, myExporter.instances);
// 

	MStatus mstat;
	return mstat;

}






EXPORT MStatus uninitializePlugin(MObject obj)
{
	MStatus		status;
	MFnPlugin	fnPlugin(obj);


	// unregister the tool command

	status = fnPlugin.deregisterCommand("doMessage");
	status = fnPlugin.deregisterCommand("doPipi");
	status = fnPlugin.deregisterCommand("SetObjectType");
	status = fnPlugin.deregisterCommand("exportall");

	




	// check error
	if (!status) {
		status.perror("deregisterContextCommand");
		return status;
	}

	//myExporter()


	//WSACleanup();
	return status;
}



MMatrix BuildTransform(const MObject& obj)
{
	MFnTransform fn(obj);

	// get matrix transform for this bone
	MMatrix M = fn.transformationMatrix();

	if (fn.parentCount() > 0) {
		return M* BuildTransform(fn.parent(0));
	}
	return M;
}