#include "SMObject.h"
#include <SMStructs.h>
#include <conio.h>
#include <tlhelp32.h>
#include <iostream>

using namespace std;
using namespace System;
using namespace System::Threading;

STARTUPINFO s[10];
PROCESS_INFORMATION p[10];

void restart(int i);

TCHAR* Units[10] =
{	
	TEXT("GPSModule.exe"),
	TEXT("LaserModule.exe"),
	TEXT("XBoxModule.exe"),
	TEXT("VehicleControlModule.exe"),
	TEXT("DisplayModule.exe"),
};

bool IsProcessRunning(const char* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp(entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}

int main() {
	SMObject PMObj(_TEXT("PMObj"), sizeof(PM));
	SMObject LaserObj(_TEXT("LaserObj"), sizeof(Laser));
	SMObject XboxObj(_TEXT("XboxObj"), sizeof(Xbox));

	PM* PMSMPtr = nullptr;
	Xbox* XboxPtr = nullptr;

	//share memory creation
	PMObj.SMCreate();
	if (PMObj.SMAccessError) {
		Console::WriteLine("Shared memory creation failed.");
	}
	LaserObj.SMCreate();
	if (LaserObj.SMAccessError) {
		Console::WriteLine("Shared memory creation failed.");
	}
	XboxObj.SMCreate();
	if (XboxObj.SMAccessError) {
		Console::WriteLine("Shared memory creation failed.");
	}

	//share memory access
	PMObj.SMAccess();
	if (PMObj.SMAccessError) {
		Console::WriteLine("Shared memory access failed.");
	}
	XboxObj.SMAccess();
	if (XboxObj.SMAccessError) {
		Console::WriteLine("Shared memory access failed.");
	}

	// Starting the processes
	for (int i = 0; i < 5; i++)
	{
		// Check if each process is running
		if (!IsProcessRunning(Units[i]))
		{
			restart(i);
		}
		std::cout << "Started: " << Units[i] << std::endl;
		Sleep(500);
	}

	PMSMPtr = (PM*)PMObj.pData;
	XboxPtr = (Xbox*)XboxObj.pData;

	PMSMPtr->Shutdown.Flags.PM = 0;
	PMSMPtr->Heartbeats.Status = 0xff; // individual module should set flag up if they are awake.++++
	int waitTime_GPS = 0;
	int waitTime_laser = 0;
	int waitTime_vehicleControl = 0;
	int waitTime_xbox = 0;
	int waitTime_display = 0;
	int counter = 0;

	while (!PMSMPtr->Shutdown.Flags.PM) {
		PMSMPtr->PMHeartbeats.Flags.GPS = 1;
		PMSMPtr->PMHeartbeats.Flags.Laser = 1;
		PMSMPtr->PMHeartbeats.Flags.Vehicle = 1;
		PMSMPtr->PMHeartbeats.Flags.Xbox = 1;
		PMSMPtr->PMHeartbeats.Flags.display = 1;

		//checking  processor heartbeats
		Console::WriteLine("GPS heartbeat is {0}.", PMSMPtr->Heartbeats.Flags.GPS);
		Console::WriteLine("Laser heartbeat is {0}.", PMSMPtr->Heartbeats.Flags.Laser);
		Console::WriteLine("xbox heartbeat is {0}.", PMSMPtr->Heartbeats.Flags.Xbox);
		Console::WriteLine("vehicle control heartbeat is {0}.", PMSMPtr->Heartbeats.Flags.Vehicle);
		Console::WriteLine("display heartbeat is {0}.", PMSMPtr->Heartbeats.Flags.display);
		std::cout << std::endl;
		//checking GPS
		if (PMSMPtr->Heartbeats.Flags.GPS == 1)
			PMSMPtr->Heartbeats.Flags.GPS = 0; // set it down by PM
		else
		{	
			++waitTime_GPS;
			if (waitTime_GPS > 50)
			{
				restart(0);
				waitTime_GPS = 0;
			}
		}
		
		//checking Laser
		if (PMSMPtr->Heartbeats.Flags.Laser == 1)
			PMSMPtr->Heartbeats.Flags.Laser = 0;
		else {
			if (++waitTime_laser > 50)
				PMSMPtr->Shutdown.Status = 0xFF;
		}

		//checking xbox
		if (PMSMPtr->Heartbeats.Flags.Xbox == 1) {
			PMSMPtr->Heartbeats.Flags.Xbox = 0;
		}
		else {
			if (++waitTime_xbox > 50)
				PMSMPtr->Shutdown.Status = 0xff;
		}
		
		//checking vehicle controll
		if (PMSMPtr->Heartbeats.Flags.Vehicle == 1) {
			PMSMPtr->Heartbeats.Flags.Vehicle = 0;
		}
		else {
			if (++waitTime_vehicleControl > 100)
				PMSMPtr->Shutdown.Status = 0xff;
		}
		
		//display 
		if (PMSMPtr->Heartbeats.Flags.display == 1)
			PMSMPtr->Heartbeats.Flags.display = 0;
		else {
			if (++waitTime_display > 50)
				PMSMPtr->Shutdown.Status = 0xff;
		}
		
		if (_kbhit()) XboxPtr->shutAll = 1;
		
		System::Threading::Thread::Sleep(150);

	}
	
	return 0;
}

void restart(int i) {
	ZeroMemory(&s[i], sizeof(s[i]));
	s[i].cb = sizeof(s[i]);
	ZeroMemory(&p[i], sizeof(p[i]));
	// Start the child processes.

	if (!CreateProcess(NULL,   // No module name (use command line)
		Units[i],        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&s[i],            // Pointer to STARTUPINFO structure
		&p[i])           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("%s failed (%d).\n", Units[i], GetLastError());
		_getch();
	}
}