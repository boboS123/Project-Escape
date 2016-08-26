#pragma once
//
#ifndef EVENTMANAGEMENT
#define EVENTMANAGEMENT
//A class that manages pointers to event classes
class EventList;
class __declspec(dllexport)  EventManagement {

	

public:
	 static EventManagement *management_singleton;
	 //int test1;
	//A pointer to Class Members
	EventList *list;
	EventList *currenti = list;
	EventManagement() : list(nullptr) {}

	//The singleton pattern
	

	static EventManagement* GetAcces();

	void RegisterHandle(EventList *handleReg);

	void SendMessageToListeners(int Event, int arg1, int arg2);

};


#endif