#ifndef PCM3718_MODULE_H
#define PCM3718_MODULE_H

#include <stdint.h>
#include <sys/io.h>
#include <iostream>

#include "EmbeddedOperations.h"

/*
 * For MTRN3500 Students - These methods and their respective signatures must not be changed. If 
 * they are and human intervention is required then marks will be deducted. You are more than
 * welcome to add private member variables and private methods to the provided classes, or create
 * your own underlying classes to provide the requested functionality as long as all of the
 * functionality for the PCM3718 can be accessed using the methods in the provided classes.
 */

namespace EmbeddedDevice {
  class PCM3718 {
    public:
      PCM3718(EmbeddedOperations *eops, uint32_t base_addr); //enable permission of ports; check if base address of PCM is correct.
      PCM3718(EmbeddedOperations *eops, uint32_t base_addr, uint8_t analog_range); //initialize the default analog range that is +-5v;
      ~PCM3718(); // just print something

      uint16_t digitalInput(); // read in the input from high and low byte, combine them and return it, noted low byte in 303h and high byte in 30B.
							   // return the value is current voltage read from low and high byte.
      uint8_t digitalByteInput(bool high_byte); // 1 = high byte, 0 = low byte; return only single one byte.
      bool digitalBitInput(uint8_t bit); // return the specific bit number from low and hight byte channel. check if bit is correct.
      void digitalOutput(uint16_t value); // 
	  void digitalByteOutput(bool high_byte, uint8_t value);
      void setRange(uint8_t new_analog_range); //should not perform any I\O operation
      double analogInput(uint8_t channel) const;

	  friend std::ostream& operator<<(std::ostream& output, const PCM3718& pcm);

    private:
      // NOTE: All sys/io function calls must be made through the EmbeddedOperations class
      EmbeddedOperations *eops;
	  // my own varibles
	  uint32_t base_addr;
	  uint8_t analog_range;
  };

  // For newer versions of g++, a friend function needs a matching declaration provided
  // in the namespace scope to make it a member of the namespace.
  // You can ignore this line.
  std::ostream& operator<<(std::ostream& output, const PCM3718& pcm);

}


#endif // PCM3718_MODULE_H
