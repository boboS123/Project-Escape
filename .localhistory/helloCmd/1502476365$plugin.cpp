#include "plugin.h"
#include "maya_precomp.h"
#include "LZ4/lz4.h"
#include "LZ4/lz4.c"



__int64 FileSize(const wchar_t *name)
{
	WIN32_FILE_ATTRIBUTE_DATA fad;
	if (!GetFileAttributesEx(name, GetFileExInfoStandard, &fad))
		return -1; // error condition, could call GetLastError to find out more
	LARGE_INTEGER size;
	size.HighPart = fad.nFileSizeHigh;
	size.LowPart = fad.nFileSizeLow;
	return size.QuadPart;
}



bool bobCATexporter::ExtractMeshDataToBuffer(MFnMesh &mesh, MESHLAYOUT layout, DWORD &size,
	MPointArray &points, 
	MFloatVectorArray normals,
	MFloatVectorArray tangents,
	MFloatVectorArray binormals, 
	MFloatArray u_coords,
	MFloatArray v_coords, 
	
	char **data, int &numVerts, VERTEX_FORMAT vertexFormat, int bitFieldVertexFormat) {




	MItMeshPolygon itPoly(mesh.object());
	unsigned int vcount = itPoly.polygonVertexCount();
	DWORD sizeOfBufCode = 0;
	int float_elements = 0;



	if ((bitFieldVertexFormat&vertexFormat_pos4) == vertexFormat_pos4) {
			float_elements += 4;
	}

	if ((bitFieldVertexFormat&vertexFormat_tex2) == vertexFormat_tex2) {
			float_elements += 2;
	}

	if ((bitFieldVertexFormat&vertexFormat_nrm3) == vertexFormat_nrm3) {
			float_elements += 3;
	}

	if ((bitFieldVertexFormat&vertexFormat_tgn3) == vertexFormat_tgn3) {
		float_elements += 3;
	}

	if ((bitFieldVertexFormat&vertexFormat_bin3) == vertexFormat_bin3) {
		float_elements += 3;
	}





	sizeOfBufCode = vcount * (sizeof(float) * float_elements) * mesh.numPolygons();
	cerr << "Float elements are: " << float_elements << endl;
	size = sizeOfBufCode;
	char *BufCode;
	BufCode = new char[sizeOfBufCode];
	cerr << "buffer size = " << sizeOfBufCode << endl << endl;

	int j = 0;
	int internalCounter = 0;


	
	while (!itPoly.isDone())
	{

		for (int i = 0; i != vcount; ++i, j++)
		{
			int float_jumper = 0;
			internalCounter++;


			size_t ptr = (float_elements * sizeof(float))*j;
			int index = itPoly.vertexIndex(i);

			int uv_index;
			itPoly.getUVIndex(i, uv_index);
			int normal_index = itPoly.normalIndex(i);
			int tangent_index = itPoly.tangentIndex(i);
			






			cerr << "Point:" << internalCounter << endl;



			if ((bitFieldVertexFormat&vertexFormat_pos4) == vertexFormat_pos4) {

				*((float*)(BufCode + (sizeof(float)*(float_jumper++)) + ptr)) = float(points[index].x);
				*((float*)(BufCode + (sizeof(float)*(float_jumper++)) + ptr)) = float(points[index].y);
				*((float*)(BufCode + (sizeof(float)*(float_jumper++)) + ptr)) = float(points[index].z) * -1.0f;
				*((float*)(BufCode + (sizeof(float)*(float_jumper++)) + ptr)) = 1.0f;
				cerr << "POINT: "  << float(points[index].x) << ", "<< float(points[index].y) << ", " << float(points[index].z)<< " ";
			//	MessageBox(NULL, L"normalized", L"message", NULL);
			}


			if ((bitFieldVertexFormat&vertexFormat_tex2) == vertexFormat_tex2) {
					float u = u_coords[uv_index];
					float v = (1.0f - v_coords[uv_index]);
					*((float*)(BufCode + (sizeof(float) * (float_jumper++)) + ptr)) = u;
					*((float*)(BufCode + (sizeof(float) * (float_jumper++)) + ptr)) = v;
					cerr << "TEXTURE COORD: " << u << ", " << v<< " ";
			}





			if ((bitFieldVertexFormat&vertexFormat_nrm3) == vertexFormat_nrm3) {

					float n3 = normals[normal_index].z * -1.0f;
					*(float*)(BufCode + (sizeof(float) * (float_jumper++)) + ptr) = normals[normal_index].x;
					*(float*)(BufCode + (sizeof(float) * (float_jumper++)) + ptr) = normals[normal_index].y;
					*(float*)(BufCode + (sizeof(float) * (float_jumper++)) + ptr) = n3;
					
					cerr << "NORMALS: " << float(normals[normal_index].x) << ", " << float(normals[normal_index].y) << ", " << float(normals[normal_index].z) << " ";
				//	MessageBox(NULL, L"normalized", L"message", NULL);
			}


			if ((bitFieldVertexFormat&vertexFormat_tgn3) == vertexFormat_tgn3) {


					*(float*)(BufCode + (sizeof(float) * (float_jumper++)) + ptr) = tangents[tangent_index].x;
					*(float*)(BufCode + (sizeof(float) * (float_jumper++)) + ptr) = tangents[tangent_index].y;
					*(float*)(BufCode + (sizeof(float) * (float_jumper++)) + ptr) = tangents[tangent_index].z;
					cerr << "Tangents" << endl;
			}



			if ((bitFieldVertexFormat&vertexFormat_bin3) == vertexFormat_bin3) {


				*(float*)(BufCode + (sizeof(float) * (float_jumper++)) + ptr) = binormals[tangent_index].x;
				*(float*)(BufCode + (sizeof(float) * (float_jumper++)) + ptr) = binormals[tangent_index].y;
				*(float*)(BufCode + (sizeof(float) * (float_jumper++)) + ptr) = binormals[tangent_index].z;
				cerr << "Tangents" << endl;
			}




			cerr << endl << endl << endl;




		}
		itPoly.next();
	}
	numVerts = internalCounter;
	cerr << "Internal Counter" << internalCounter << endl;
	//return BufCode;
	*data = BufCode;

	return 1;
}





