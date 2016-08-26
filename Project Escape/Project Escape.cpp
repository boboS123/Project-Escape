// Project Escape.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Project Escape.h"
#include"Events\EventManagement.h"
#include"App\App.h"
#include "EngineDx12\engine.h"

#pragma comment(lib, "escape.lib")

class myApplication: public engine {
public:
 
	HWND test;
	HMENU menu;

	

	void UpdateScene(float dt) {
		//manager.GetAcces()->SendMessageToListeners(RIGHTMOUSECLICKED,1,1);
		//manager.test1 = 7;
		Render();
	}

	void DrawScene(float dt) {

	}

	void BeforeWindowInit() {

	}

	void AfterWindowInit() {

	}


	//myApplication()  : engine(App(1, 1, 1, 1, test, menu)) {}
		



	
};

myApplication *myApp = new myApplication;


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

	myApp->WindowInitialization();
	myApp->InitiateDx12API();



	myApp->Run();
	



}

