#include <iostream>
#include <Windows.h>
#include <xinput.h>
#include <math.h>
#include "XinputWrapper.h"
#include "XBoxController.h"

GamePad::XBoxController::XBoxController(XInputWrapper * xinput, DWORD id)
{
	this->xinput = xinput;
	user_ID = id;
}

DWORD GamePad::XBoxController::GetControllerId()
{
	return GamePad::XBoxController::user_ID;
}

bool GamePad::XBoxController::IsConnected()
{	

	if (xinput->XInputGetState(user_ID, &State) == ERROR_DEVICE_NOT_CONNECTED) {
		return 0;
	}
	else {
		return 1;
	}
}

bool GamePad::XBoxController::PressedA()
{
	if (IsConnected() == 1) {
		//xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x1000) {
			return 1;
		}
	} 
	else {
		return 0;
	}
}

bool GamePad::XBoxController::PressedB()
{
	if (IsConnected() == 1) {
		xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x2000) {
			return 1;
		}
	}
	else {
		return 0;
	}
}

bool GamePad::XBoxController::PressedX()
{
	if (IsConnected() == 1) {
		xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x4000) {
			return 1;
		}
	}
	else {
		return 0;
	}
}

bool GamePad::XBoxController::PressedY()
{
	if (IsConnected() == 1) {
		xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x8000) {
			return 1;
		}
	}
	else {
		return 0;
	}
}

bool GamePad::XBoxController::PressedLeftShoulder()
{
	if (IsConnected() == 1) {
		xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x0100) {
			return 1;
		}
	}
	else {
		return 0;
	}
}

bool GamePad::XBoxController::PressedRightShoulder()
{
	if (IsConnected() == 1) {
		xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x0200) {
			return 1;
		}
	}
	else {
		return 0;
	}
}

bool GamePad::XBoxController::PressedLeftDpad()
{
	if (IsConnected() == 1) {
		xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x0004) {
			return 1;
		}
	}
	else {
		return 0;
	}
}

bool GamePad::XBoxController::PressedRightDpad()
{
	if (IsConnected() == 1) {
		xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x0008) {
			return 1;
		}
	}
	else {
		return 0;
	}
}

bool GamePad::XBoxController::PressedUpDpad()
{
	if (IsConnected() == 1) {
		xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x0001) {
			return 1;
		}
	}
	else {
		return 0;
	}
}

bool GamePad::XBoxController::PressedDownDpad()
{
	if (IsConnected() == 1) {
		xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x0002) {
			return 1;
		}
	}
	else {
		return 0;
	}
}

bool GamePad::XBoxController::PressedStart()
{
	if (IsConnected() == 1) {
		xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x0010) {
			return 1;
		}
	}
	else {
		return 0;
	}
}

bool GamePad::XBoxController::PressedBack()
{
	if (IsConnected() == 1) {
		xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x0020) {
			return 1;
		}
	}
	else {
		return 0;
	}
}

bool GamePad::XBoxController::PressedLeftThumb()
{
	if (IsConnected() == 1) {
		xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x0040) {
			return 1;
		}
	}
	else {
		return 0;
	}
}

bool GamePad::XBoxController::PressedRightThumb()
{
	if (IsConnected() == 1) {
		xinput->XInputGetState(user_ID, &State);
		if (State.Gamepad.wButtons == 0x0080) {
			return 1;
		}
	}
	else {
		return 0;
	}
}

BYTE GamePad::XBoxController::LeftTriggerLocation()
{	
	if (IsConnected() == 0) {
		return 0;
	} else {
	xinput->XInputGetState(XBoxController::user_ID, &State);
	return State.Gamepad.bLeftTrigger;
	}
}

BYTE GamePad::XBoxController::RightTriggerLocation()
{
	if (IsConnected() == 0) {
		return 0;
	}
	else {
		xinput->XInputGetState(user_ID, &State);
		return State.Gamepad.bRightTrigger;
	}
}

GamePad::Coordinate GamePad::XBoxController::LeftThumbLocation()
{
	SHORT x = State.Gamepad.sThumbLX;
	SHORT y = State.Gamepad.sThumbLY;
	
	GamePad::Coordinate Joysticks(x,y);
	return Joysticks;
	//return coordinate(State.Gamepad.sThumbLX,State.Gamepad.sThumbLY);
}

GamePad::Coordinate GamePad::XBoxController::RightThumbLocation()
{
	SHORT x = State.Gamepad.sThumbRX;
	SHORT y = State.Gamepad.sThumbRY;
	Coordinate Joysticks(x, y);
	return Joysticks;
}

void GamePad::XBoxController::Vibrate(WORD left, WORD right)
{	
	vibration.wLeftMotorSpeed = left;

	vibration.wRightMotorSpeed = right;

	xinput->XInputSetState(user_ID, &vibration);
}

void GamePad::XBoxController::SetDeadzone(unsigned int radius)
{
	deadzone = radius;
}

GamePad::Coordinate GamePad::XBoxController::Deadzone_LeftThumb(DWORD deadzone)
{
	float LX = State.Gamepad.sThumbLX;
	float LY = State.Gamepad.sThumbLY;
	float magnitudeL = sqrt(LX*LX + LY*LY);
	float length_scaled = (magnitudeL - deadzone) * 32767 / (32767 - deadzone);
	float angle = atan (LY / LX);
	
	//DWORD LX1 = LX / (32767 - deadzone);
	//DWORD LY1 = LY / (32767 - deadzone);

	if (magnitudeL < deadzone) {
		return Coordinate(0,0);
	}
	else {
		return Coordinate(length_scaled*cos(angle), length_scaled*sin(angle));
	}
}

GamePad::Coordinate GamePad::XBoxController::Deadzone_RightThumb(DWORD deadzone)
{
	float RX = State.Gamepad.sThumbRX;
	float RY = State.Gamepad.sThumbRY;
	float magnitudeL = sqrt(RX*RX + RY * RY);
	float length_scaled = (magnitudeL - deadzone) * 32767 / (32767 - deadzone);
	float angle = atan(RY / RX);

	//DWORD LX1 = LX / (32767 - deadzone);
	//DWORD LY1 = LY / (32767 - deadzone);

	if (magnitudeL < deadzone) {
		return Coordinate(0, 0);
	}
	else {
		return Coordinate(length_scaled*cos(angle), length_scaled*sin(angle));
	}
}



	
	