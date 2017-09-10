#include "stdafx.h"
#include "XFile2.h"


bool XFile2::FileExists() {

	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(fileName, &FindFileData);
	int found = handle != INVALID_HANDLE_VALUE;

	if (found)
	{
		//FindClose(&handle); this will crash
		FindClose(handle);
	}
	return found;

}

void XFile2::SetPtr(DWORD pos) {
	SetFilePointer(handle, pos, 0, FILE_BEGIN);

}


bool XFile2::isFileOpen()
{
	return FileIsOpen;
}

bool XFile2::Write(DWORD size, void *data) {


	DWORD dwBytesWritten;
	WriteFile(handle, data, size, &dwBytesWritten, NULL);

	if (dwBytesWritten != 0) {
			writtenBytesTotal += dwBytesWritten;
		return 1;
	}
	else {
//		MessageBox(NULL, L"Write To File failed", file_name, NULL);
		//exit(0);
		return 0;
	}

}

XFile2::XFile2(LPCWSTR name, int access_mode, int creation_mode) : fileName(name), error(0), internalChunkBuffer(NULL) {


	creationmode = creation_mode;
	openmode = access_mode;
	internalPointer = 0;
	sizeLoaded = 0;
	LastAddress = 0;
	AutoLoadChunks = 0;
	AutoLoadChunkSize = 0;
	sizeLoadedFromStart = 0;
}


XFile2::~XFile2()
{

	//Close the File Handle
	CloseHandle(handle);

	//Check if we pass the chunk buffer to another object
	//If we didn't we must delete the data

	if (internalChunkBuffer != nullptr) {
		delete internalChunkBuffer;
		internalChunkBuffer = nullptr;
	}


}

DWORD XFile2::GetWrittenBytes()
{
	return writtenBytesTotal;

}

void XFile2::PromptError() {

	if (handle == INVALID_HANDLE_VALUE) {

		error = GetLastError();

		switch (error)  {

		case 5:
			MessageBox(NULL, L"Access is denied.", fileName, NULL);
			break;
		case 111:
			MessageBox(NULL, L"The file name is too long.", fileName, NULL);
			break;
		case 112:
			MessageBox(NULL, L"There is not enough space on the disk.", fileName, NULL);
			break;
		case 161:
			MessageBox(NULL, L"The specified path is invalid.", fileName, NULL);
			break;

		case 32:
			MessageBox(NULL, L"1Sharing Violation!!!", fileName, NULL);
			break;
		default:
			int a = error;
			wchar_t buffer[256];
			wsprintfW(buffer, L"%d", a);
			MessageBoxW(NULL, buffer, L"File Error", MB_OK);


		}

		CloseHandle(handle);


	}
}


__int64  XFile2::FileSize() {


	WIN32_FILE_ATTRIBUTE_DATA fad;
	if (!GetFileAttributesEx(fileName, GetFileExInfoStandard, &fad)) {

		MessageBox(NULL, L"GetFileSize error", fileName, NULL);
		PromptError();
		return -1; // error condition, could call GetLastError to find out more

	}

	LARGE_INTEGER size;
	size.HighPart = fad.nFileSizeHigh;
	size.LowPart = fad.nFileSizeLow;
	return size.QuadPart;


}




int XFile2::OpenFile() {



	if (creationmode == 4 || creationmode == 3) {

		if (!FileExists()) {

			//The File is not available 
			MessageBox(NULL, L"File does not exist!", fileName, NULL);

			return 0;
		}


		else {

			if (FileIsOpen == 0) {
				handle = CreateFile(fileName,
					openmode,
					0,
					NULL,
					creationmode,
					FILE_ATTRIBUTE_NORMAL,
					NULL);


				if (handle != INVALID_HANDLE_VALUE) {
					FileIsOpen = 1;
					return 1;
				}

				else {
					FileIsOpen = 0;
					PromptError();
				}


			}
			else {
				return 2;
			}


		}


	}


	else {


		if (FileExists()) {

			//MessageBox(NULL, L"File does exist!", file_name, NULL);

		}


		//Create a new file 

		handle = CreateFile(fileName,
			openmode,
			0,
			NULL,
			creationmode,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (handle != INVALID_HANDLE_VALUE) {

			FileIsOpen = 1;
			return 1;
		}
		else {
			FileIsOpen = 0;
			PromptError();
		}


	}

	return 1;

}



void XFile2::LoadChunk(DWORD startAdress, DWORD size){

	//Delete the old buffer
	if (internalChunkBuffer != NULL) {
		delete internalChunkBuffer;
		internalPointer = 0;
	}

	LastAddress = startAdress;
	SetPtr(startAdress);
	DWORD dwBytesWritten;
	sizeLoaded = size;
	internalChunkBuffer = new char[size];
	ReadFile(handle, &internalChunkBuffer[0], size, &dwBytesWritten, NULL);




}

char* XFile2::ReadFromChunk(DWORD size) {

	//if (!(internalPointer >= (LastAddress + sizeLoaded))) {

		internalPointer += size;
		return internalChunkBuffer + internalPointer-size;
		

	//}


	


}

char* XFile2::GetInternalBuffer() {

	//Instead of copying data all over if we don't do any processing we just give our created buffer away
	//We assign the adress of the buffer to another object so we don't have any control over it anymore.
	char *temp = internalChunkBuffer;
	internalChunkBuffer = nullptr;

	//return the adress
	return temp;

}



void XFile2::WriteToChunk(char *data, DWORD size)
{
	//
}

bool XFile2::CloseFile() {
	if (FileIsOpen) {
		return CloseHandle(handle);
	}
	


}