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





INT_PTR CALLBACK DialogProcedure(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return tApp->DialogProc(hDlg, uMsg, wParam, lParam);


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
	HWND hDlg;

	hDlg = CreateDialogParam(mhAppInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProcedure, 0);

	mhMainWnd = CreateWindow(L"snow", L"main",
		WS_OVERLAPPEDWINDOW^ WS_THICKFRAME, posX, posY, 800, 600, 0, 0, mhAppInst, 0);

	if (!hDlg)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
	}



	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
	}



	ShowWindow(hDlg, SW_SHOWNORMAL);
	UpdateWindow(hDlg);

	ShowWindow(mhMainWnd, SW_SHOWNORMAL);
	


	UpdateWindow(mhMainWnd);





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

LRESULT App::DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{


	case WM_INITDIALOG:
	{

		HWND hWnd = GetDlgItem(hwnd, IDC_STATIC5);


		SetWindowText(hWnd, L"adaptation");
		return TRUE;
	}




	}

	return FALSE;
}

