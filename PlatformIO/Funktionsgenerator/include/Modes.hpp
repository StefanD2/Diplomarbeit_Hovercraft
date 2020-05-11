#ifndef __MODES_HPP__
#define __MODES_HPP__

#include <inttypes.h>
#include <stdlib.h>
#include <string>

// abstract father class for every mode
class Mode
{
public:
    Mode();                // Constructor, sets varibale current to 0
    void up();             // handles upcounting of variable current
    virtual int get() = 0; // should return a value in range of -2047 to +2047, needs to be overrind in child class

    virtual std::string getID() = 0; // gets the ID (=Name) of the Mode

protected:
    int current; // current t in period, range: 0 - 359, gets upcounted in up()
};

// class for triangle signal, child class of Mode
class Triangle : public Mode
{
public:
    int get(); // returns a value in range of -2047 to +2047, depending on value of current

    std::string getID(); // gets the ID (=Name) of the Mode
};

// class for rectangle signal, child class of Mode
class Rectangle : public Mode
{
public:
    int get(); // returns a value in range of -2047 to +2047, depending on value of current

    std::string getID(); // gets the ID (=Name) of the Mode
};

// class for sinus signal, child class of Mode
class Sinus : public Mode
{
public:
    void preCalc(); // pre-alculates sinus values and stores them in array values (to get nessercery speed)
    int get();      // returns a value in range of -2047 to +2047, depending on value of current, needs preCalc() called befor to function correctly

    std::string getID(); // gets the ID (=Name) of the Mode

private:
    int calc(int degree); // returns value of sinus (in dgrees), uses pre-calculated values stroed in array values
    int *values;          // stores pre-calculculated values
};

#endif /*__MODES_HPP__*/