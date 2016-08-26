#include "stdafx.h"
#include "engine.h"
#include <fstream>
using namespace std;
fstream f1;
bool engine::InitiateDx12API()
{

	f1.open(L"C:\\Workspace\\_bin\\Debug\\dbg_dx12.txt", std::ifstream::out);



	HRESULT hr;

	// -- Create the Device -- //

	
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr))
	{
		MessageBox(0, L"CreateDXGIFactory1  Failed.", 0, 0);
	}



	while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// we dont want a software device
			continue;
		}

		// we want a device that is compatible with direct3d 12 (feature level 11 or higher)
		hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr))
		{
			adapterFound = true;
			break;
		}

		adapterIndex++;
	}




	HRESULT hr1 = D3D12CreateDevice(
		adapter,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&device)
	);
	if (FAILED(hr))
	{
		MessageBox(0, L"DeviceDx12 Failed.", 0, 0);
		return 0;
	}
	else {
		MessageBox(0, L"DeviceDx12 Success.", 0, 0);
		//return 1;
	}





	CreateCommandQueueDx12(&commandQueue);
	CreateSwapChainDx12(mhMainWnd);
	CreateCommandListDx12(&commandList);
	CreateFence();
	CreateRootSignatureDx12();
	CompileShaders();



	return false;
}





bool engine::CreateCommandQueueDx12(ID3D12CommandQueue** commandQueue)
{


	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // direct means the gpu can directly execute this command queue

	HRESULT hr = device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&*commandQueue)); // create the command queue
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Command Queue failed", L"error", NULL);
		//Running = false;
		//return false;
	}
	else {
		MessageBox(NULL, L"Command Queue OK", L"error", NULL);
		//Running = false;
		//return false;
	}


	return 1;


}

bool engine::CreateSwapChainDx12(HWND windowHwnd)
{


	DXGI_MODE_DESC backBufferDesc = {}; // this is to describe our display mode
	backBufferDesc.Width = 299; // buffer width
	backBufferDesc.Height = 322; // buffer height
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)

														// describe our multi-sampling. We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
	
	sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

						  // Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = frameBufferCount; // number of buffers we have
	swapChainDesc.BufferDesc = backBufferDesc; // our back buffer description
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // this says the pipeline will render to this swap chain
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
	swapChainDesc.OutputWindow = windowHwnd; // handle to our window
	swapChainDesc.SampleDesc = sampleDesc; // our multi-sampling description
	swapChainDesc.Windowed = 1; // set to true, then if in fullscreen must call SetFullScreenState with true for full screen to get uncapped fps

	IDXGISwapChain* tempSwapChain;

	dxgiFactory->CreateSwapChain(
		commandQueue, // the queue will be flushed once the swap chain is created
		&swapChainDesc, // give it the swap chain description we created above
		&tempSwapChain // store the created swap chain in a temp IDXGISwapChain interface
	);

	swapChain = static_cast<IDXGISwapChain3*>(tempSwapChain);

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	// -- Create the Back Buffers (render target views) Descriptor Heap -- //





	// describe an rtv descriptor heap and create
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = frameBufferCount; // number of descriptors for this heap.
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // this heap is a render target view heap

													   // This heap will not be directly referenced by the shaders (not shader visible), as this will store the output from the pipeline
													   // otherwise we would set the heap's flag to D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed Descriptor Heap", L"failed", NULL);
		///Running = false;
		//return false;
	}

	// get the size of a descriptor in this heap (this is a rtv heap, so only rtv descriptors should be stored in it.
	// descriptor sizes may vary from device to device, which is why there is no set size and we must ask the 
	// device to give us the size. we will use this size to increment a descriptor handle offset
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// get a handle to the first descriptor in the descriptor heap. a handle is basically a pointer,
	// but we cannot literally use it like a c++ pointer.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each buffer (double buffering is two buffers, tripple buffering is 3).
	for (int i = 0; i < frameBufferCount; i++)
	{
		// first we get the n'th buffer in the swap chain and store it in the n'th
		// position of our ID3D12Resource array
		hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed GetBuffer", L"failed", NULL);
			//Running = false;
			//return false;
		}

		// the we "create" a render target view which binds the swap chain buffer (ID3D12Resource[n]) to the rtv handle
		device->CreateRenderTargetView(renderTargets[i], nullptr, rtvHandle);

		// we increment the rtv handle by the rtv descriptor size we got above
		rtvHandle.Offset(1, rtvDescriptorSize);
	}




	for (int i = 0; i < frameBufferCount; i++)
	{
		HRESULT hr2 = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator[i]));
		if (FAILED(hr2))
		{
			MessageBox(NULL, L"CommandAllocator Failed", L"failed", NULL);
			//return false;
		}
	}






	HRESULT hr3 = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator[frameIndex], NULL, IID_PPV_ARGS(&commandList));
	if (FAILED(hr))
	{

		MessageBox(NULL, L"CommandList Failed", L"failed", NULL);
		//Running = false;
		//Running = false;
		exit(0);
		//	return false;
	}

	// -- Create a Fence & Fence Event -- //






	return 1;



}

