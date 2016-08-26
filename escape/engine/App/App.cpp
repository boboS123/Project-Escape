#include "stdafx.h"
#include "App.h"
#include "Screamer.h"

App* tApp = 0;
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before returns, and thus before mhMainWnd is valid.
	return tApp->MsgProc(hwnd, msg, wParam, lParam);
}

Screamer scr;

int App::Run()
{
	MSG msg = { 0 };

	//mTimer.Reset();

	while (msg.message != WM_QUIT)
	{


		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			//MessageBox(NULL, L"I'm still here", L"here", NULL);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Otherwise, do animation/game stuff.
		else
		{

			//Calculate the time passed since last Tick call

			//timer.Tick();
			UpdateScene(0);
		//	DrawScene(timer.DeltaTime());


		}






	}

	return (int)msg.wParam;
}



void App::WindowInitialization()
{

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"snow";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);

	}



	mhMainWnd = CreateWindow(L"snow", L"main",
		WS_OVERLAPPEDWINDOW^ WS_THICKFRAME, posX, posY, 100, 100, 0, 0, mhAppInst, 0);

	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
	}



	ShowWindow(mhMainWnd, SW_MAXIMIZE);
	UpdateWindow(mhMainWnd);

// 
// 	HRESULT hr;
// 
// 	// -- Create the Device -- //
// 
// 	IDXGIFactory4* dxgiFactory;
// 	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
// 	if (FAILED(hr))
// 	{
// 		MessageBox(0, L"CreateDXGIFactory1  Failed.", 0, 0);
// 	}
// 
// 
// 
// 
// 
// 
// 	IDXGIAdapter1* adapter; // adapters are the graphics card (this includes the embedded graphics on the motherboard)
// 
// 	int adapterIndex = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0
// 
// 	bool adapterFound = false; // set this to true when a good one was found
// 
// //	IDXGIAdapter1* adapter; // adapters are the graphics card (this includes the embedded graphics on the motherboard)
// 
// //	int adapterIndex = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0
// 
// //	bool adapterFound = false; // set this to true when a good one was found
// 
// 							   // find first hardware gpu that supports d3d 12
// 	while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
// 	{
// 		DXGI_ADAPTER_DESC1 desc;
// 		adapter->GetDesc1(&desc);
// 
// 		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
// 		{
// 			// we dont want a software device
// 			continue;
// 		}
// 
// 		// we want a device that is compatible with direct3d 12 (feature level 11 or higher)
// 		hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
// 		if (SUCCEEDED(hr))
// 		{
// 			adapterFound = true;
// 			break;
// 		}
// 
// 		adapterIndex++;
// 	}
// 	const int frameBufferCount = 3; // number of buffers we want, 2 for double buffering, 3 for triple buffering
// 
// 
// 
// 	ID3D12Device* device; // direct3d device
// 
// 	IDXGISwapChain3* swapChain; // swapchain used to switch between render targets
// 
// 	ID3D12CommandQueue* commandQueue; // container for command lists
// 
// 	ID3D12DescriptorHeap* rtvDescriptorHeap; // a descriptor heap to hold resources like the render targets
// 
// 	ID3D12Resource* renderTargets[frameBufferCount]; // number of render targets equal to buffer count
// 
// 	ID3D12CommandAllocator* commandAllocator[frameBufferCount]; // we want enough allocators for each buffer * number of threads (w
// 
// 
// // 	if (!adapterFound)
// // 	{
// // 		Running = false;
// // 		return false;
// // 	}
// 
// 	// Create the device
// 	hr = D3D12CreateDevice(
// 		adapter,
// 		D3D_FEATURE_LEVEL_11_0,
// 		IID_PPV_ARGS(&device)
// 	);
// 	if (FAILED(hr))
// 	{
//  		MessageBox(0, L"DeviceDx12 Failed.", 0, 0);
// 	}
// 	else {
// 		MessageBox(0, L"DeviceDx12 Success.", 0, 0);
// 	}




}



LRESULT App::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	POINT p;
	switch (msg)
	{

	case WM_ACTIVATE:

		return 0;




	case WM_SIZE:
		//OnResize();
		return 0;



	case WM_ENTERSIZEMOVE:

		return 0;




	case WM_EXITSIZEMOVE:

		return 0;


	case WM_DESTROY:

		//CloseApplication();
		PostQuitMessage(0);

		return 0;





	case WM_LBUTTONDOWN:
		GetCursorPos(&p);
		manager.GetAcces()->SendMessageToListeners(LEFTMOUSECLICKED, 1, 1);
//		ScreenToClient(mhMainWnd, &p);

		//EventDispatcher::Get()->SendEvent(E_MOUSELEFTBUTTONPRESS, p.x, p.y);
		//OnMouseDown(wParam, p.x, p.y);
		return 0;
	case WM_MBUTTONDOWN:

	case WM_RBUTTONUP:



	case WM_LBUTTONUP:

		GetCursorPos(&p);

//		ScreenToClient(mhMainWnd, &p);

		//OnMouseUp(wParam, p.x, p.y);
	//	EventDispatcher::Get()->SendEvent(E_MOUSELEFTBUTTONRELEASE, p.x, p.y);
		return 0;
	case WM_MBUTTONUP:

	case WM_RBUTTONDOWN:
		
		GetCursorPos(&p);
		//OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		//EventDispatcher::Get()->SendEvent(E_MOUSERIGHTBUTTONPRESS, p.x, p.y);
		
		return 0;
	case WM_MOUSEMOVE:
		GetCursorPos(&p);


	//	EventDispatcher::Get()->SendEvent(E_MOUSEMOVE, p.x, p.y);

		return 0;




	}


	return DefWindowProc(hwnd, msg, wParam, lParam);

}

