#using <System.dll>
#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>
#include <tlhelp32.h>
#include <iostream>
//#include "GPSModule.h"
#pragma pack(1)

//using namespace std;
using namespace System;
using namespace System::Threading;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

#define CRC32_POLYNOMIAL 0xEDB88320L

struct GPS//112 bytes
{
	unsigned char Header[4];
	unsigned char Discards1[40];
	double Northing;
	double Easting;
	double Height;
	unsigned char Discards2[40];
	unsigned int Checksum;
};

GPS LocalGPS;

unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);
unsigned long CRC32Value(int i);

int main() {

	PM* PMSMPtr = nullptr;
	Xbox* XboxPtr = nullptr;

	SMObject PMObj(_TEXT("PMObj"), sizeof(PM));
	SMObject XboxObj(_TEXT("XboxObj"), sizeof(Xbox));

	PMObj.SMAccess();
	if (PMObj.SMAccessError) {
		std::cout << "PM Shared memory access failed" << std::endl;
	}
	XboxObj.SMAccess();
	if (XboxObj.SMAccessError) {
		std::cout << "Xbox Shared memory access failed" << std::endl;
	}

	PMSMPtr = (PM*)PMObj.pData;
	XboxPtr = (Xbox*)XboxObj.pData;

	int waitTime = 0;
	PMSMPtr->Shutdown.Flags.GPS = 0;
	
	unsigned int calculatedCRC;
	int PortNumber = 24000;
	TcpClient^ Client;
	array<unsigned char>^ ReadData;
	Client = gcnew TcpClient("192.168.1.200", PortNumber);
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;
	Client->SendTimeout = 500;
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;
	ReadData = gcnew array<unsigned char>(2500);
	NetworkStream^ Stream = Client->GetStream();

	double Height;
	double Northing;
	double Easting;
	
	while (!PMSMPtr->Shutdown.Flags.GPS) {

		PMSMPtr->Heartbeats.Flags.GPS = 1;

		if (PMSMPtr->PMHeartbeats.Flags.GPS == 1)
		{
			PMSMPtr->PMHeartbeats.Flags.GPS = 0;
			waitTime = 0;
		}
		else
		{
			waitTime++;
			if (waitTime > 500)
			{
				PMSMPtr->Shutdown.Status = 0xFF;
			}
		}

		System::Threading::Thread::Sleep(200);
		
		if (Stream->DataAvailable)
		{
			Stream->Read(ReadData, 0, ReadData->Length);
		}

		//GPS LocalGPS;
		unsigned char* BytePtr = (unsigned char*)& LocalGPS;
		for (int i = 0; i < sizeof(GPS); i++)
		{
			*(BytePtr + i) = ReadData[i];
		}

		calculatedCRC = CalculateBlockCRC32(108, BytePtr);

		std::cout << "CalculatedCRC = " << calculatedCRC << std::endl;
		std::cout << "Checksum = " << LocalGPS.Checksum << std::endl;

		if (calculatedCRC == LocalGPS.Checksum)
		{
			Height = LocalGPS.Height;
			Northing = LocalGPS.Northing;
			Easting = LocalGPS.Easting;
		}
		else
		{
			return 0;
		}

		std::cout << "Northing = " << Northing << std::endl;
		std::cout << "Easting = " << Easting << std::endl;
		std::cout << "Height = " << Height << std::endl;
		std::cout << std::endl;

		
		if (_kbhit() == 1 || XboxPtr->shutAll == 1) break;
	}

	Stream->Close();
	Client->Close();
	return 0;
}



unsigned long CRC32Value(int i)
{
	unsigned long ulCRC;
	ulCRC = i;
	for (int j = 8; j > 0; j--)
	{
		if (ulCRC & 1)
			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
		else
			ulCRC >>= 1;
	}
	return ulCRC;
}

unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer)
{
	unsigned long ulTemp1;
	unsigned long ulTemp2;
	unsigned long ulCRC = 0;
	while (ulCount-- != 0)
	{
		ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
		ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xff);
		ulCRC = ulTemp1 ^ ulTemp2;
	}
	return(ulCRC);
}

