#pragma once
#ifndef IEVENTHANDLER
#define IEVENTHANDLER

#include "Events/EventManagement.h"

enum EVENTTYPE {
	RIGHTMOUSECLICKED,
	LEFTMOUSECLICKED,
	MIDDLEMOUSECLICKED,
	MOUSEMOVED,
	RIGHTMOUSERELEASED,
	LEFTMOUSERELEASED
};






//A class that is inherited by all classes that use events

class EventManagement;
class __declspec(dllexport)  EventList {
public:
	EventList() {
		EventManagement::GetAcces()->RegisterHandle(this);
	}


	//EventList *start=nullptr;
	EventList *nextList=nullptr;
	
	//How the class reacts to a message

	virtual void EventHandler(int EventType, int arg1, int arg2)=0;

	 void SendMessageEvnt(EVENTTYPE Event1,int arg1, int arg2) {
		 EventManagement::GetAcces()->SendMessageToListeners(Event1,arg1,arg2);
	}

	//Register this class pointer into a list of classes to be processed
	 void RegisterClassEvnt(EventList *classEvnt);

};

#endif