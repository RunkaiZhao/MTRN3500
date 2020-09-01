#using <System.dll>
#include "SMObject.h"
#include <SMStructs.h>
#include <conio.h>
#include <tlhelp32.h>
#include <iostream>

#include <Windows.h>
#include <xinput.h>
#include <math.h>
#include <conio.h>
#include "XinputWrapper.h"
#include "XBoxController.h"

using namespace System;
using namespace System::Threading;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

int main()
{	
	//share memory access
	SMObject PMObj(_TEXT("PMObj"), sizeof(PM));
	SMObject XboxObj(_TEXT("XboxObj"), sizeof(Xbox));

	PM* PMPtr = nullptr;
	Xbox* XboxPtr = nullptr;

	PMObj.SMAccess();
	if (PMObj.SMAccessError) {
		Console::WriteLine("Shared memory creation failed.");
	}
	XboxObj.SMAccess();
	if (XboxObj.SMAccessError) {
		Console::WriteLine("Shared memory access failed.");
	}
	PMPtr = (PM*)PMObj.pData;
	XboxPtr = (Xbox*)XboxObj.pData;
	

	XInputWrapper xinput;
	GamePad::XBoxController controller(&xinput, 0);

	if (controller.IsConnected() == 1) {
		std::cout << "controller is already connected." << std::endl;
	}
	else {
		std::cout << "controller is not connected yet." << std::endl;
	}

	PMPtr->Shutdown.Flags.Xbox = 0;
	int counter = 0;

	while (!PMPtr->Shutdown.Flags.Xbox) {
		System::Threading::Thread::Sleep(100);
		//hearbeat checker
		PMPtr->Heartbeats.Flags.Xbox = 1;
		if (PMPtr->PMHeartbeats.Flags.Xbox == 1) {
			PMPtr->PMHeartbeats.Flags.Xbox = 0;
			counter = 0;
		}
		else {
			if (++counter > 300)
				PMPtr->Shutdown.Status = 0xff;
		}

		//controller.IsConnected();
		if (controller.PressedA() == 1) {
			std::cout << "You are pressing button A." << std::endl;
			XboxPtr->shutAll = 1;
			System::Threading::Thread::Sleep(10);
			break;
		}

		//Right thumb joystick represent steering
		float R_X = controller.RightThumbLocation().GetX();
		float deadZone = 5000.0;
		if (R_X == -32768)
			R_X = -32767;

		float angle_scaled;
		if (R_X > 5000.0) {
			angle_scaled = (R_X - deadZone) * 32767.0 / (32767.0 - deadZone);
		}
		else if (R_X < -5000.0) {
			angle_scaled = (R_X + deadZone) * (-32767.0) / (-32767.0 + deadZone);
		}
		else {
			angle_scaled = 0;
		}
		float steerAngle = 40 * angle_scaled / 32767;
		XboxPtr->steer = -steerAngle; // store the steering angle into SM
		std::cout << XboxPtr->steer << "   ";


		//Left thumb joystick represent speed
		float L_Y = controller.LeftThumbLocation().GetY();
		if (L_Y == -32768)
			L_Y = -32767;

		float YLength_scaled;
		if (L_Y > 5000.0) {
			YLength_scaled = (L_Y - deadZone) * 32767.0 / (32767.0 - deadZone);
		}
		else if (L_Y < -5000.0) {
			YLength_scaled = (L_Y + deadZone) * (-32767.0) / (-32767.0 + deadZone);
		}
		else {
			YLength_scaled = 0;
		}

		float speed = 1 * YLength_scaled / 32767;
		XboxPtr->speed = speed;
		std::cout << XboxPtr->speed << std::endl;

		if (_kbhit()) break;
	}
	return 0;
}