int bobCATexporter::GetSurfaceShaderbyID(XVector<MaterialInfo>& materialDatabase, MFnMesh &mesh) {



	if (!mesh.isIntermediateObject())
	{


		for (int k = 0; k < mesh.parentCount(); ++k)
		{


			MIntArray    FaceIndices;
			MObjectArray Shaders;
			mesh.getConnectedShaders(k, Shaders, FaceIndices);


			int kk = 0;

			// for each surface shader
			for (kk = 0; kk != Shaders.length(); ++kk)
			{

				MFnDependencyNode fnShader(Shaders[kk]);


				MPlug sshader = fnShader.findPlug("surfaceShader");
				MPlugArray materials;
				sshader.connectedTo(materials, true, true);
				//




				for (unsigned int l = 0; l < materials.length(); ++l)
				{
					MFnDependencyNode fnMat(materials[l].node());
					cerr << "MeshMaterial:" << fnMat.name() << "At k=" << k << endl;
					MaterialInfo mat_nfo;
					mat_nfo.name = fnMat.name().asChar();
				//	int found = MaterialHeader.Find(mat_nfo, 0, MaterialHeader.count);
					cerr << "mESH PARENT COUNT IS:" << mesh.parentCount() << endl;
					cerr << "Shader is " << fnMat.name().asChar() << endl;

				}




			}
		}
		return -1;
	}
	return 1;
}

int bobCATexporter::GetNumMeshes() {

	int numMeshes = 0;
	MItDependencyNodes it(MFn::kMesh);
	while (!it.isDone())
	{

		MFnMesh fn(it.item());
		numMeshes++;
		it.next();
	}

	return numMeshes;



}


bobCATexporter::~bobCATexporter()
{


//	CleanupBeforeExport();
	
	XTextures->CloseFile();
	XStaticMesh->CloseFile();
	XMaterials->CloseFile();
	Instances->CloseFile();






}

void bobCATexporter::CleanupBeforeExport()
{



	meshes.Destroy();
	instances.Destroy();
	TextureHeader.Destroy();
	MaterialHeader.Destroy();




}

