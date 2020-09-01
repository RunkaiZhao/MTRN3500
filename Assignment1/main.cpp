#include <iostream>
#include <stdint.h>
#include "PCM3718.h"
#include "MSIP404.h"
#include "DAC06.h"
#include "EmbeddedOperations.h"
using namespace std;

int main(void) {
	EmbeddedOperations eops;
	EmbeddedDevice::PCM3718 pcm(&eops, 0x300);
	EmbeddedDevice::PCM3718 pcm(&eops, 0x320, -1);
	EmbeddedDevice::MSIP404 msi(&eops, 0x300);
	EmbeddedDevice::DAC06 dac(&eops, 0x310);

	/*dac.analogOutputRaw(0, 4095);
	cout << endl;
	dac.analogOutputRaw(1, 0);
	cout << endl;
	dac.analogOutputVoltage(0, -2);
	cout << endl;
	dac.analogOutputVoltage(1, 6);
	cout << endl;*/

	/*msi.resetChannel(0);
	cout << endl;
	msi.resetChannel(9);
	cout << endl;
	int num = msi.readIndex(0);
	int num3 = msi.readIndex(2);
	cout << endl;
	bool num1 = msi.readChannel(1);
	cout << endl;
	bool num4 = msi.operator!();*/

	uint16_t num1 = pcm.digitalInput();
	cout << endl;
	uint8_t num2 = pcm.digitalByteInput(1);
	cout << endl;
	bool num3 = pcm.digitalBitInput(-1);
	cout << endl;
	pcm.digitalOutput();
	cout << endl;
	pcm.digitalByteOutput(1, 200);
	cout << endl;
	pcm.setRange(0);
	cout << endl;
	//double num4 = pcm.analogInput(1);
	cout << endl;
	cout << pcm;

	return 0;
}

