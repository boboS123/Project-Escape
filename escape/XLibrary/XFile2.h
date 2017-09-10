#ifndef XFILE2_H
#define XFILE2_H



class __declspec(dllexport) XFile2 {


private:




	bool FileIsOpen = 0;
	LPCWSTR fileName;
	HANDLE handle;

	int creationmode;
	int openmode;

	//Address of the allocated chunk from file
	char *internalChunkBuffer;
	DWORD internalPointer;



	DWORD LastAddress;
	DWORD sizeLoaded;
	DWORD sizeLoadedFromStart;
	DWORD writtenBytesTotal=0;






	bool FileExists();

	DWORD error;

	void PromptError();
	

	bool AutoLoadChunks;
	DWORD AutoLoadChunkSize;




public:


	XFile2(LPCWSTR name, int access_mode, int creation_mode);
	~XFile2();
	void SetPtr(DWORD pos);
	bool isFileOpen();

	DWORD GetWrittenBytes();
	int OpenFile();
	bool CloseFile();

	//Load from File to buffer
	void LoadChunk(DWORD startAdress, DWORD size);
	void LoadChunkAndUncompress(DWORD startAdress, DWORD size);
	//Equivalent of ReadFromFile
	char* ReadFromChunk(DWORD size);
	void FlushChunkBuffer();

	char*GetInternalBuffer();

	//Write to buffer
	void GenerateChunkForWriting(DWORD size);
	void WriteToChunk(char *data, DWORD size);
	void SaveChunkToFileAndCompress();
	void SaveChunkToFile();
	__int64 FileSize();


	bool Write(DWORD size, void *data);





};


#endif