boundingBOX bobCATexporter::GetAABB(MPointArray &points)
{

	boundingBOX returnedAABB = { FLT_MAX ,FLT_MAX };
	
	returnedAABB.Min = XMVectorSetX(returnedAABB.Min, FLT_MAX);
	returnedAABB.Min = XMVectorSetY(returnedAABB.Min, FLT_MAX);
	returnedAABB.Min = XMVectorSetZ(returnedAABB.Min, FLT_MAX);


	returnedAABB.Max = XMVectorSetX(returnedAABB.Max, -FLT_MAX);
	returnedAABB.Max = XMVectorSetY(returnedAABB.Max, -FLT_MAX);
	returnedAABB.Max = XMVectorSetZ(returnedAABB.Max, -FLT_MAX);




	for (unsigned int i = 0; i < points.length(); i++) {

		returnedAABB.Min = XMVectorSetX(returnedAABB.Min, min(XMVectorGetX(returnedAABB.Min), float(points[i].x)));
		returnedAABB.Min = XMVectorSetY(returnedAABB.Min, min(XMVectorGetY(returnedAABB.Min), float(points[i].y)));
		returnedAABB.Min = XMVectorSetZ(returnedAABB.Min, min(XMVectorGetZ(returnedAABB.Min), float(points[i].z)));

		returnedAABB.Max = XMVectorSetX(returnedAABB.Max, max(XMVectorGetX(returnedAABB.Max), float(points[i].x)));
		returnedAABB.Max = XMVectorSetY(returnedAABB.Max, max(XMVectorGetY(returnedAABB.Max), float(points[i].y)));
		returnedAABB.Max = XMVectorSetZ(returnedAABB.Max, max(XMVectorGetZ(returnedAABB.Max), float(points[i].z)));




	}
	XMVectorSetZ(returnedAABB.Max, XMVectorGetZ(returnedAABB.Max)*-1.0f);
	XMVectorSetZ(returnedAABB.Min, XMVectorGetZ(returnedAABB.Min)*-1.0f);


	return returnedAABB;
}

