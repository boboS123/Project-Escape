#ifndef APP_H
#define APP_H
#include <windows.h>
#include "Events/IEventHandler.h"

class __declspec(dllexport)  App {

public:

// 	App(int resx, int resy, int posx, int posy, HWND parent, HMENU menu) :
// 		resX(resx), resY(resy), posX(posx), posY(posy), hwndPrent(parent), hMenu(menu) {}
// 	


	void InitiateApp(int resx, int resy, int posx, int posy, HWND parent, HMENU menu) {
		resX = resx;
		resY = resy;
		posX = posx;
		posY = posy; 
		hwndPrent = parent;
		hMenu = menu;

		
	}

	EventManagement manager;
	//Timer timer;
	int Run();
	virtual void UpdateScene(float dt)=0;
	virtual void DrawScene(float dt)=0;

	virtual void BeforeWindowInit()=0;
	virtual void AfterWindowInit()=0;

// 	void EventHandler(const Event &e) {
// 
// 	}


	void WindowInitialization();
	void Init() {

		BeforeWindowInit();

		WindowInitialization();
		AfterWindowInit();
	}

	
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:


	bool Initialization();

	virtual bool CloseApplication(){ return 1; };



	HINSTANCE mhAppInst;

	HWND      mhMainWnd;
	bool      mAppPaused;
	bool      mMinimized;
	bool      mMaximized;
	bool      mResizing;



	//Window resolution, position, parent and menu

	int posX;
	int posY;

	int resX;
	int resY;
	HWND hwndPrent;
	HMENU hMenu;



};



#endif