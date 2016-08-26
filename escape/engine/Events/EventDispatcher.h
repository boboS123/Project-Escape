#pragma once


class IEventHandler;

class __declspec(dllexport) EventDispatcher {

// definitions necessary to handle a singleton class
public:
  //returns a pointer to the EventDispatcher singleton instance
  static EventDispatcher*	Get();

private:
  // These are private so that only instance will ever be created
  // the _deviceList pointer is initialized to null as the linked list
  // is null-terminated
  EventDispatcher(void) : _deviceList(0) {;}
  ~EventDispatcher(void) {;}

  // pointer to singleton class
  static EventDispatcher*	_instanceOf;

// definitions of methods which handle events
public:

  void RegisterHandler(IEventHandler *device);

  // Sends the event to all the devices registered to listen
  void SendEvent(int eventType, int arg1 = 0, int arg2 = 0);

private:
  IEventHandler *_deviceList;
};