bool bobCATexporter::ExportTextures()
{
	/*
	The header will contain filetextures name, size and location;
	Data is stored in a Balanced Binary Tree;
	Any object that needs textures need to load the tree and lookup the textures in the file




	*/


	XTextures->OpenFile();
	int numFiles = 0;
	DWORD FTrack = 0;


	MItDependencyNodes it2(MFn::kFileTexture);
	while (!it2.isDone())
	{
		numFiles++;
		it2.next();

	}





	cerr << "----------------------" << numFiles << " Textures are in the scene" << endl;
	// an array to hold all of the texture nodes
	MObjectArray Files;

	//The header is the number of textures
	XTextures->Write(sizeof(int), &numFiles);

	for (int i = 0; i < numFiles; i++) {

		TextureInfo temp;;

		XTextures->Write(sizeof(int), &temp.id);
		XTextures->Write(sizeof(DWORD), &temp.adress);
		XTextures->Write(sizeof(DWORD), &temp.size);
	}



	int imp = 0;
	MItDependencyNodes it(MFn::kFileTexture);
	while (!it.isDone())
	{

		TextureInfo t_info;

		// attach a dependency node to the file node
		MFnDependencyNode fn(it.item());

		cerr << "Item is " << fn.name() << endl;
		t_info.name = fn.name().asChar();
		t_info.id = imp;


		// append the texture to a list of files
		Files.append(it.item());

		// get the attribute for the full texture path
		MPlug ftn = fn.findPlug("ftn");

		// get the filename from the attribute
		MString filename;
		ftn.getValue(filename);

		// write the file name

		cerr << filename.asChar() << endl;



		//MPlugArray connectedTo;
		//fn.getConnections(connectedTo);
		//for (int i = 0; i < connectedTo.length(); i++) {

		MString raw;

		//	cerr << "Plugs are " << connectedTo[i].info() << endl;
		//	cerr << "Children are" << connectedTo[i].numChildren() << endl;
		//DisplayPlug(connectedTo[i]);

		// 			for (int j = 0; j < connectedTo[i].numChildren(); j++) {
		// 				cerr << "Children ["<<j<<"] "<< connectedTo[i].child(j).info() << endl;
		// 
		// 
		// 			}
		// 			connectedTo[i].child(0);
		//	}



		HANDLE temp_file = CreateFile(filename.asWChar(),
			GENERIC_READ,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		DWORD dwBytesWritten;
		int file_size = FileSize(filename.asWChar());

		if (file_size == -1) {
			MessageBox(NULL, L"Error reading the file", L"error", NULL);
		}
		cerr << "Debug File size" << file_size << endl;

		//Create a buffer that stores the whole file;
		char *ftemp;
		ftemp = new char[file_size];

		//Read the file in the temporary buffer
		ReadFile(temp_file, &ftemp[0], file_size, &dwBytesWritten, NULL);


		//Write the file in the texture pack
		t_info.size = file_size;
		//XTextures->Write(sizeof(int), &file_size);
		


		/* The exact texture data adress*/
		t_info.adress = XTextures->GetWrittenBytes();


		XTextures->Write(file_size, ftemp);
		//	cerr << "Whole file size" << dwBytesWritten << endl;
		// get next transform
		TextureHeader.Insert(t_info);
		cerr << endl;
		imp++;
		it.next();
		CloseHandle(temp_file);
	}


	//TextureHeader.SortVector();

	XTextures->SetPtr(sizeof(int));

	for (int i = 0; i < numFiles; i++)
	{
		cerr << "Texture_Name:" << TextureHeader[i].name << endl;
		cerr << "ID=" << TextureHeader[i].id << endl;
		cerr << "Address=" << TextureHeader[i].adress << endl;
		cerr << "Size" << TextureHeader[i].size << endl;
		cerr << endl;
		XTextures->Write(sizeof(int), &TextureHeader[i].id);
		XTextures->Write(sizeof(DWORD), &TextureHeader[i].adress);
		XTextures->Write(sizeof(DWORD), &TextureHeader[i].size);

	}



	XTextures->CloseFile();
	return 1;
}

int bobCATexporter::ExportAllMeshes(bool compressed, XVector<MeshInfo> &sceneInfo, int vertexFormat) {


	int numMeshes = GetNumMeshes();

	XStaticMesh->OpenFile();
	XStaticMesh->Write(sizeof(int), &numMeshes);


	//Write the header 
	for (int i = 0; i < numMeshes; i++) {
		MeshInfo meshtemp;
		meshtemp.id = 0;
		meshtemp.adress = 0;
		meshtemp.size = 0;
		meshtemp.compressed_size = 0;
		meshtemp.numVerts = 0;

		XStaticMesh->Write(sizeof(int), &meshtemp.id);

		//Write AABB
		for (int ib = 0; ib < 6; ib++) {
			float temp = 0;
			XStaticMesh->Write(sizeof(float), &temp);

		}





		XStaticMesh->Write(sizeof(int), &meshtemp.numVerts);
		XStaticMesh->Write(sizeof(DWORD), &meshtemp.adress);
		XStaticMesh->Write(sizeof(DWORD), &meshtemp.size);
		XStaticMesh->Write(sizeof(DWORD), &meshtemp.compressed_size);
		XStaticMesh->Write(sizeof(DWORD), &vertexFormat);
	}



	//Iterate the scene to grab all mesh data and export it to File
	MItDependencyNodes it(MFn::kMesh);
	int id = 0;

	while (!it.isDone())
	{
		MeshInfo meshtemp;
		MFnMesh fn(it.item());
		meshtemp.name = fn.name().asChar();
		cerr << "Mesh Name :" << meshtemp.name << endl;
		//Store temporary buffers with the data from the scene before exporting to the file
		MPointArray points;
		MFloatVectorArray normals;
		MFloatVectorArray tangents;
		MFloatVectorArray binormals;
		MFloatArray u_coords, v_coords;


		//Make sure the mesh is not a history object
		//	if (!fn.isIntermediateObject())
		//	{




		//Fill the buffers with data from the mesh
		fn.getPoints(points);
		fn.getNormals(normals);
		fn.getTangents(tangents);
		fn.getUVs(u_coords, v_coords);
		fn.getBinormals(binormals);


		//	}

		//Store the id of the mesh in the order we find them
		meshtemp.id = id++;



		//size of the uncompressed mesh
		DWORD size = 0;

		//A buffer for the uncompressed mesh data
		char *data;

		//Number of vertices in the mesh
		int numVerts = 0;
	

		vertexFormat = vertexFormat | vertexFormat_pos4;
		vertexFormat = vertexFormat | vertexFormat_tex2;
		vertexFormat = vertexFormat | vertexFormat_nrm3;
		vertexFormat = vertexFormat | vertexFormat_tgn3;
		vertexFormat = vertexFormat | vertexFormat_bin3;

		//Fill the size, data and numVerts in a continuous buffer that we can save to the file in one Write
		ExtractMeshDataToBuffer(fn, POS4COLOR4, size, points, normals, tangents,binormals, u_coords, v_coords, &data, numVerts, POS4_TEX2_NORM3, vertexFormat);

		MBoundingBox bounding1 = fn.boundingBox();

		meshtemp.maxAABB.x = (float)bounding1.max().x;
		meshtemp.maxAABB.y = (float)bounding1.max().y;
		meshtemp.maxAABB.z = (float)bounding1.max().z;



		meshtemp.minAABB.x = (float)bounding1.min().x;
		meshtemp.minAABB.y = (float)bounding1.min().y;
		meshtemp.minAABB.z = (float)bounding1.min().z;




		//meshtemp.maxAABB.x
		meshtemp.numVerts = numVerts;
		meshtemp.size = size;
		meshtemp.adress = XStaticMesh->GetWrittenBytes();



		char* pchCompressed;
		if (compressed) {

			//Create an additional buffer to store the compressed data of the mesh
			// Since we don't know the compressed size we'll make it the size of the original mesh

			pchCompressed = new char[size];

			int nCompressedSize = LZ4_compress(data, pchCompressed, size);
			meshtemp.compressed_size = nCompressedSize;
			XStaticMesh->Write(nCompressedSize, pchCompressed);

		}


		else {

			//Write the Uncompressed version
			//XStaticMesh->Write(size, data);
		}

		for (int ip = 0; ip < 36; ip++) {

			unsigned long counter = ip * (sizeof(float) * 6);
			//
			
			
			
			




			cerr << "Data point:" << *((float*)(data + counter)) <<" ";
			cerr << "Data point:" << *((float*)(data + sizeof(float) + counter)) << " ";
			cerr << "Data point:" << *((float*)(data + (sizeof(float)) * 2 + counter)) << " ";
			cerr << "Data point:" << *((float*)(data + (sizeof(float) * 3) + counter)) << " ";


			cerr << "Data point:" << *((float*)(data + (sizeof(float) * 4) + counter)) << " ";
			cerr << "Data point:" << *((float*)(data + (sizeof(float) * 5) + counter)) << " ";
		}






		//Write the compressed buffer to file



		//Insert the mesh info in the header
		meshes.Insert(meshtemp);



		//Delete the buffers
		//If we had compressed buffer
		if (compressed) {
			delete pchCompressed;
		}
		delete data;



		//Go to the next element in the scene
		it.next();
	}
	cerr << "AABB" << endl;
	//Go back and update the header
	XStaticMesh->SetPtr(sizeof(int));
	for (int i = 0; i < numMeshes; i++) {


		XStaticMesh->Write(sizeof(int), (char*)&meshes[i].id);



		//Write AABB

		float temp = meshes[i].minAABB.x;
		cerr << temp << "   ";
		XStaticMesh->Write( sizeof(float), (char*)&temp);
		temp = meshes[i].minAABB.y;
		cerr << temp << "   ";
		XStaticMesh->Write( sizeof(float), (char*)&temp);
		temp = meshes[i].minAABB.z;
		cerr << temp << "   ";
		XStaticMesh->Write( sizeof(float), (char*)&temp);



		temp = meshes[i].maxAABB.x;
		cerr << temp << "   ";
		XStaticMesh->Write(sizeof(float),(char*)&temp);
		temp = meshes[i].maxAABB.y;
		cerr << temp << "   ";
		XStaticMesh->Write(sizeof(float),(char*)&temp);
		temp = meshes[i].maxAABB.z;
		cerr << temp << "   ";
		XStaticMesh->Write( sizeof(float),(char*)&temp);






		XStaticMesh->Write(sizeof(int), &meshes[i].numVerts);
		XStaticMesh->Write(sizeof(DWORD), &meshes[i].adress);
		XStaticMesh->Write(sizeof(DWORD), &meshes[i].size);
		XStaticMesh->Write(sizeof(DWORD), &meshes[i].compressed_size);
		XStaticMesh->Write(sizeof(int), &vertexFormat);

		cerr << "Compressed Size for mesh [" << i << "] is " << meshes[i].compressed_size << endl;
		cerr << "Mesh Adress [" << i << "] is " << meshes[i].adress << endl;

	}

	//XStaticMesh->SaveChunkToFile();
	XStaticMesh->CloseFile();

	return numMeshes;

}

int bobCATexporter::ExportInstances(bool compressed, XVector<InstanceInfo> &instanceVector)
{
	MItDependencyNodes it(MFn::kTransform);


	int num = 0;
	while (!it.isDone()) {

		int id = 0;
		InstanceInfo instance_temp;
		instance_temp.ID = id++;



		MFnTransform fn(it.item());
		//Iterate only through game objects
		if (isGameObject(fn)) {

			num++;
		}
		it.next();
	}

	Instances->OpenFile();
	Instances->Write(sizeof(int), &num);

	for (int i = 0; i < num; i++) {

		InstanceInfo instance_temp;

		instance_temp.ID = -1;
		instance_temp.materialID_LOD1 = -1;
		instance_temp.ObjectType = TREE;
		instance_temp.phisicsID_LOD1 = -1;
		instance_temp.meshID_LOD1 = -1;



		Instances->Write(sizeof(int), &instance_temp.ID);
		int test = -1;
		Instances->Write(sizeof(int), &test);

		Instances->Write(sizeof(int), &instance_temp.materialID_LOD1);
		//Instances->Write(sizeof(int), &instance_temp.ObjectType);
		//Instances->Write(sizeof(int), &instance_temp.phisicsID_LOD1);
		Instances->Write(sizeof(int), &instance_temp.meshID_LOD1);




		//Write the transformation matrix header
		float mtx = 0;
		for (int im = 0; im < 16; im++) {
			Instances->Write(sizeof(float), &mtx);
		}


	}


	int id = 0;
	MItDependencyNodes it2(MFn::kTransform);
	while (!it2.isDone()) {





		MFnTransform fn(it2.item());
		//Iterate only through game objects
		if (isGameObject(fn)) {


			InstanceInfo instance_temp;
			instance_temp.ID = id++;

			int c = getObjectType(fn);
			instance_temp.ObjectType = getObjectType(fn);

			cerr << "Transform name---:" << fn.name().asChar() << endl;
			instance_temp.name = fn.name().asChar();
			num++;


		
			mat.setToIdentity();
			mat = fn.transformationMatrix();

		
// 			XMMATRIX finalMatrix = XMMatrixIdentity();
// 			for (int im = 0; im != 4; ++im)
// 			{
// 				for (int jm = 0; jm != 4; ++jm)
// 				{
// 
// 					
// 					finalMatrix.r->m128_f32[0] = static_cast<float>(mat(im, jm));
// 				}
// 			}
			//instance_temp.transformation = finalMatrix;
			
			//instance_temp.transformation
			mat(3,1) *= -1.0f;
			mat(3,2) *= -1.0f;
			mat(3,4) *= -1.0f;
			mat(1,3) *= -1.0f;
			mat(2,3) *= -1.0f;
			mat(4,3) *= -1.0f;


			mat(4,1) = (float)mat.matrix[3][0];
			mat(4,2) = (float)mat.matrix[3][1];
			mat(4,3) = (float)mat.matrix[3][2] * -1.0f;








			//Iterate the children of transform to search for a mesh
			int numChildren = fn.childCount();
			for (int i = 0; i < numChildren; i++) {
				if (fn.child(i).apiType() == MFn::kMesh) {
					MFnMesh fn3(fn.child(i));


					if (!fn3.isIntermediateObject())
					{

						MPointArray points;
						fn3.getPoints(points);
						boundingBOX box = GetAABB(points);
						instance_temp.AABB[0].x = XMVectorGetX(box.Min);
						instance_temp.AABB[0].y = XMVectorGetY(box.Min);
						instance_temp.AABB[0].z = XMVectorGetZ(box.Min);

						instance_temp.AABB[1].x = XMVectorGetX(box.Max);
						instance_temp.AABB[1].y = XMVectorGetY(box.Max);
						instance_temp.AABB[1].z = XMVectorGetZ(box.Max);



					}



					//Iterate the mesh instances so we can find the one assigned to this transform and extract materials
					int instanceCount = fn3.parentCount();
					for (int i = 0; i < instanceCount; i++) {

						MFnTransform fntParent((fn3.parent(i)));

						if (fntParent.name().asChar() == fn.name().asChar()) {

							cerr << "Transform name" << fntParent.name().asChar() << endl;



							MIntArray    FaceIndices;
							MObjectArray Shaders;
							fn3.getConnectedShaders(i, Shaders, FaceIndices);


							int kk = 0;

							// for each surface shader
							for (kk = 0; kk != Shaders.length(); ++kk)
							{

								MFnDependencyNode fnShader(Shaders[kk]);


								MPlug sshader = fnShader.findPlug("surfaceShader");
								MPlugArray materials;
								sshader.connectedTo(materials, true, true);

								for (unsigned int l = 0; l < materials.length(); ++l)
								{
									MFnDependencyNode fnMat(materials[l].node());
									cerr << "Shader is " << fnMat.name().asChar() << endl;

									MaterialInfo mat_nfo;
									mat_nfo.name = fnMat.name().asChar();
									int found = MaterialHeader.Find(mat_nfo, 0, MaterialHeader.count);
									instance_temp.materialID_LOD1 = found;
									cerr << "ID material is:" << found << endl;
									//cerr << endl;


								}

							}


						}

						//	cerr << "Transform name" << fnt.name().asChar() << endl;
						//	cerr << "Original mesh is" << fn.name().asChar() << endl;

					}





					cerr << "The mesh for the transform is" << fn3.name().asChar() << endl;
					MeshInfo tempmesh;
					tempmesh.name = fn3.name().asChar();
					instance_temp.meshID_LOD1 = meshes.Find(tempmesh, 0, meshes.count);
					cerr << "Mesh ID is " << instance_temp.meshID_LOD1 << endl;
					cerr << "Is game object:" << isGameObject(fn) << endl;
					cerr << endl << endl;

				}
			}
			instanceVector.Insert(instance_temp);
		}

		it2.next();
	}




	Instances->SetPtr(sizeof(int));

	for (int i = 0; i < instanceVector.count; i++) {


		Instances->Write(sizeof(int), &instanceVector[i].ID);

		int object_type = instanceVector[i].ObjectType;
		Instances->Write(sizeof(int), &object_type);
		cerr << "Write Object Type" << object_type;

		Instances->Write(sizeof(int), &instanceVector[i].materialID_LOD1);
		Instances->Write(sizeof(int), &instanceVector[i].meshID_LOD1);

		XMMATRIX test_matrix = instanceVector[i].transformation;

		//Write the transformation matrix header
		float mtx = 0;
		for (int im = 0; im < 4; im++) {
			for (int jm = 0; jm < 4; jm++) {
				float temp = (float) mat(im, jm);
				Instances->Write(sizeof(float), &temp);
			}
		}




		Instances->Write(sizeof(float), &instanceVector[i].AABB[0].x);
		Instances->Write(sizeof(float), &instanceVector[i].AABB[0].y);
		Instances->Write(sizeof(float), &instanceVector[i].AABB[0].z);

		Instances->Write(sizeof(float), &instanceVector[i].AABB[1].x);
		Instances->Write(sizeof(float), &instanceVector[i].AABB[1].y);
		Instances->Write(sizeof(float), &instanceVector[i].AABB[1].z);










	}



	Instances->CloseFile();
	return num;

}


bool bobCATexporter::ExportMaterials_v2() {

	/*



	*/
	int mat_number = 0;
	MItDependencyNodes itl2(MFn::kPhong);
	//Iterate and find Phong materials
	while (!itl2.isDone()) {

		if (itl2.item().apiType() == MFn::kPhong)
		{
			mat_number++;
		}
		itl2.next();
	}




	//Write the header
	XMaterials->OpenFile();
	XMaterials->Write(sizeof(int), &mat_number);


	for (int i = 0; i < mat_number; i++) {


		//Initial header has the elements to 0;
		MaterialInfo m_temp;
		m_temp.texture_id_color = 0;
		m_temp.texture_id_normal = 0;
		m_temp.texture_id_specular = 0;
		m_temp.id = 0;

		XMaterials->Write(sizeof(int), &m_temp.id);
		XMaterials->Write(sizeof(int), &m_temp.texture_id_color);
		XMaterials->Write(sizeof(int), &m_temp.texture_id_normal);
		XMaterials->Write(sizeof(int), &m_temp.texture_id_specular);


	}



	cerr << "Phong Material Number is " << mat_number << endl;


	int id = 0;
	MItDependencyNodes itl(MFn::kPhong);
	//Iterate and find Phong materials
	while (!itl.isDone()) {


		if (itl.item().apiType() == MFn::kPhong)
		{


			MFnPhongShader fnPhong(itl.item());
			MaterialInfo m_info;
			m_info.name = fnPhong.name().asChar();
			m_info.id = id;



			/*

			COLOR MAP

			*/


			MPlug p;
			p = fnPhong.findPlug("color");
			MPlugArray plugs;
			p.connectedTo(plugs, true, true);
			m_info.texture_id_color = -1;

			// see if any file textures are present
			for (int i = 0; i < plugs.length(); i++)
			{

				if (plugs[i].node().apiType() == MFn::kFileTexture)
				{



					// bind a function set to it ....
					MFnDependencyNode fnDep(plugs[i].node());


					float repeat_u;
					float repeat_v;

					MPlug plg = fnDep.findPlug("reu");
					plg.getValue(repeat_u);;
					plg = fnDep.findPlug("rev");
					plg.getValue(repeat_v);



					// to get the node name
					//texname = fnDep.name();
					cerr << "I'm searching for " << m_info.name << endl;
					cerr << "Texture is " << fnDep.name() << endl;
					TextureInfo text_temp;
					text_temp.name = fnDep.name().asChar();


					cerr << "D(" << text_temp.name << ") =(" << m_info.name << ")" << endl;
					m_info.texture_id_color = TextureHeader[TextureHeader.Find(text_temp, 0, TextureHeader.count)].id;
					cerr << "MATERIAL TEXTURE ID: " << m_info.texture_id_color << endl;


					cerr << endl;
				}





			}

			/*

			NORMAL MAP

			*/

			m_info.texture_id_normal = -1;
			MPlug c;
			c = fnPhong.findPlug("normalCamera");
			c.connectedTo(plugs, true, true);

			for (unsigned int i = 0; i < plugs.length(); ++i)
			{
				cerr << "Connected shader Bump loop" << endl;

				if (plugs[i].node().apiType() == MFn::kBump)
				{



					cerr << "Material has Normal Map" << endl;

					// attach a function set to the 2d bump node
					MFnDependencyNode fnBump(plugs[i].node());

					float bump_depth;

					// get the bump depth value from the node
					MPlug bumpDepth = fnBump.findPlug("bumpDepth");
					bumpDepth.getValue(bump_depth);
					cerr << "Bump Depth" << endl;

					// we now have the fun and joy of actually finding 
					// the file node that is connected to the bump map 
					// node itself. This is going to involve checking
					// the attribute connections to the bumpValue attribute.
					MPlug bump_value = fnBump.findPlug("bumpValue");
					MPlugArray bv_connections;

					bump_value.connectedTo(bv_connections, true, false);
					for (unsigned int j = 0; j < bv_connections.length(); ++j)
					{


						if (bv_connections[i].node().apiType() == MFn::kFileTexture)
						{
							MFnDependencyNode fnTex(bv_connections[i].node());

							MPlug ftn = fnTex.findPlug("ftn");

							// get the filename from the attribute
							MString filename;
							ftn.getValue(filename);


							cerr << "I'm searching for " << fnTex.name() << endl;
							//	cerr << "Normal File is " << filename << endl;


							TextureInfo text_temp;
							text_temp.name = fnTex.name().asChar();
							// 
							// 

							m_info.texture_id_normal = TextureHeader[TextureHeader.Find(text_temp, 0, TextureHeader.count)].id;
							cerr << "MATERIAL NORMAL ID: " << m_info.texture_id_normal << endl;
							// 

							cerr << endl;
						}

					}

				}


			}




			MPlug spec;
			spec = fnPhong.findPlug("specularColor");
			spec.connectedTo(plugs, true, true);


			m_info.texture_id_specular = -1;
			for (int i = 0; i < plugs.length(); i++)
			{


				// if file texture found
				if (plugs[i].node().apiType() == MFn::kFileTexture)
				{

					int spec = 1;
					//XMaterials->Write(sizeof(int), &spec);



					MFnDependencyNode fnTex(plugs[i].node());


					MPlug ftn = fnTex.findPlug("ftn");

					// get the filename from the attribute
					cerr << "SPECULAR ON" << endl;
					cerr << "Specular is " << fnTex.name() << endl;;
					TextureInfo text_temp;
					text_temp.name = fnTex.name().asChar();
					m_info.texture_id_specular = TextureHeader[TextureHeader.Find(text_temp, 0, TextureHeader.count)].id;
					cerr << "ID is:" << m_info.texture_id_specular << endl;

				}


			}



			MaterialHeader.Insert(m_info);

		}

		id++;
		itl.next();
	}

	XMaterials->SetPtr(sizeof(int));

	for (int i = 0; i < mat_number; i++) {


		XMaterials->Write(sizeof(int), &MaterialHeader[i].id);
		XMaterials->Write(sizeof(int), &MaterialHeader[i].texture_id_color);
		XMaterials->Write(sizeof(int), &MaterialHeader[i].texture_id_normal);
		XMaterials->Write(sizeof(int), &MaterialHeader[i].texture_id_specular);


	}

	XMaterials->CloseFile();
	return 1;
}

















bool bobCATexporter::isGameObject(MFnTransform &instance)
{
	MPlug ftn = instance.findPlug("gameObject");
	return ftn.asBool();
}

SceneObjectType bobCATexporter::getObjectType(MFnTransform &instance)
{
	MPlug ftn = instance.findPlug("objectType");
	int gm = ftn.asInt();
	return  SceneObjectType(ftn.asInt());
}
