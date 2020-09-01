#pragma once

#using <System.dll>
#include "SMObject.h"
#include <SMStructs.h>
#include <conio.h>
#include <tlhelp32.h>
#include <iostream>
#include <math.h>

using namespace System;
using namespace System::Threading;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

ref class laser
{
private:
	TcpClient^ Client;
	int Portnumber;
	System::String^ IPAddress;
	//System::String^ ResponseData;
	double StartAngle;
	double Resolution;
public:
	// arrays of unsigned chars to send and receive data
	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
	NetworkStream^ Stream;
	System::String^ ResponseData;
	int NumPoints;
	array<double>^ distance;
	array<double>^ cartesian_X;
	array<double>^ cartesian_Y;

	bool discard;
public:
	laser(int portnumber, System::String^ ipaddress);
	void TcpInitialization();
	void TcpRequest();
	void TcpReceive();
	void GetResponseData();
	void GetXYRangeData(System::String^ responsedata);
	void Close();
};

