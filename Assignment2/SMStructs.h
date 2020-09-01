#pragma once

struct ModuleFlags {
	unsigned char PM : 1, GPS : 1, Laser : 1, Xbox : 1, Vehicle : 1, Unused : 3;
};

union ExecFlags {
	unsigned char Status;
	ModuleFlags Flags;
};

struct Remote
{
	double SetSpeed;
	double SetSteering;
};

struct GPS
{
	int waitTime;
	double Easting;
	double Northing;
	double Height;
};

struct Laser
{
	double XRange[361];
	double YRange[361];
};

struct Xbox
{
	float steer;
	float speed;
};

struct VehicleControl
{

};

struct PM
{
	double PMTimeStamp;
	ExecFlags Heartbeats;
	ExecFlags PMHeartbeats;
	ExecFlags Shutdown;
	//bool Shutdown;
};


