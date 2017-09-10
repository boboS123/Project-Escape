// FIleReader.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "FIleReader.h"
#include<Commctrl.h>
#include "XLibrary/XFile2.h"
#include "XLibrary/XVector.h"
#include "ResourceManager.h"
#include "LZ4/lz4.h"
#include "LZ4/lz4.c"
#pragma comment(lib, "escape.lib")


ResourceManager *mgr;



int FileSelected=-1;




INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndList = GetDlgItem(hDlg, IDC_LIST1);
	HWND hwndList2 = GetDlgItem(hDlg, IDC_LIST2);
	INT RES;


	int index;
	switch (uMsg)
	{









	case WM_INITDIALOG:
	{
		
		// Add items to list. 
		
	    HWND hwndList = GetDlgItem(hDlg, IDC_LIST1);

		SendMessage(hwndList, LB_ADDSTRING, 0,(LPARAM)L"InstanceFile");
		SendMessage(hwndList, LB_SETITEMDATA, 0, (LPARAM)0);


		SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)L"MeshFile");
		SendMessage(hwndList, LB_SETITEMDATA, 0, (LPARAM)1);



		SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)L"TextureFile");
		SendMessage(hwndList, LB_SETITEMDATA, 0, (LPARAM)2);






		SetFocus(hwndList);
		return TRUE;
	}






	case WM_COMMAND:


		switch (LOWORD(wParam))
		{
		case IDC_LIST2:
			switch (HIWORD(wParam)) {
			case LBN_SELCHANGE: {


				if (FileSelected == 0) {

					hwndList2 = GetDlgItem(hDlg, IDC_LIST2);
					int item_count = SendMessage(hwndList2, LB_GETCOUNT, 0, 0);




					for (int i = 0; i < item_count; i++) {

						//if an item gets selected
						if (SendMessage(hwndList2, LB_GETSEL, i, 0)) {

							
							
							
							
							HWND hwndBtn = GetDlgItem(hDlg, IDC_BUTTON1);
							EnableWindow(hwndBtn, TRUE);



							int a = mgr->getHeaderVertexID(i);//  [i]->VertexBufferID;

							wchar_t buffer[256];
							wsprintfW(buffer, L"%d", a);
							//SetWindowText(hWnd, buffer);

							if (a >= 0) {
								SendMessage(hwndBtn, WM_SETTEXT, 0, (LPARAM)buffer);
							}
							else {
								SendMessage(hwndBtn, WM_SETTEXT, 0, (LPARAM)_T("Not found"));
								EnableWindow(hwndBtn, FALSE);
							}




							HWND hwndBtn2 = GetDlgItem(hDlg, IDC_BUTTON2);
							a = mgr->getInstancesTextureMap(i);
							wchar_t buffer2[256];
							wsprintfW(buffer2, L"%d", a);
							//SetWindowText(hWnd, buffer);

							if (a >= 0) {
								SendMessage(hwndBtn2, WM_SETTEXT, 0, (LPARAM)buffer2);
							}
							else {
								SendMessage(hwndBtn2, WM_SETTEXT, 0, (LPARAM)_T("Not found"));
								EnableWindow(hwndBtn2, FALSE);
							}



							HWND hwndBtn3 = GetDlgItem(hDlg, IDC_BUTTON3);
//							a = mgr->getInstancesNormalMap(i);
							wchar_t buffer3[256];
							wsprintfW(buffer3, L"%d", a);
							//SetWindowText(hWnd, buffer);

							if (a >= 0) {
								SendMessage(hwndBtn3, WM_SETTEXT, 0, (LPARAM)buffer3);
							}
							else {
								SendMessage(hwndBtn3, WM_SETTEXT, 0, (LPARAM)_T("Not found"));
								EnableWindow(hwndBtn3, FALSE);
							}



							HWND hwndBtn4 = GetDlgItem(hDlg, IDC_BUTTON4);
//							a = mgr->getInstancesSpecularMap(i);
							wchar_t buffer4[256];
							wsprintfW(buffer4, L"%d", a);
							//SetWindowText(hWnd, buffer);

							if (a >= 0) {
								SendMessage(hwndBtn4, WM_SETTEXT, 0, (LPARAM)buffer4);
							}
							else {
								SendMessage(hwndBtn4, WM_SETTEXT, 0, (LPARAM)_T("Not found"));
								EnableWindow(hwndBtn4, FALSE);
							}





							HWND hwndBtn5 = GetDlgItem(hDlg, IDC_BUTTON5);
							a = mgr->getInstancesObjectType(i);

							wchar_t buffer5[256];
							wsprintfW(buffer5, L"%d", a);
							//SetWindowText(hWnd, buffer);

							if (a >= 0) {
								SendMessage(hwndBtn5, WM_SETTEXT, 0, (LPARAM)buffer5);
							}
							else {
								SendMessage(hwndBtn5, WM_SETTEXT, 0, (LPARAM)_T("Not found"));
								EnableWindow(hwndBtn5, FALSE);
							}




						}

					}

				}
				//FileSelected



				//Mesh File
			






			}
			}
		break;




		case IDC_LIST1: 



			switch (HIWORD(wParam)) {
			case LBN_SELCHANGE: {



				//Detect what file was selected from the File list;
				hwndList = GetDlgItem(hDlg, IDC_LIST1);
				int item_count = SendMessage(hwndList, LB_GETCOUNT, 0, 0);
				
				for (int i = 0; i < item_count; i++) {
					if (SendMessage(hwndList, LB_GETSEL, i, 0)) {
						FileSelected = i;
					}
				}

				
				



				//FileSelected










// 
// 				HMENU hMenu;
// 				
// 
// 				hMenu = CreatePopupMenu();
// 			//	ClientToScreen(hwnd, &point);
// 
// 				AppendMenuW(hMenu, MF_STRING, 0, L"&New");
// 				AppendMenuW(hMenu, MF_STRING, 1, L"&Open");
// 				AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
// 				AppendMenuW(hMenu, MF_STRING, 2, L"&Quit");
// 
// 


	//			TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, 200, 100, 0, hDlg, NULL);

				//Set the num of files

				if (FileSelected == 0) {
					HWND hWnd = GetDlgItem(hDlg, IDC_MY_STATIC);
					
					
					int a = mgr->getnumInstancesTotalFile();
					wchar_t buffer[256];
					wsprintfW(buffer, L"%d", a);
					SetWindowText(hWnd, buffer);


					HWND hWnd2 = GetDlgItem(hDlg, IDC_num);
					SetWindowText(hWnd2, L"numInstances");


					HWND hwndList2 = GetDlgItem(hDlg, IDC_LIST2);


					//reset the old list 
					RES = SendMessage(hwndList2, LB_RESETCONTENT, 0, (LPARAM)0);



					//Populate second list with Items

					//HACK*
					for (int i = 0; i <  mgr->numInstancesTotalFile; i++) {


						int a = mgr->getInstancesInstanceID(i);
						wchar_t bufferT[256];
						wchar_t *buffer2 = L"meshID:";
						wsprintfW(bufferT, L"%d", a);
						//	wcscat_s(bufferT, buffer2);


						SendMessage(hwndList2, LB_ADDSTRING, 0, (LPARAM)bufferT);
						SendMessage(hwndList2, LB_SETITEMDATA, 0, (LPARAM)0);

					}



					HWND hwndBtn = GetDlgItem(hDlg, IDC_BUTTON1);

				//	EnableWindow(hwndBtn, FALSE);

				//	SendMessage(hwndBtn, WM_SETTEXT, 0, (LPARAM)_T("meshID:0"));

				}



				if (FileSelected == 1) {



					HWND hWnd = GetDlgItem(hDlg, IDC_MY_STATIC);


					int a = mgr->getMeshHeaderNumMeshes(0);
					wchar_t buffer[256];
					wsprintfW(buffer, L"%d", a);
					SetWindowText(hWnd, buffer);



					hWnd = GetDlgItem(hDlg, IDC_num);
					SetWindowText(hWnd, L"numMeshes");

					hWnd = GetDlgItem(hDlg, IDC_STATICX);
					SetWindowText(hWnd, L"numVerts:");
					


					HWND hwndBtn = GetDlgItem(hDlg, IDC_BUTTON1);
					//EnableWindow(hwndBtn, TRUE);
				//	ShowWindow(hwndBtn, FALSE);





				}

















			}
								
			}
			break;

		}
		break;



	case WM_CLOSE:

		
		

		if (MessageBox(hDlg, TEXT("Close the program?"), TEXT("Close"),
			MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			DestroyWindow(hDlg);
		}
		return TRUE;



		




	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	}

	return FALSE;
}






int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
   
	mgr = new ResourceManager;
	mgr->LoadInstances();
	mgr->loadMaterials();
	mgr->loadTextureHeader();
	mgr->loadmeshHeader();




	HWND hDlg;
	HWND hListView;

//	LoadInstances();

	hDlg = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc, 0);
	//hListView = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDC_LIST1), 0, listProc, 0);
	ShowWindow(hDlg, nCmdShow);


	BOOL ret;
	MSG msg;
	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
		if (ret == -1) /* error found */
			return -1;

		if (!IsDialogMessage(hDlg, &msg)) {
			TranslateMessage(&msg); /* translate virtual-key messages */
			DispatchMessage(&msg); /* send it to dialog procedure */
		}
	}









}

