#using <System.dll>
#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>
#include <tlhelp32.h>
#include <iostream>


using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

int main() {
	//share memory initialization
	SMObject PMObj(_TEXT("PMObj"), sizeof(PM));
	SMObject XboxObj(_TEXT("XboxObj"), sizeof(Xbox));
	PM* PMSMPtr = nullptr;
	Xbox* XboxSMPtr = nullptr;
	//access to shared memory
	PMObj.SMAccess();
	if (PMObj.SMAccessError) {
		Console::WriteLine("PM shared memory access failed.");
	}
	XboxObj.SMAccess();
	if (XboxObj.SMAccessError) {
		Console::WriteLine("Xbox shared memory access failed.");
	}
	PMSMPtr = (PM*)PMObj.pData;
	XboxSMPtr = (Xbox*)XboxObj.pData;
	
	/////////////////////////////////////////////////////////////////////////
	int flag = 0;
	int counter = 0;
	PMSMPtr->Shutdown.Flags.Vehicle = 0;

	while (!PMSMPtr->Shutdown.Flags.Vehicle) {
		//System::Threading::Thread::Sleep(150);
		//Heartbeats part
		PMSMPtr->Heartbeats.Flags.Vehicle = 1;
		if (PMSMPtr->PMHeartbeats.Flags.Vehicle == 1)
		{
			PMSMPtr->PMHeartbeats.Flags.Vehicle = 0;
			counter = 0;
		}
		else {
			if (++counter > 3000)
				PMSMPtr->Shutdown.Status = 0xff;
		}

		/////////////////////////////////////////////////////////////////////
		
		//TCP communication connection
		int PortNumber = 25000;
		// Pointer to TcpClent type object on managed heap
		TcpClient^ Client;
		// arrays of unsigned chars to send data
		array<unsigned char>^ SendData;
		array<unsigned char>^ SendID;
		// Creat TcpClient object and connect to it
		System::String^ ID = gcnew System::String("5146927\n");
		Client = gcnew TcpClient("192.168.1.200", PortNumber);
		// Configure connection
		Client->NoDelay = true;
		Client->ReceiveTimeout = 500;//ms
		Client->SendTimeout = 500;//ms
		Client->ReceiveBufferSize = 1024;
		Client->SendBufferSize = 1024;
		// unsigned char arrays of 16 bytes each are created on managed heap
		SendData = gcnew array<unsigned char>(16); // the size may be bigger
		SendID = gcnew array<unsigned char>(16);
		SendID = System::Text::Encoding::ASCII->GetBytes(ID);

		NetworkStream^ Stream = Client->GetStream();
		Stream->Write(SendID, 0, SendID->Length);
		
		//send xbox control data to vehicle
		//System::Threading::Thread::Sleep(50);
		
		if (flag == 0)
		{
			flag = 1;
		}
		else
		{
			flag = 0;
		}

		System::String^ Hash = "#";
		System::String^ Space = " ";
		System::String^ Speed = System::Convert::ToString(XboxSMPtr->speed);
		System::String^ Steer = System::Convert::ToString(XboxSMPtr->steer);
		System::String^ Flag = System::Convert::ToString(flag);

		Speed = System::Convert::ToString(Speed);
		Steer = System::Convert::ToString(Steer);
		Flag = System::Convert::ToString(Flag);

		array<System::String^>^ DataForCat = { Hash, Space, Steer, Space, Speed, Space, Flag, Space, Hash };
		System::String^ Data = System::String::Concat(DataForCat);
		SendData = System::Text::Encoding::ASCII->GetBytes(Data);
		Console::WriteLine(Data);
		Stream->Write(SendData, 0, SendData->Length);

		Stream->Close();
		Client->Close();
		
		//System::Threading::Thread::Sleep(150);
		if (_kbhit() || XboxSMPtr->shutAll == 1) break;
	}

	
	return 0;
}