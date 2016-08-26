#include"stdafx.h"
#include"engine\Events\IEventHandler.h"
#include"engine\Events\EventManagement.h"




void EventManagement::RegisterHandle(EventList *handleReg) {
	if (list == nullptr) {
		list = handleReg;
	}
	else {

		//Iterate the list;
		while (currenti->nextList != NULL) {
			currenti = currenti->nextList;
		}

		currenti->nextList = handleReg;

	}


}

void EventManagement::SendMessageToListeners(int Event, int arg1, int arg2) {
	
	if (list == nullptr) {
		return;
	}
	currenti = list;
	currenti->EventHandler(Event, arg1, arg2);
	while (currenti->nextList != NULL) {
		
		currenti = currenti->nextList;
	}


};


 EventManagement* EventManagement::GetAcces() {

	if (management_singleton) {
		return management_singleton;
	}
	else
	{
		management_singleton = new EventManagement;
		return management_singleton;
	}

}
 EventManagement*  EventManagement::management_singleton = nullptr;

