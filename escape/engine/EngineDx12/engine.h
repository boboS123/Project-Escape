#pragma once
#include "App/App.h"
#include "stdafx.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }
class __declspec(dllexport) engine :public App {

private:


	 const static int frameBufferCount = 3;


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
	HANDLE fenceEvent; // a handle to an event when our fence is unlocked by the gpu

	struct ConstantBufferPerObject {
		XMFLOAT4X4 wvpMat;
	};
	//ID3D12Resource* indexBuffer; // a default buffer in GPU memory that we will load index data for our triangle into
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	UINT8* cbvGPUAddress[frameBufferCount];
	int ConstantBufferPerObjectAlignedSize = (sizeof(ConstantBufferPerObject) + 255) & ~255;


	ConstantBufferPerObject cbPerObject;
	UINT64 fenceValue[frameBufferCount];
	//

	ID3D12Resource* vertexBuffer; // a default buffer in GPU memory that we will load vertex data for our triangle into
	ID3D12Resource* indexBuffer; // a default buffer in GPU memory that we will load index data for our triangle into

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

	XMFLOAT4X4 cube2WorldMat; // our first cubes world matrix (transformation matrix)
	XMFLOAT4X4 cube2RotMat; // this will keep track of our rotation for the second cube
	XMFLOAT4 cube2PositionOffset; // our second cube will rotate around the first cube, so this is the position offset from the first cube


	bool CreateCommandQueueDx12(ID3D12CommandQueue** commandQueue);
	bool CreateSwapChainDx12(HWND windowHwnd);
	bool CreateCommandListDx12(ID3D12GraphicsCommandList** cmdLst);
	bool CreateFence();
	bool CreateRootSignatureDx12();
	
	bool CompileShaders();
	void WaitForPreviousFrame();

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