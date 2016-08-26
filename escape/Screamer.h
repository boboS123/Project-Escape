#pragma once
#include "stdafx.h"
#include "engine\Events\IEventHandler.h"
class Screamer : public EventList {


	void EventHandler(int EventType, int arg1, int arg2) {

		switch (EventType) {
		case 0:
			MessageBox(NULL, L"Message0", L"booboo", NULL);
			break;
		case 1:
			MessageBox(NULL, L"Message1", L"booboo", NULL);
			break;
		case 2:
			MessageBox(NULL, L"Message2", L"booboo", NULL);
			break;


		}
		
	 }

};