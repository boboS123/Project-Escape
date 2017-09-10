#pragma once
#include "App/App.h"
#include "stdafx.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "XLibrary/XFile2.h"
#include "XLibrary/XVector.h"
#include "FileStructs.h"
#include "ResourceManager.h"
/*#include "DDSTextureLoader.h"*/

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
ResourceManager mgr;

Microsoft::WRL::ComPtr<ID3D12Resource> texture2 = nullptr;
Microsoft::WRL::ComPtr<ID3D12Resource> texture3 = nullptr;
Microsoft::WRL::ComPtr<ID3D12Resource> texture4 = nullptr;


Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap2 = nullptr;
Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap3 = nullptr;


Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
	if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;

	else return DXGI_FORMAT_UNKNOWN;
}

// get a dxgi compatible wic format from another wic format
WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
	if (wicFormatGUID == GUID_WICPixelFormatBlackWhite) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat1bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat2bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat4bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat2bppGray) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat4bppGray) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayFixedPoint) return GUID_WICPixelFormat16bppGrayHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFixedPoint) return GUID_WICPixelFormat32bppGrayFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR555) return GUID_WICPixelFormat16bppBGRA5551;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR101010) return GUID_WICPixelFormat32bppRGBA1010102;
	else if (wicFormatGUID == GUID_WICPixelFormat24bppBGR) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat24bppRGB) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppPBGRA) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppPRGBA) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGB) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppBGR) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPBGRA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppBGRFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppPRGBAFloat) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFloat) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBE) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppCMYK) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppCMYK) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat40bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat80bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGB) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGB) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBAHalf) return GUID_WICPixelFormat64bppRGBAHalf;
#endif

	else return GUID_WICPixelFormatDontCare;
}

// get the number of bits per pixel for a dxgi format
int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat)
{
	if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
	else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
	else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
	else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

	else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT) return 32;
	else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R16_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R8_UNORM) return 8;
	else if (dxgiFormat == DXGI_FORMAT_A8_UNORM) return 8;
}





int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename, int &bytesPerRow)
{
	HRESULT hr;




	// we only need one instance of the imaging factory to create decoders and frames
	static IWICImagingFactory *wicFactory;

	// reset decoder, frame and converter since these will be different for each image we load
	IWICBitmapDecoder *wicDecoder = NULL;
	IWICBitmapFrameDecode *wicFrame = NULL;
	IWICFormatConverter *wicConverter = NULL;

	bool imageConverted = false;

	if (wicFactory == NULL)
	{
		// Initialize the COM library
		CoInitialize(NULL);

		// create the WIC factory
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&wicFactory)
		);
		if (FAILED(hr)) return 0;
	}

	// load a decoder for the image

	hr = wicFactory->CreateDecoderFromFilename(
		filename,                        // Image we want to load in
		NULL,                            // This is a vendor ID, we do not prefer a specific one so set to null
		GENERIC_READ,                    // We want to read from this file
		WICDecodeMetadataCacheOnLoad,    // We will cache the metadata right away, rather than when needed, which might be unknown
		&wicDecoder                      // the wic decoder to be created
	);

	
	







	if (FAILED(hr)) return 0;

	// get image from decoder (this will decode the "frame")
	hr = wicDecoder->GetFrame(0, &wicFrame);
	if (FAILED(hr)) return 0;

	// get wic pixel format of image
	WICPixelFormatGUID pixelFormat;
	hr = wicFrame->GetPixelFormat(&pixelFormat);
	if (FAILED(hr)) return 0;

	// get size of image
	UINT textureWidth, textureHeight;
	hr = wicFrame->GetSize(&textureWidth, &textureHeight);
	if (FAILED(hr)) return 0;

	// we are not handling sRGB types in this tutorial, so if you need that support, you'll have to figure
	// out how to implement the support yourself

	// convert wic pixel format to dxgi pixel format
	DXGI_FORMAT dxgiFormat = GetDXGIFormatFromWICFormat(pixelFormat);

	// if the format of the image is not a supported dxgi format, try to convert it
	if (dxgiFormat == DXGI_FORMAT_UNKNOWN)
	{
		// get a dxgi compatible wic format from the current image format
		WICPixelFormatGUID convertToPixelFormat = GetConvertToWICFormat(pixelFormat);

		// return if no dxgi compatible format was found
		if (convertToPixelFormat == GUID_WICPixelFormatDontCare) return 0;

		// set the dxgi format
		dxgiFormat = GetDXGIFormatFromWICFormat(convertToPixelFormat);

		// create the format converter
		hr = wicFactory->CreateFormatConverter(&wicConverter);
		if (FAILED(hr)) return 0;

		// make sure we can convert to the dxgi compatible format
		BOOL canConvert = FALSE;
		hr = wicConverter->CanConvert(pixelFormat, convertToPixelFormat, &canConvert);
		if (FAILED(hr) || !canConvert) return 0;

		// do the conversion (wicConverter will contain the converted image)
		hr = wicConverter->Initialize(wicFrame, convertToPixelFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
		if (FAILED(hr)) return 0;

		// this is so we know to get the image data from the wicConverter (otherwise we will get from wicFrame)
		imageConverted = true;
	}

	int bitsPerPixel = GetDXGIFormatBitsPerPixel(dxgiFormat); // number of bits per pixel
	bytesPerRow = (textureWidth * bitsPerPixel) / 8; // number of bytes in each row of the image data
	int imageSize = bytesPerRow * textureHeight; // total image size in bytes

												 // allocate enough memory for the raw image data, and set imageData to point to that memory
	*imageData = (BYTE*)malloc(imageSize);

	// copy (decoded) raw image data into the newly allocated memory (imageData)
	if (imageConverted)
	{
		// if image format needed to be converted, the wic converter will contain the converted image
		hr = wicConverter->CopyPixels(0, bytesPerRow, imageSize, *imageData);
		if (FAILED(hr)) return 0;
	}
	else
	{
		// no need to convert, just copy data from the wic frame
		hr = wicFrame->CopyPixels(0, bytesPerRow, imageSize, *imageData);
		if (FAILED(hr)) return 0;
	}

	// now describe the texture with the information we have obtained from the image
	resourceDescription = {};
	resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDescription.Alignment = 0; // may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
	resourceDescription.Width = textureWidth; // width of the texture
	resourceDescription.Height = textureHeight; // height of the texture
	resourceDescription.DepthOrArraySize = 1; // if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
	resourceDescription.MipLevels = 1; // Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
	resourceDescription.Format = dxgiFormat; // This is the dxgi format of the image (format of the pixels)
	resourceDescription.SampleDesc.Count = 1; // This is the number of samples per pixel, we just want 1 sample
	resourceDescription.SampleDesc.Quality = 0; // The quality level of the samples. Higher is better quality, but worse performance
	resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
	resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE; // no flags


														  // return the size of the image. remember to delete the image once your done with it (in this tutorial once its uploaded to the gpu)
	return imageSize;
}