bool engine::CreateCommandListDx12(ID3D12GraphicsCommandList** cmdLst)
{










	return 1;

}

bool engine::CreateFence()
{



	// create the fences
	for (int i = 0; i < frameBufferCount; i++)
	{
		HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence[i]));
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Fence Failed", L"failed", NULL);
			return false;
		}
		fenceValue[i] = 0; // set the initial fence value to 0
	}

	// create a handle to a fence event
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr)
	{
		MessageBox(NULL, L"FenceEvent Failed", L"failed", NULL);
		return false;
	}

	else {
		MessageBox(NULL, L"FenceEvent Succeded", L"failed", NULL);
	}

}

bool engine::CreateRootSignatureDx12()
{

	// create root signature

	// create a root descriptor, which explains where to find the data for this root parameter
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
	rootCBVDescriptor.RegisterSpace = 0;
	rootCBVDescriptor.ShaderRegister = 0;

	// create a descriptor range (descriptor table) and fill it out
	// this is a range of descriptors inside a descriptor heap
	D3D12_DESCRIPTOR_RANGE  descriptorTableRanges[1]; // only one range right now
	descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // this is a range of shader resource views (descriptors)
	descriptorTableRanges[0].NumDescriptors = 1; // we only have one texture right now, so the range is only 1
	descriptorTableRanges[0].BaseShaderRegister = 0; // start index of the shader registers in the range
	descriptorTableRanges[0].RegisterSpace = 0; // space 0. can usually be zero
	descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables

																									   // create a descriptor table
	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges); // we only have one range
	descriptorTable.pDescriptorRanges = &descriptorTableRanges[0]; // the pointer to the beginning of our ranges array

																   // create a root parameter for the root descriptor and fill it out
	D3D12_ROOT_PARAMETER  rootParameters[2]; // two root parameters
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // this is a constant buffer view root descriptor
	rootParameters[0].Descriptor = rootCBVDescriptor; // this is the root descriptor for this root parameter
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // our pixel shader will be the only shader accessing this parameter for now

																		 // fill out the parameter for our descriptor table. Remember it's a good idea to sort parameters by frequency of change. Our constant
																		 // buffer will be changed multiple times per frame, while our descriptor table will not be changed at all (in this tutorial)
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // this is a descriptor table
	rootParameters[1].DescriptorTable = descriptorTable; // this is our descriptor table for this root parameter
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // our pixel shader will be the only shader accessing this parameter for now

																		// create a static sampler
	D3D12_STATIC_SAMPLER_DESC sampler = {};

	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameters), // we have 2 root parameters
		rootParameters, // a pointer to the beginning of our root parameters array
		1, // we have one static sampler
		&sampler, // a pointer to our static sampler (array)
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);


	HRESULT hrSer = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errorBuff);
	if (FAILED(hrSer))
	{
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		MessageBox(NULL, L"Serialize Root Failed", L"failed", NULL);
		//return false;
	}

	hrSer = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	if (FAILED(hrSer))
	{
		MessageBox(NULL, L"RotSig Failed", L"failed", NULL);
	}
	else {
		MessageBox(NULL, L"RotSig Succeded", L"failed", NULL);
	}



	return 1;

}

