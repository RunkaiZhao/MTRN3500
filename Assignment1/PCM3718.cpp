#include "PCM3718.h"
#include <iostream>

EmbeddedDevice::PCM3718::PCM3718(EmbeddedOperations* eops, uint32_t base_addr)
{
	if (base_addr != 0x300) {
		std::cout << "The base address of PCM3718 passed in is wrong!!!The adress has been changed to correct one." << std::endl;
		this->base_addr = 0x300; 
	}
	else {
		this->base_addr = base_addr;
	}
	
	if (eops->ioperm(base_addr, 16, 1) != 0) {
		std::cout << "permission failed!" << std::endl;
	}

	this->analog_range = 0; // Range code of 0b0000 means +-5v, which is default.

	this->eops = eops;
}

EmbeddedDevice::PCM3718::PCM3718(EmbeddedOperations* eops, uint32_t base_addr, uint8_t analog_range)
{
	if (base_addr != 0x300) {
		std::cout << "The base address of PCM3718 passed in is wrong!!!The adress has been changed to correct one." << std::endl;
		this->base_addr = 0x300;
	}
	else {
		this->base_addr = base_addr;
	} // check base address

	if ((analog_range < 0) || (analog_range > 11)) {
		std::cout << "please re-pass in a correct range code. The default range is +-5v" << std::endl;
		this->analog_range = 0;
	} // check analog range. if wrong, set it to default rangde code.

	if (eops->ioperm(this->base_addr, 16, 1) != 0) {
		std::cout << "permission failed!" << std::endl;
	}

	this->analog_range = analog_range;
	this->eops = eops;
}

EmbeddedDevice::PCM3718::~PCM3718()
{	
}

uint16_t EmbeddedDevice::PCM3718::digitalInput()
{	
	int lowByte = 0, highByte = 0;
	lowByte = eops->inb(this->base_addr + 0x03);
	highByte = eops->inb(this->base_addr + 0x0B);

	return ((highByte << 8) | (lowByte)); // return the combined 16-bit value;
}

uint8_t EmbeddedDevice::PCM3718::digitalByteInput(bool high_byte)
{	
	int lowByte = 0, highByte = 0;
	lowByte = eops->inb(this->base_addr + 0x03);
	highByte = eops->inb(this->base_addr + 0x0B);

	if (high_byte == 1) {
		return highByte;
	}
	else if (high_byte == 0) {
		return lowByte;
	}

	return 0;
}

bool EmbeddedDevice::PCM3718::digitalBitInput(uint8_t bit)
{
	if ((bit > 15) || (bit < 0)) {
		std::cout << "please re-pass in correct value that is between 0-15." << std::endl;
		return 0;
	}

	int lowByte = 0, highByte = 0, _16BiteValue = 0;
	lowByte = eops->inb(this->base_addr + 0x03);
	highByte = eops->inb(this->base_addr + 0x0B);
	_16BiteValue = (highByte << 8) | (lowByte); // return the combined 16-bit value;

	int BitValue = (_16BiteValue & (1 << bit)); // left shift *bit place
	return BitValue;
}

void EmbeddedDevice::PCM3718::digitalOutput(uint16_t value)
{
	/*if ((value < 0) || (value > 65535)) {
		std::cout << "please re-pass in correct value that is between 0-65535." << std::endl;
		//return;
	} // the argument of value is unsigned 16-bit value, so the range of it is 0-65535*/

	int _lowByte = 0, _highByte = 0;
	_lowByte = value & 0xFF; // this is like 0b00000000xxxxxxxx.
	_highByte = value & 0xFF00; // this value should be like 0bxxxxxxxx00000000, then shift it right by 8 places.
	_highByte = _highByte >> 8;

	eops->outb(_lowByte, this->base_addr + 0x03);
	eops->outb(_highByte, this->base_addr + 0x0B);

	std::cout << "digital Output is called." << std::endl;
}

void EmbeddedDevice::PCM3718::digitalByteOutput(bool high_byte, uint8_t value)
{
	if (high_byte == 0) {
		eops->outb(value, this->base_addr + 0x03);
	}
	else if (high_byte == 1) {
		eops->outb(value, this->base_addr + 0x0B);
	}
}

void EmbeddedDevice::PCM3718::setRange(uint8_t new_analog_range)
{
	this->analog_range = new_analog_range;
}

double EmbeddedDevice::PCM3718::analogInput(uint8_t channel) const
{
	//select channel 0-15
	if ((channel < 0) || (channel > 15)) {
		std::cout << "out of the range!!!" << std::endl;
	}

	eops->outb((channel | (channel << 4)), this->base_addr + 2);

	//set range
	eops->outb(this->analog_range, this->base_addr + 0x01);
	usleep(100);

	//trigger conversion
	eops->outb(0x00, this->base_addr);

	//waiting some time to complete conversion
	int i = 0;
	while (eops->inb(this->base_addr + 0x08) & 0x80) { i++; }

	//read 12-bit converted number
	uint16_t _12BitValue = 0;
	uint8_t LoByte = 0, HiByte = 0;
	LoByte = eops->inb(this->base_addr + 0x00);
	HiByte = eops->inb(this->base_addr + 0x01);
	_12BitValue = ((LoByte >> 4) | (HiByte << 4));

	//convert 12-bit number to a voltage between +-5v
	double min = 0;
	double LengthOfRange = pow(10.0, 1 - (this->analog_range & 0x03)); // = 10^(1-i), i is depending on lsb and 2nd lsb of 4-bit range code.

	//figure out what value of min in range
	//unipolar or bipolar is dependent on 2nd msb of range code
	if ((this->analog_range & 0x04) == 1) {
		min = 0;
	}
	else if ((this->analog_range & 0x04) == 0) {
		min = -LengthOfRange / 2;
	}

	double AnalogInput = (static_cast<double>(_12BitValue) / 4095) * LengthOfRange + min;
	return AnalogInput;
}

std::ostream& EmbeddedDevice::operator<<(std::ostream& output, const PCM3718& pcm)
{
	
	output << "channel 0: " << pcm.analogInput(0) << "\t" << "channel 1: " << pcm.analogInput(1) << std::endl;

	return output;
}