D3D12_GPU_DESCRIPTOR_HANDLE handle;
D3D12_GPU_DESCRIPTOR_HANDLE handle1;
D3D12_GPU_DESCRIPTOR_HANDLE handle2;















#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }
class __declspec(dllexport) engine :public App {

private:



	 const static int frameBufferCount = 3;

	 int numCubeIndices;
	 ID3D12Resource* textureBuffer;

	 ID3D12Resource* textureBuffer1;
	 ID3D12Resource* textureBuffer2;

	 ID3D12Resource* textureBuffer3;

	 D3D12_GPU_DESCRIPTOR_HANDLE GPUsrvHandle1;
	 D3D12_GPU_DESCRIPTOR_HANDLE GPUsrvHandle2;
	 D3D12_GPU_DESCRIPTOR_HANDLE GPUsrvHandle3;








	 UINT srvHandleSize1;
	 CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle1; // the font srv
	

	 UINT srvHandleSize2;
	 CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle2; // the font srv



	 UINT srvHandleSize3;
	 CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle3; // the font srv








	ID3D12Device* device; 
	IDXGISwapChain3* swapChain;



	ID3D12CommandQueue* commandQueue;
	ID3D12DescriptorHeap* rtvDescriptorHeap;
	ID3D12Resource* renderTargets[frameBufferCount]; 
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;


	ID3D12Resource* constantBufferUploadHeaps[frameBufferCount];
	ID3D12CommandAllocator* commandAllocator[frameBufferCount]; 
	IDXGIFactory4* dxgiFactory;
	int frameIndex;
	int rtvDescriptorSize;
	ID3DBlob* errorBuff; // a buffer holding the error data if any
	ID3DBlob* signature;
	DXGI_SAMPLE_DESC sampleDesc = {};
	ID3D12GraphicsCommandList* commandList;
	ID3D12DescriptorHeap* dsDescriptorHeap; // This is a heap for our depth/stencil buffer descriptor
	ID3D12DescriptorHeap* mainDescriptorHeap;

