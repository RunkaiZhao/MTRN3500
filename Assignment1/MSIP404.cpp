#include "MSIP404.h"
#include <iostream>
//#define BASE_encoder 0x200

EmbeddedDevice::MSIP404::MSIP404(EmbeddedOperations* eops, uint32_t base_addr)
{
	this->eops = eops;
	
	if (base_addr != 0x200) {
		std::cout << "WRONG ADDRESS!" << std::endl;
		this->base_addr = 0x200;
	} //check if the input address is correct
	else {
		this->base_addr = base_addr;
	}

	if (eops->ioperm(this->base_addr, 32, 1) != 0)
	{
		std::cout << "encoder permission failed." << std::endl;
	}

	if (eops->ioperm(this->base_addr_index, 1, 1) != 0) {
		std::cout << "index pulse permission failed." << std::endl;
	}
}

EmbeddedDevice::MSIP404::~MSIP404()
{
}

void EmbeddedDevice::MSIP404::resetChannel(uint8_t channel)
{
	if ((channel < 0) || (channel > 7)) {
		std::cout << "WRONG CHANNEL NUMBER!" << std::endl;
	}
	eops->outb(0x00, this->base_addr + channel);
}

uint32_t EmbeddedDevice::MSIP404::readChannel(uint8_t channel)
{
	/*uint32_t readValue = 0;
	int32_t channelMSB = 0;
	int32_t channel2ndMSB = 0;
	int32_t channel2ndLSB = 0;
	int32_t channelLSB = 0;

	//base_addr = base_addr + 0x0;
	channelMSB = eops->inb(this->base_addr + channel * 4 + 0x03);
	channel2ndMSB = eops->inb(this->base_addr + channel * 4 + 0x02);
	channel2ndLSB = eops->inb(this->base_addr + channel * 4 + 0x01);
	channelLSB = eops->inb(this->base_addr + channel * 4);

	channelMSB = channelMSB << 24;
	channel2ndMSB = channel2ndMSB << 16;
	channel2ndLSB = channel2ndLSB << 8;
	channelLSB = channelLSB;
	readValue = (channelMSB | channel2ndMSB | channel2ndLSB | channelLSB);*/

	//make an union for read-in encoder value
	union Encoder {
		uint32_t i;
		char c[4];
	} e;

	int j = 0;
	for (j = 3; j >= 0; j--) {
		e.c[j] = eops->inb(this->base_addr + channel * 4 + j);
	}
	return e.i;
}

bool EmbeddedDevice::MSIP404::readIndex(uint8_t channel)
{	
	//this->base_addr_index = 0x800;
	if (channel == 0)
	{
		return (eops->inb(this->base_addr_index) & 0x80);
	}
	else if (channel == 1)
	{
		return (eops->inb(this->base_addr_index) & 0x20);
	}
	else if (channel == 2)
	{
		return ((eops->inb(this->base_addr_index + 0x01)) & 0x80);
	}

	return 0;
}

bool EmbeddedDevice::MSIP404::operator!()
{
	for (int i = 0; i < 8; i++)
	{
		eops->outb(0x00, this->base_addr + i);
	}

	return 1;
}