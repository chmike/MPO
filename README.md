MPO
===

**M**essage **P**assing **O**bject library.

This library provides a C++ Signal Slot communication mechanism. 

It differs from the boost::signal package in that the signal passes an object of the class **Message** to the slot.

A signal variable may be define as to emit only a sub class of messages and a slot to accept only a sub class of messages. 

A static cast on the message will be performed when calling the associated slot function or method when both message types respect the polymorphism rule. That is, the messages emitted by the Signal is not a parent class of the messages accepted by the connected slot. Otherwise a dynamic cast is performed on the message. 

The signal slot communication system is intended to be used with communicating classes organized in a network. Though this library allows for a signal or a slot to be a free variable and the slot to be associated to a method or a free variable. 

Signal and slots may be associated to a string name registered in a global directory to ease connection establishment. A use case is to load the connection definition from a configuration file. 

Action class
------------

The class Action is provided as candidate of such communicating object network. Though the user could use its own signal slot hosting classes.

Action classes are also named and registered in their own gobal directory. The system is designed so that these Action classes may be instantiated by an object factory based on a configuration file.

Execution and threads
---------------------

The system is designed to be run as single threaded with the design goal to be run from an a thread running asio.

If a slot method or function requires an undefined or long execution time, the user should define the slot method or function so that the effective task is delegated to a thread pool for asynchronous handling.

When a signal emits a Message object, the Message is simply queued for later dispatching. The Message will be dispatched to the slots when the Message::processNext() static method is called. 
This call execute one slot method or function call and returns false if the message queue is empty after execution. This return value could be used to set the executing thread back to sleep. 

The user may set a callback function to be called whenever a message is queued to ensure the thread processing the message queue is waken up if required.

Final notice
------------

This library is a work in progress. It has no logging, control and configuration functionality. 

It is provided as is, as a reference of some programming work performed by the author Christophe Meessen. 
