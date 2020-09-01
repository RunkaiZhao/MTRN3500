
//Compile in a C++ CLR empty project
#using <System.dll>
#include "SMObject.h"
#include <SMStructs.h>
#include <conio.h>
#include <tlhelp32.h>
#include <iostream>
#include "LaserModule.h"

using namespace System;
using namespace System::Threading;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

//void data();

int main() {
	
	SMObject PMObj(_TEXT("PMObj"), sizeof(PM)); //declare object of class SMObject
	SMObject LaserObj(_TEXT("LaserObj"), sizeof(Laser));
	SMObject XboxObj(_TEXT("XboxObj"), sizeof(Xbox));

	PM* PMSMPtr = nullptr; //declare a pointer pointing to PM
	Laser* LaserPtr = nullptr;
	Xbox* XboxPtr = nullptr;

	//access to shared memory
	PMObj.SMAccess();
	if (PMObj.SMAccessError) {
		Console::WriteLine("Shared memory access failed.");
		Console::ReadKey();
		return -1;
	}
	LaserObj.SMAccess();
	if (LaserObj.SMAccessError) {
		Console::WriteLine("Laser shared memory access failed.");
	}
	XboxObj.SMAccess();
	if (XboxObj.SMAccessError) {
		Console::WriteLine("Laser shared memory access failed.");
	}

	PMSMPtr = (PM*)PMObj.pData; //PMSMPtr now is pointing to shared-memory
	LaserPtr = (Laser*)LaserObj.pData;
	XboxPtr = (Xbox*)XboxObj.pData;

	PMSMPtr->Shutdown.Flags.Laser = 0;
	int waitTime_laser = 0;
	
	int Portnumber = 23000;
	System::String^ IPAddress = gcnew System::String("192.168.1.200");
	laser LaserProcessor(Portnumber, IPAddress);
	LaserProcessor.TcpInitialization();
	
	while (!PMSMPtr->Shutdown.Flags.Laser) {
		System::Threading::Thread::Sleep(100);
		PMSMPtr->Heartbeats.Flags.Laser = 1; // indicate laser is awake

		if (PMSMPtr->PMHeartbeats.Flags.Laser == 1) {
			PMSMPtr->PMHeartbeats.Flags.Laser = 0;
			waitTime_laser = 0;
		}
		else {
			if (++waitTime_laser > 1000)
				PMSMPtr->Shutdown.Status = 0xFF;
		}
		
		LaserProcessor.TcpRequest();
		LaserProcessor.TcpReceive();
		LaserProcessor.GetResponseData();
		Console::WriteLine(LaserProcessor.ResponseData);
		LaserProcessor.GetXYRangeData(LaserProcessor.ResponseData);
		
		if (LaserProcessor.discard == 0) {
			for (int i = 0; i < 361; i++)
			{
				*(LaserPtr->XRange + i) = LaserProcessor.cartesian_X[i];
				*(LaserPtr->YRange + i) = LaserProcessor.cartesian_Y[i];
			}
		}
		else if (LaserProcessor.discard == 1) {
				std::cout << "this string has too much garbage data." << std::endl << std::endl << std::endl;
		}

		if (_kbhit() == 1 || XboxPtr->shutAll == 1) break;
		//System::Threading::Thread::Sleep(200);
	}

	LaserProcessor.Close();
	return 0;
}

/*Unhandled Exception : System.FormatException : Additional non - parsable characters are at the end of the string.
at System.ParseNumbers.StringToInt(String s, Int32 radix, Int32 flags, Int32* currPos)
at System.Convert.ToUInt32(String value, Int32 fromBase)
at laser.GetXYRangeData(String responsedata) in C : \Users\ted03\OneDrive\Desktop\VehicleModule\LaserModule\LaserModule.cpp:line 66
at main() in C : \Users\ted03\OneDrive\Desktop\VehicleModule\LaserModule\LaserMain.cpp:line 64
at _mainCRTStartup()*/