bool engine::CompileShaders()
{



	// compile vertex shader
	ID3DBlob* vertexShader; // d3d blob for holding vertex shader bytecode
	HRESULT hr = D3DCompileFromFile(L"VertexShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexShader,
		&errorBuff);
	if (FAILED(hr))
	{
//		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		MessageBox(NULL, L"Shader  Failed", L"failed", NULL);
		exit(0);
		return false;

	}
	else {
		MessageBox(NULL, L"Shader  Success", L"oky", NULL);
	}

	// fill out a shader bytecode structure, which is basically just a pointer
	// to the shader bytecode and the size of the shader bytecode
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = vertexShader->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = vertexShader->GetBufferPointer();

	// compile pixel shader
	ID3DBlob* pixelShader;
	hr = D3DCompileFromFile(L"PixelShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelShader,
		&errorBuff);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		MessageBox(NULL, L"Shader  Failed", L"failed", NULL);
		return false;
	}

	// fill out shader bytecode structure for pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = pixelShader->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = pixelShader->GetBufferPointer();

	// create input layout

	// The input layout is used by the Input Assembler so that it knows
	// how to read the vertex data bound to it.

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// fill out an input layout description structure
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

	// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout;

	// create a pipeline state object (PSO)

	// In a real application, you will have many pso's. for each different shader
	// or different combinations of shaders, different blend states or different rasterizer states,
	// different topology types (point, line, triangle, patch), or a different number
	// of render targets you will need a pso

	// VS is the only required shader for a pso. You might be wondering when a case would be where
	// you only set the VS. It's possible that you have a pso that only outputs data with the stream
	// output, and not on a render target, which means you would not need anything after the stream
	// output.

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; // a structure to define a pso
	psoDesc.InputLayout = inputLayoutDesc; // the structure describing our input layout
	psoDesc.pRootSignature = rootSignature; // the root signature that describes the input data this pso needs
	psoDesc.VS = vertexShaderBytecode; // structure describing where to find the vertex shader bytecode and how large it is
	psoDesc.PS = pixelShaderBytecode; // same as VS but for pixel shader
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	psoDesc.SampleDesc = sampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
	psoDesc.SampleMask = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // a default rasterizer state.
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	psoDesc.NumRenderTargets = 1; // we are only binding one render target
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state

																		   // create the pso
	hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineStateObject));
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Shader  Failed", L"failed", NULL);
		return false;
	}

	// Text PSO

	// compile vertex shader
	ID3DBlob* textVertexShader; // d3d blob for holding vertex shader bytecode
	hr = D3DCompileFromFile(L"TextVertexShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&textVertexShader,
		&errorBuff);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		MessageBox(NULL, L"Shader  Failed", L"failed", NULL);
		return false;
	}

	// fill out a shader bytecode structure, which is basically just a pointer
	// to the shader bytecode and the size of the shader bytecode
	D3D12_SHADER_BYTECODE textVertexShaderBytecode = {};
	textVertexShaderBytecode.BytecodeLength = textVertexShader->GetBufferSize();
	textVertexShaderBytecode.pShaderBytecode = textVertexShader->GetBufferPointer();

	// compile pixel shader
	ID3DBlob* textPixelShader;
	hr = D3DCompileFromFile(L"TextPixelShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&textPixelShader,
		&errorBuff);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		MessageBox(NULL, L"Shader  Failed", L"failed", NULL);
		return false;
	}

	// fill out shader bytecode structure for pixel shader
	D3D12_SHADER_BYTECODE textPixelShaderBytecode = {};
	textPixelShaderBytecode.BytecodeLength = textPixelShader->GetBufferSize();
	textPixelShaderBytecode.pShaderBytecode = textPixelShader->GetBufferPointer();

	// create input layout

	// The input layout is used by the Input Assembler so that it knows
	// how to read the vertex data bound to it.

	D3D12_INPUT_ELEMENT_DESC textInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
	};

	// fill out an input layout description structure
	D3D12_INPUT_LAYOUT_DESC textInputLayoutDesc = {};

	// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	textInputLayoutDesc.NumElements = sizeof(textInputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	textInputLayoutDesc.pInputElementDescs = textInputLayout;

	// create the text pipeline state object (PSO)

	D3D12_GRAPHICS_PIPELINE_STATE_DESC textpsoDesc = {};
	textpsoDesc.InputLayout = textInputLayoutDesc;
	textpsoDesc.pRootSignature = rootSignature;
	textpsoDesc.VS = textVertexShaderBytecode;
	textpsoDesc.PS = textPixelShaderBytecode;
	textpsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	textpsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	textpsoDesc.SampleDesc = sampleDesc;
	textpsoDesc.SampleMask = 0xffffffff;
	textpsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	D3D12_BLEND_DESC textBlendStateDesc = {};
	textBlendStateDesc.AlphaToCoverageEnable = FALSE;
	textBlendStateDesc.IndependentBlendEnable = FALSE;
	textBlendStateDesc.RenderTarget[0].BlendEnable = TRUE;

	textBlendStateDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	textBlendStateDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	textBlendStateDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

	textBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	textBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	textBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	textBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	textpsoDesc.BlendState = textBlendStateDesc;
	textpsoDesc.NumRenderTargets = 1;
	D3D12_DEPTH_STENCIL_DESC textDepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	textDepthStencilDesc.DepthEnable = false;
	textpsoDesc.DepthStencilState = textDepthStencilDesc;

	hr = device->CreateGraphicsPipelineState(&textpsoDesc, IID_PPV_ARGS(&textPSO));
	if (FAILED(hr))
	{
		MessageBox(NULL, L"PSO  Failed", L"failed", NULL);
		return false;
	}



	// a cube
	Vertex vList[] = {
		// front face
		{ -0.5f,  0.5f, -0.5f, 0.0f, 0.0f },
		{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
		{ -0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
		{ 0.5f,  0.5f, -0.5f, 1.0f, 0.0f },

		// right side face
		{ 0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
		{ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f },
		{ 0.5f, -0.5f,  0.5f, 1.0f, 1.0f },
		{ 0.5f,  0.5f, -0.5f, 0.0f, 0.0f },

		// left side face
		{ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
		{ -0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, -0.5f, 1.0f, 0.0f },

		// back face
		{ 0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
		{ -0.5f, -0.5f,  0.5f, 1.0f, 1.0f },
		{ 0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f,  0.5f, 1.0f, 0.0f },

		// top face
		{ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f },
		{ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f },
		{ 0.5f,  0.5f, -0.5f, 1.0f, 1.0f },
		{ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f },

		// bottom face
		{ 0.5f, -0.5f,  0.5f, 0.0f, 0.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
		{ 0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
		{ -0.5f, -0.5f,  0.5f, 1.0f, 0.0f },
	};

	int vBufferSize = sizeof(vList);

	// create default heap
	// default heap is memory on the GPU. Only the GPU has access to this memory
	// To get data into this heap, we will have to upload the data using
	// an upload heap
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
										// from the upload heap to this heap
		nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&vertexBuffer));
	if (FAILED(hr))
	{

		MessageBox(NULL, L"	Vertecx failed  Failed", L"failed", NULL);
//		Running = false;
	//	return false;
	}

	// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
	vertexBuffer->SetName(L"Vertex Buffer Resource Heap");

	// create upload heap
	// upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
	// We will upload the vertex buffer using this heap to the default heap
	ID3D12Resource* vBufferUploadHeap;
	
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&vBufferUploadHeap));
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Upload vertex buffer  Failed", L"failed", NULL);
		//Running = false;
		//return false;
	}
	vBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(vList); // pointer to our vertex array
	vertexData.RowPitch = vBufferSize; // size of all our triangle vertex data
	vertexData.SlicePitch = vBufferSize; // also the size of our triangle vertex data

										 // we are now creating a command with the command list to copy the data from
										 // the upload heap to the default heap
	UpdateSubresources(commandList, vertexBuffer, vBufferUploadHeap, 0, 0, 1, &vertexData);

	// transition the vertex buffer data from copy destination state to vertex buffer state
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));


	DWORD iList[] = {
		// ffront face
		0, 1, 2, // first triangle
		0, 3, 1, // second triangle

				 // left face
				 4, 5, 6, // first triangle
				 4, 7, 5, // second triangle

						  // right face
						  8, 9, 10, // first triangle
						  8, 11, 9, // second triangle

									// back face
									12, 13, 14, // first triangle
									12, 15, 13, // second triangle

												// top face
												16, 17, 18, // first triangle
												16, 19, 17, // second triangle

															// bottom face
															20, 21, 22, // first triangle
															20, 23, 21, // second triangle
	};

	int iBufferSize = sizeof(iList);

	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(iBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // start in the copy destination state
		nullptr, // optimized clear value must be null for this type of resource
		IID_PPV_ARGS(&indexBuffer));



	// Create the depth/stencil buffer

	// create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsDescriptorHeap));
	if (FAILED(hr))
	{
		//Running = false;
		//return false;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, 600, 700, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&depthStencilBuffer)
	);
	if (FAILED(hr))
	{
		//Running = false;
		//return false;
	}
	dsDescriptorHeap->SetName(L"Depth/Stencil Resource Heap");

	device->CreateDepthStencilView(depthStencilBuffer, &depthStencilDesc, dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart());





	for (int i = 0; i < frameBufferCount; ++i)
	{
		// create resource for cube 1
		hr = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // this heap will be used to upload the constant buffer data
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT), // size of the resource heap. Must be a multiple of 64KB for single-textures and constant buffers
			D3D12_RESOURCE_STATE_GENERIC_READ, // will be data that is read from so we keep it in the generic read state
			nullptr, // we do not have use an optimized clear value for constant buffers
			IID_PPV_ARGS(&constantBufferUploadHeaps[i]));
		if (FAILED(hr))
		{
			MessageBox(NULL, L"	Vertecx failed  Failed", L"failed", NULL);
			return false;
		}
		constantBufferUploadHeaps[i]->SetName(L"Constant Buffer Upload Resource Heap");

		ZeroMemory(&cbPerObject, sizeof(cbPerObject));

		CD3DX12_RANGE readRange(0, 0);	// We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)

										// map the resource heap to get a gpu virtual address to the beginning of the heap
		hr = constantBufferUploadHeaps[i]->Map(0, &readRange, reinterpret_cast<void**>(&cbvGPUAddress[i]));

		// Because of the constant read alignment requirements, constant buffer views must be 256 bit aligned. Our buffers are smaller than 256 bits,
		// so we need to add spacing between the two buffers, so that the second buffer starts at 256 bits from the beginning of the resource heap.
		memcpy(cbvGPUAddress[i], &cbPerObject, sizeof(cbPerObject)); // cube1's constant buffer data
		memcpy(cbvGPUAddress[i] + ConstantBufferPerObjectAlignedSize, &cbPerObject, sizeof(cbPerObject)); // cube2's constant buffer data
	}
















	commandList->Close();


	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT; // 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
	indexBufferView.SizeInBytes = iBufferSize;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 299;
	viewport.Height = 322;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;



	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = 299;
	scissorRect.bottom = 322;



	// build projection and view matrix
	XMMATRIX tmpMat = XMMatrixPerspectiveFovLH(45.0f*(3.14f / 180.0f), (float)1200 / (float)800, 0.1f, 1000.0f);
	XMStoreFloat4x4(&cameraProjMat, tmpMat);

	// set starting camera state
	cameraPosition = XMFLOAT4(0.0f, 2.0f, -4.0f, 0.0f);
	cameraTarget = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	cameraUp = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

	// build view matrix
	XMVECTOR cPos = XMLoadFloat4(&cameraPosition);
	XMVECTOR cTarg = XMLoadFloat4(&cameraTarget);
	XMVECTOR cUp = XMLoadFloat4(&cameraUp);
	tmpMat = XMMatrixLookAtLH(cPos, cTarg, cUp);
	XMStoreFloat4x4(&cameraViewMat, tmpMat);

	// set starting cubes position
	// first cube
	cube1Position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); // set cube 1's position
	XMVECTOR posVec = XMLoadFloat4(&cube1Position); // create xmvector for cube1's position

	tmpMat = XMMatrixTranslationFromVector(posVec); // create translation matrix from cube1's position vector
	XMStoreFloat4x4(&cube1RotMat, XMMatrixIdentity()); // initialize cube1's rotation matrix to identity matrix
	XMStoreFloat4x4(&cube1WorldMat, tmpMat); // store cube1's world matrix

											 // second cube
	cube2PositionOffset = XMFLOAT4(1.5f, 0.0f, 0.0f, 0.0f);
	posVec = XMLoadFloat4(&cube2PositionOffset) + XMLoadFloat4(&cube1Position); // create xmvector for cube2's position
																				// we are rotating around cube1 here, so add cube2's position to cube1

	tmpMat = XMMatrixTranslationFromVector(posVec); // create translation matrix from cube2's position offset vector
	XMStoreFloat4x4(&cube2RotMat, XMMatrixIdentity()); // initialize cube2's rotation matrix to identity matrix
	XMStoreFloat4x4(&cube2WorldMat, tmpMat); // store cube2's world matrix






}

