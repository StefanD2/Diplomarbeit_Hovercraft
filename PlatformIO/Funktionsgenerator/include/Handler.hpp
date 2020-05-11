#ifndef __HANDLER_HPP__
#define __HANDLER_HPP__

#include <inttypes.h>
#include <stdlib.h>
#include <string>
#include "Modes.hpp"

// class to handle modes
class Handler
{
public:
    Handler(Mode **modes, size_t length); // Constructor, timer: pinter of bool with gets set true in timer ruotine, modes: array of modes, length: length of the array of modes

    void init(); // initalizes the SPI
    void loop(); // function to call in loop, handles timer interrupt

    void nextMode();                       // skips to next mode
    void previousMode();                   // skips to previous mode
    std::string getCurrentMode();          // gets ID (=name) of current mode
    bool increaseFrequency();              // increases frequency, false if maximum reached
    bool decreaseFrequency();              // decreases frequency, false if minimum reached
    uint32_t getFrequency();               // gets current frequency (*10)
    bool setOffset(int offset);            // sets  offset, returns true if updated succesfully, otherwise retruns false
    int getOffset();                       // gets current offset
    bool setAmplitude(uint16_t amplitude); // sets amplitude, returns true if updated succesfully,otherwise retruns false
    uint16_t getAmplitude();               // gets current amplidude

    static volatile bool timer; // var which set to ture in timer interrupt

private:
    void setDAC(int value); // writes a 10bit value to the 12bit DAC via SPI
    bool setTimer();        // sets timer to match current frequency, returns true if updated succesfully

    Mode **modes;
    uint16_t amplitude; // max 2047-offset
    int offset;         // max 2047-amplitude
    uint16_t ocr1a;
    size_t length;
    size_t currentMode;
};

#endif /*__HANDLER_HPP__*/