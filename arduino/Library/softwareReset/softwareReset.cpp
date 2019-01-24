#include <Arduino.h>
#include <avr/wdt.h>
#include <softwareReset_.h>


softwareReset_::softwareReset_()
{
	  wdt_enable(prescaller);
	  while(1) {};

}

softwareReset_::reset()
{

}