void  engine::WaitForPreviousFrame()
{
	HRESULT hr;

	// swap the current rtv buffer index so we draw on the correct buffer
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
	if (fence[frameIndex]->GetCompletedValue() < fenceValue[frameIndex])
	{
		// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
		hr = fence[frameIndex]->SetEventOnCompletion(fenceValue[frameIndex], fenceEvent);
		if (FAILED(hr))
		{
			//Running = false;
		}

		// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
		// has reached "fenceValue", we know the command queue has finished executing
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	// increment fenceValue for next frame
	fenceValue[frameIndex]++;
}

void engine::Render()
{




	XMMATRIX rotXMat = XMMatrixRotationX(0.001f * 1);
	XMMATRIX rotYMat = XMMatrixRotationY(0.002f * 1);
	XMMATRIX rotZMat = XMMatrixRotationZ(0.003f * 1);

	// add rotation to cube1's rotation matrix and store it
	XMMATRIX rotMat = XMLoadFloat4x4(&cube1RotMat) * rotXMat * rotYMat * rotZMat;
	XMStoreFloat4x4(&cube1RotMat, rotMat);

	// create translation matrix for cube 1 from cube 1's position vector
	XMMATRIX translationMat = XMMatrixTranslationFromVector(XMLoadFloat4(&cube1Position));

	// create cube1's world matrix by first rotating the cube, then positioning the rotated cube
	XMMATRIX worldMat = rotMat * translationMat;

	// store cube1's world matrix
	XMStoreFloat4x4(&cube1WorldMat, worldMat);

	// update constant buffer for cube1
	// create the wvp matrix and store in constant buffer
	XMMATRIX viewMat = XMLoadFloat4x4(&cameraViewMat); // load view matrix
	XMMATRIX projMat = XMLoadFloat4x4(&cameraProjMat); // load projection matrix
	XMMATRIX wvpMat = XMLoadFloat4x4(&cube1WorldMat) * viewMat * projMat; // create wvp matrix
	XMMATRIX transposed = XMMatrixTranspose(wvpMat); // must transpose wvp matrix for the gpu
	XMStoreFloat4x4(&cbPerObject.wvpMat, transposed); // store transposed wvp matrix in constant buffer

													  // copy our ConstantBuffer instance to the mapped constant buffer resource
	memcpy(cbvGPUAddress[frameIndex], &cbPerObject, sizeof(cbPerObject));

	// now do cube2's world matrix
	// create rotation matrices for cube2
	rotXMat = XMMatrixRotationX(0.003f * 1);
	rotYMat = XMMatrixRotationY(0.002f * 1);
	rotZMat = XMMatrixRotationZ(0.001f * 1);

	// add rotation to cube2's rotation matrix and store it
	rotMat = rotZMat * (XMLoadFloat4x4(&cube2RotMat) * (rotXMat * rotYMat));
	XMStoreFloat4x4(&cube2RotMat, rotMat);

	// create translation matrix for cube 2 to offset it from cube 1 (its position relative to cube1
	XMMATRIX translationOffsetMat = XMMatrixTranslationFromVector(XMLoadFloat4(&cube2PositionOffset));

	// we want cube 2 to be half the size of cube 1, so we scale it by .5 in all dimensions
	XMMATRIX scaleMat = XMMatrixScaling(0.5f, 0.5f, 0.5f);

	// reuse worldMat. 
	// first we scale cube2. scaling happens relative to point 0,0,0, so you will almost always want to scale first
	// then we translate it. 
	// then we rotate it. rotation always rotates around point 0,0,0
	// finally we move it to cube 1's position, which will cause it to rotate around cube 1
	worldMat = scaleMat * translationOffsetMat * rotMat * translationMat;

	wvpMat = XMLoadFloat4x4(&cube2WorldMat) * viewMat * projMat; // create wvp matrix
	transposed = XMMatrixTranspose(wvpMat); // must transpose wvp matrix for the gpu
	XMStoreFloat4x4(&cbPerObject.wvpMat, transposed); // store transposed wvp matrix in constant buffer

													  // copy our ConstantBuffer instance to the mapped constant buffer resource
	memcpy(cbvGPUAddress[frameIndex] + ConstantBufferPerObjectAlignedSize, &cbPerObject, sizeof(cbPerObject));

	// store cube2's world matrix
	XMStoreFloat4x4(&cube2WorldMat, worldMat);



	HRESULT hr;

	// We have to wait for the gpu to finish with the command allocator before we reset it
	WaitForPreviousFrame();

	// we can only reset an allocator once the gpu is done with it
	// resetting an allocator frees the memory that the command list was stored in
	f1 << "frameIndex " << frameIndex<< endl;
	f1.close();
	hr = commandAllocator[frameIndex]->Reset();
	if (hr== E_FAIL)
	{
		MessageBox(NULL, L"commandAllocator Resey  Failed", L"failed", NULL);
		//return false;
	}

	// reset the command list. by resetting the command list we are putting it into
	// a recording state so we can start recording commands into the command allocator.
	// the command allocator that we reference here may have multiple command lists
	// associated with it, but only one can be recording at any time. Make sure
	// that any other command lists associated to this command allocator are in
	// the closed state (not recording).
	// Here you will pass an initial pipeline state object as the second parameter,
	// but in this tutorial we are only clearing the rtv, and do not actually need
	// anything but an initial default pipeline, which is what we get by setting
	// the second parameter to NULL
	hr = commandList->Reset(commandAllocator[frameIndex], pipelineStateObject);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Reset  Failed", L"failed", NULL);
	}

	// here we start recording commands into the commandList (which all the commands will be stored in the commandAllocator)

	// transition the "frameIndex" render target from the present state to the render target state so the command list draws to it starting from here
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);

	// get a handle to the depth/stencil buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// set the render target for the output merger stage (the output of the pipeline)
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// Clear the render target by using the ClearRenderTargetView command
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// clear the depth/stencil buffer
	commandList->ClearDepthStencilView(dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// set root signature
	commandList->SetGraphicsRootSignature(rootSignature); // set the root signature

														  // set the descriptor heap
//	ID3D12DescriptorHeap* descriptorHeaps[] = { mainDescriptorHeap };
//	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// set the descriptor table to the descriptor heap (parameter 1, as constant buffer root descriptor is parameter index 0)
//	commandList->SetGraphicsRootDescriptorTable(1, mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	commandList->RSSetViewports(1, &viewport); // set the viewports
	commandList->RSSetScissorRects(1, &scissorRect); // set the scissor rects
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView); // set the vertex buffer (using the vertex buffer view)
	commandList->IASetIndexBuffer(&indexBufferView);























	// first cube

	// set cube1's constant buffer
	commandList->SetGraphicsRootConstantBufferView(0, constantBufferUploadHeaps[frameIndex]->GetGPUVirtualAddress());

	// draw first cube
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

	// second cube

	// set cube2's constant buffer. You can see we are adding the size of ConstantBufferPerObject to the constant buffer
	// resource heaps address. This is because cube1's constant buffer is stored at the beginning of the resource heap, while
	// cube2's constant buffer data is stored after (256 bits from the start of the heap).
	///commandList->SetGraphicsRootConstantBufferView(0, constantBufferUploadHeaps[frameIndex]->GetGPUVirtualAddress() + ConstantBufferPerObjectAlignedSize);

	// draw second cube
	//commandList->DrawIndexedInstanced(numCubeIndices, 1, 0, 0, 0);

	// draw the text
	//RenderText(arialFont, std::wstring(L"FPS: ") + std::to_wstring(timer.fps), XMFLOAT2(0.02f, 0.01f), XMFLOAT2(2.0f, 2.0f));

	// transition the "frameIndex" render target from the render target state to the present state. If the debug layer is enabled, you will receive a
	// warning if present is called on the render target when it's not in the present state
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	hr = commandList->Close();
	if (FAILED(hr))
	{
		MessageBox(NULL, L"commandList->Close()  Failed!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", L"failed", NULL);
	}



	HRESULT hrR;



					  // create an array of command lists (only one command list here)
	ID3D12CommandList* ppCommandLists[] = { commandList };

	// execute the array of command lists
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// this command goes in at the end of our command queue. we will know when our command queue 
	// has finished because the fence value will be set to "fenceValue" from the GPU since the command
	// queue is being executed on the GPU
	hrR = commandQueue->Signal(fence[frameIndex], fenceValue[frameIndex]);
	if (FAILED(hr))
	{
		//Running = false;
	}

	// present the current backbuffer
	hrR = swapChain->Present(0, 0);
	if (FAILED(hr))
	{
		//Running = false;
	}






}


