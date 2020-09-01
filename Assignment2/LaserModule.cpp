#include "LaserModule.h"

laser::laser(int portnumber, System::String^ ipaddress)
{
	this->Portnumber = portnumber;
	this->IPAddress = ipaddress;
}

void laser::TcpInitialization()
{
	System::String^ AskScan = gcnew System::String("sRN LMDscandata");
	// Creat TcpClient object and connect to it
	this->Client = gcnew TcpClient(this->IPAddress, this->Portnumber);
	// Configure connection
	this->Client->NoDelay = true;
	this->Client->ReceiveTimeout = 500;//ms
	this->Client->SendTimeout = 500;//ms
	this->Client->ReceiveBufferSize = 1024;
	this->Client->SendBufferSize = 1024;
	// unsigned char arrays of 16 bytes each are created on managed heap
	this->SendData = gcnew array<unsigned char>(16);
	this->ReadData = gcnew array<unsigned char>(2500);
	// Convert string command to an array of unsigned char
	this->SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);
	// Get the network streab object associated with clien so we 
	// can use it to read and write
	this->Stream = Client->GetStream();

	System::String^ ID = gcnew System::String("5146927\n");
	array<unsigned char>^ SendID;
	array<unsigned char>^ ReadID;
	SendID = gcnew array<unsigned char>(16);
	ReadID = gcnew array<unsigned char>(2500);
	SendID = System::Text::Encoding::ASCII->GetBytes(ID);
	this->Stream->Write(SendID, 0, SendID->Length);
	this->Stream->Read(ReadID, 0, ReadID->Length);
}

void laser::TcpRequest()
{
	System::Threading::Thread::Sleep(100);
	this->Stream->WriteByte(0x02);
	this->Stream->Write(this->SendData, 0, this->SendData->Length);
	this->Stream->WriteByte(0x03);
	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	System::Threading::Thread::Sleep(10);
}

void laser::TcpReceive()
{
	this->Stream->Read(this->ReadData, 0, this->ReadData->Length);
}

void laser::GetResponseData()
{
	this->ResponseData = System::Text::Encoding::ASCII->GetString(this->ReadData);
}

void laser::GetXYRangeData(System::String^ responsedata)
{
	array<System::String^>^ stringArray = responsedata->Split(' ');

	//search LMDscandata
	/*
	int index = 0;
	while (1) {
		if (stringArray[index] == "LMDscandata") {
			break;
		}
		index++;
	}
	index -= 1;
	std::cout << index << std::endl;*/
	if (stringArray[1] == "LMDscandata") {
		this->discard = 0;
		this->StartAngle = (double)Convert::ToUInt32(stringArray[23], 16);
		this->Resolution = (double)Convert::ToUInt16(stringArray[24], 16) / 10000;
		this->NumPoints = (double)Convert::ToUInt16(stringArray[25], 16);

		std::cout << this->StartAngle << std::endl;
		std::cout << this->Resolution << std::endl;
		std::cout << this->NumPoints << std::endl;

		this->distance = gcnew array<double>(this->NumPoints);
		this->cartesian_X = gcnew array<double>(this->NumPoints);
		this->cartesian_Y = gcnew array<double>(this->NumPoints);

		for (int i = 0; i < this->NumPoints; i++) {
			this->distance[i] = Convert::ToUInt32(stringArray[26 + i], 16);
			this->cartesian_X[i] = this->distance[i] * Math::Sin(i * this->Resolution * Math::PI / 180.0);
			this->cartesian_Y[i] = this->distance[i] * Math::Cos(i * this->Resolution * Math::PI / 180.0);
		}
	}
	else {
		this->discard = 1;
	}
}

void laser::Close()
{
	this->Stream->Close();
	this->Client->Close();
}




/*array<System::String^>^ laser::Get_StringArray(NetworkStream^ stream, array<unsigned char>^ recvData)
{
	stream->Read(recvData, 0, recvData->Length); //read a single scan into recvData
	System::String^ LaserMeasurements = System::Text::Encoding::ASCII->GetString(recvData); //transfer binary to string(ascii)
	array<System::String^>^ StringArray = LaserMeasurements->Split(' '); // split every data by space, and store each of them into an array.
	return StringArray;
}

double laser::Getter_StartAngle(array<System::String^>^ stringarray)
{
	double startangle = System::Convert::ToInt32(stringarray[23], 16);
	return startangle;
}

double laser::Getter_Resolution(array<System::String^>^ stringarray)
{
	double resolution = System::Convert::ToInt32(stringarray[24], 16);
	return resolution;
}

double laser::Getter_NumberPoints(array<System::String^>^ stringarray)
{
	double numberpoints = System::Convert::ToInt32(stringarray[25], 16);
	return numberpoints;
}

double laser::Getter_distance(int index, array<System::String^>^ stringarray)
{
	array<double>^ distance = gcnew array<double>(this->Getter_NumberPoints);
	distance[index] = System::Convert::ToInt32(stringarray[26 + index], 16);
	return distance[index];
}

double laser::Getter_cartesianX(int index, double resolution)
{	
	array<double>^ cartesianX = gcnew array<double>(this->Getter_NumberPoints); //可能要另写一个member function

	//cartesianX[index] = distance[index]*Math::Sin(i*resolution*Math::PI/180);
	//cartesianY[index] = distnace[index]*Math::Cos(i*resolution*Math::PI/180);
}*/

