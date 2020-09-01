#include "DAC06.h"
#include <iostream>

EmbeddedDevice::DAC06::DAC06(EmbeddedOperations* eops, uint32_t base_addr)
{
	this->eops = eops;

	if (base_addr != 0x320) {
		std::cout << "WRONG ADDRESS!" << std::endl;
		this->base_addr = 0x320;
	} //check if the input address is correct
	else {
		this->base_addr = base_addr;
	}

	if (eops->ioperm(this->base_addr, 12, 1) != 0)
	{
		std::cout << "permission failed." << std::endl;
	}
}

EmbeddedDevice::DAC06::~DAC06()
{
}

void EmbeddedDevice::DAC06::analogOutputRaw(uint8_t channel, uint16_t value)
{
	uint8_t highByte = 0, lowByte = 0;
	lowByte = value & 0xFF;
	value = (value >> 8);
	highByte = value & 0x0F;
	
	eops->outb(lowByte, this->base_addr + channel * 2);
	eops->outb(highByte, this->base_addr + 1 + channel * 2);
}

void EmbeddedDevice::DAC06::analogOutputVoltage(uint8_t channel, double desired_voltage)
{	
	if ((desired_voltage < -5) || (desired_voltage > 5)) {
		std::cout << "PLEASE TYPE IN VOLTAGE BETWEEN -5 TO 5V!" << std::endl;
	}
	uint16_t valueConverted = static_cast<uint16_t>(((desired_voltage + 5) / 10) * 4095); // 12bits number
	this->analogOutputRaw(channel, valueConverted);
	/*uint8_t _highByte = 0, _lowByte = 0;
	_lowByte = valueConverted & 0xFF;
	valueConverted = (valueConverted >> 8);
	_highByte = valueConverted & 0x0F;

	eops->outb(_lowByte, this->base_addr + channel * 2);
	eops->outb(_highByte, this->base_addr + 1 + channel * 2);*/
}
