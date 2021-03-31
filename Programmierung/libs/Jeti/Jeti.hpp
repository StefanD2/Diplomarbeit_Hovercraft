/*
* Jeti-Library by Stefan Deimel, 23.10.2020
* Jeti.hpp -- includes all needed files
*/
#ifndef __JETI_HPP__
#define __JETI_HPP__

#ifndef JETI_MODE
// if no JETI_MODE is defined, no file can load the definitions
#error define JETI_MODE before including this library
#endif

#include <JetiBase.hpp>

#include <Jeti_UNO.hpp>            // ARDUINO UNO
#include <Jeti_ATmega_Serial1.hpp> // ARDUINO MEGA - Serial 1

#endif /*__JETI_HPP__*/