	ID3D12PipelineState* pipelineStateObject; // pso containing a pipeline state
	ID3D12PipelineState* textPSO; // pso containing a pipeline state
	D3D12_VIEWPORT viewport; // a
	D3D12_RECT scissorRect;

	//Fence
	ID3D12Fence* fence[frameBufferCount];
	ID3D12Fence* drawOne;



	HANDLE fenceEvent; // a handle to an event when our fence is unlocked by the gpu
	HANDLE drawOnefenceEvent;

	struct ConstantBufferPerObject {
		XMFLOAT4X4 wvpMat;
	};


	//ID3D12Resource* indexBuffer; // a default buffer in GPU memory that we will load index data for our triangle into
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	UINT8* cbvGPUAddress[frameBufferCount];
	int ConstantBufferPerObjectAlignedSize = (sizeof(ConstantBufferPerObject) + 255) & ~255;


	ConstantBufferPerObject cbPerObject;
	ConstantBufferPerObject cbPerObject2;
	UINT64 fenceValue[frameBufferCount];
	UINT64 drawOnefenceValue;

	//

	ID3D12Resource* vertexBuffer; // a default buffer in GPU memory that we will load vertex data for our triangle into
	ID3D12Resource* indexBuffer; // a default buffer in GPU memory that we will load index data for our triangle into


	//Second buffer
	ID3D12Resource* vertexBuffer2; // a default buffer in GPU memory that we will load vertex data for our triangle into
	ID3D12Resource* indexBuffer2; // a default buffer in GPU memory that we will load index data for our triangle into





	XFile2 *loadMesh;
	XVector<MeshHeader> header;



	struct MeshStruct {
		float x;
		float y;
		float z;
		float w;

		float u;
		float v;
	};


	MeshStruct meshS[36];





	ID3D12Resource* depthStencilBuffer; // This is the memory for our depth buffer. it will also be used for a stencil buffer in a later tutorial
	//ID3D12DescriptorHeap* dsDescriptorHeap; // This is a heap for our depth/stencil buffer descriptor



	ID3D12RootSignature* rootSignature; // root signature defines data shaders will access

	IDXGIAdapter1* adapter; // adapters are the graphics card (this includes the embedded graphics on the motherboard)
	int adapterIndex = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0
	bool adapterFound = false; // set this to true when a good one was found
	
	XMFLOAT4X4 cameraProjMat; // this will store our projection matrix
	XMFLOAT4X4 cameraViewMat; // this will store our view matrix

	XMFLOAT4 cameraPosition; // this is our cameras position vector
	XMFLOAT4 cameraTarget; // a vector describing the point in space our camera is looking at
	XMFLOAT4 cameraUp; // the worlds up vector

	XMFLOAT4X4 cube1WorldMat; // our first cubes world matrix (transformation matrix)
	XMFLOAT4X4 cube1RotMat; // this will keep track of our rotation for the first cube
	XMFLOAT4 cube1Position; // our first cubes position in space
	XMFLOAT4 cube2Position;


	XMFLOAT4X4 cube2WorldMat; // our first cubes world matrix (transformation matrix)
	XMFLOAT4X4 cube3WorldMat;
	XMFLOAT4X4 cube2RotMat; // this will keep track of our rotation for the second cube
	XMFLOAT4X4 cube3RotMat;
	XMFLOAT4 cube2PositionOffset; // our second cube will rotate around the first cube, so this is the position offset from the first cube
	XMFLOAT4 cube3PositionOffset; // our second cube will rotate around the first cube, so this is the position offset from the first cube


	int Width = 800;
	int Height = 600;




	//MAIN FUNCTIONS

	//bool LoadPriximityVertexObjects(float Proximity, XMFLOAT4X4 &camera);

	bool CreateCommandQueueDx12(ID3D12CommandQueue** commandQueue);
	bool CreateSwapChainDx12(HWND windowHwnd);
	bool CreateCommandListDx12(ID3D12GraphicsCommandList** cmdLst);
	bool CreateFence();
	bool CreateRootSignatureDx12();
	
	bool CompileShaders();
	void WaitForPreviousFrame();


	ID3D12Resource* CreateVertexBuffer();


	//Resource Loading
	//ResourceMag
















	//assuming we used another method to load the data
	CD3DX12_GPU_DESCRIPTOR_HANDLE CreateTexture(BYTE **data, ID3D12DescriptorHeap** descHeap, ID3D12Device** device, ID3D12Resource** textureBuffer, D3D12_RESOURCE_DESC *textDesc, int fontImageBytesPerRow,int Offset) {



		//D3D12_RESOURCE_DESC textureDescription;
		//int ImageBytesPerRow2;

		HRESULT hr = (*device)->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			textDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&(*textureBuffer)));


		if (FAILED(hr))
		{
			MessageBox(NULL, L"error creating image buffer444444444444444444", L"error", NULL);
			//Running = false;
		}


		(*textureBuffer)->SetName(L"Font Texture Buffer Resource Heap3J");




		ID3D12Resource* fontTextureBufferUploadHeap2;
		UINT64 fontTextureUploadBufferSize2;
		(*device)->GetCopyableFootprints(textDesc, 0, 1, 0, nullptr, nullptr, nullptr, &fontTextureUploadBufferSize2);

		// create an upload heap to copy the texture to the gpu
		hr = (*device)->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(fontTextureUploadBufferSize2),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&fontTextureBufferUploadHeap2));
		
		if (FAILED(hr))
		{
			MessageBox(NULL, L"error creating image buffer4444444444444436544444", L"error", NULL);
		}
		fontTextureBufferUploadHeap2->SetName(L"Font Texture Buffer2 Upload Resource Heap");

		// store font image in upload heap




		D3D12_SUBRESOURCE_DATA fontTextureData2 = {};
		fontTextureData2.pData = &(*data)[0]; // pointer to our image data
		fontTextureData2.RowPitch = fontImageBytesPerRow; // size of all our triangle vertex data
		fontTextureData2.SlicePitch = fontImageBytesPerRow * textDesc->Height; // also the size of our triangle vertex data

																					   // Now we copy the upload buffer contents to the default heap
		UpdateSubresources(commandList, *textureBuffer, fontTextureBufferUploadHeap2, 0, 0, 1, &fontTextureData2);

		// transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(*textureBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		// create an srv for the font
		D3D12_SHADER_RESOURCE_VIEW_DESC fontsrvDesc2t = {};
		fontsrvDesc2t.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		fontsrvDesc2t.Format = textDesc->Format;
		fontsrvDesc2t.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		fontsrvDesc2t.Texture2D.MipLevels = 1;

		// we need to get the next descriptor location in the descriptor heap to store this srv
	//	UINT srvHandleSize2 = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//srvHandle2 = CD3DX12_GPU_DESCRIPTOR_HANDLE(mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 2, srvHandleSize2);


		//	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle3(mainDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 1, srvHandleSize2);
		//device->CreateShaderResourceView(textureBuffer3, &fontsrvDesc2, srvHandle3);
		//srvHandle3.Offset(1, srvHandleSize2);


		UINT srvHandleSize3 = (*device)->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPUsrvHandle2X = CD3DX12_GPU_DESCRIPTOR_HANDLE((*descHeap)->GetGPUDescriptorHandleForHeapStart(), Offset, srvHandleSize3);
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandlegT((*descHeap)->GetCPUDescriptorHandleForHeapStart(), Offset, srvHandleSize3);
		(*device)->CreateShaderResourceView(*textureBuffer, &fontsrvDesc2t, srvHandlegT);


		return GPUsrvHandle2X;




	}








	public:


		void UpdatePipeline();
		void Render();


public:
	bool InitiateDx12API();
	

	void Cleanup()
	{
		// wait for the gpu to finish all frames
		for (int i = 0; i < frameBufferCount; ++i)
		{
			frameIndex = i;
			WaitForPreviousFrame();
		}

		// get swapchain out of full screen before exiting
		BOOL fs = false;
		if (swapChain->GetFullscreenState(&fs, NULL))
			swapChain->SetFullscreenState(false, NULL);

		SAFE_RELEASE(device);
		SAFE_RELEASE(swapChain);
		SAFE_RELEASE(commandQueue);
		SAFE_RELEASE(rtvDescriptorHeap);
		SAFE_RELEASE(commandList);

		for (int i = 0; i < frameBufferCount; ++i)
		{
			SAFE_RELEASE(renderTargets[i]);
			SAFE_RELEASE(commandAllocator[i]);
			SAFE_RELEASE(fence[i]);
		};

		SAFE_RELEASE(pipelineStateObject);
		SAFE_RELEASE(rootSignature);
		SAFE_RELEASE(vertexBuffer);
		SAFE_RELEASE(indexBuffer);

//		SAFE_RELEASE(depthStencilBuffer);
		SAFE_RELEASE(dsDescriptorHeap);

// 		for (int i = 0; i < frameBufferCount; ++i)
// 		{
// 			SAFE_RELEASE(constantBufferUploadHeaps[i]);
// 		};
	}












};