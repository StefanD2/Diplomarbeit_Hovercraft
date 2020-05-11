#ifndef __LABEL_HPP__
#define __LABEL_HPP__

#include <Handler.hpp>
#include <LiquidCrystal.h>
#include <string>

using namespace std;

// abstract father class for every label
class Label
{
public:
    Label(Handler *handler, LiquidCrystal *lcd); // Constructor
    void show();                                 // shows current label
    void update();                               // shows second line of label

    virtual string getFirstLine() = 0;  // gets frist line of label
    virtual string getSecondLine() = 0; // gets second line of label
    virtual void buttonUP() = 0;        // call on buttonUP pressed
    virtual void buttonDOWN() = 0;      // call on buttonDOWN pressed

    static string fillChars(string in); // fills input string with empty chars to 16 (to override everything in the line)

protected:
    string to_string(int x);                // converts a int to a string
    string to_string(int x, size_t digits); // converts a int to a string with digits digits
    Handler *handler;                       // handler
    LiquidCrystal *lcd;                     // lcd display
};

class LabelWaveform : public Label
{
public:
    LabelWaveform(Handler *handler, LiquidCrystal *lcd); // Constructor, calls Label(handler, lcd)
    string getFirstLine();                               // gets frist line of label
    string getSecondLine();                              // gets second line of label
    void buttonUP();                                     // call on buttonUP pressed
    void buttonDOWN();                                   // call on buttonDOWN pressed
};

class LabelFrequency : public Label
{
public:
    LabelFrequency(Handler *handler, LiquidCrystal *lcd); // Constructor, calls Label(handler, lcd)
    string getFirstLine();                                // gets frist line of label
    string getSecondLine();                               // gets second line of label
    void buttonUP();                                      // call on buttonUP pressed
    void buttonDOWN();                                    // call on buttonDOWN pressed
};

class LabelAmplitude : public Label
{
public:
    LabelAmplitude(Handler *handler, LiquidCrystal *lcd); // Constructor, calls Label(handler, lcd)
    string getFirstLine();                                // gets frist line of label
    string getSecondLine();                               // gets second line of label
    void buttonUP();                                      // call on buttonUP pressed
    void buttonDOWN();                                    // call on buttonDOWN pressed
};

class LabelOffset : public Label
{
public:
    LabelOffset(Handler *handler, LiquidCrystal *lcd); // Constructor, calls Label(handler, lcd)
    string getFirstLine();                             // gets frist line of label
    string getSecondLine();                            // gets second line of label
    void buttonUP();                                   // call on buttonUP pressed
    void buttonDOWN();                                 // call on buttonDOWN pressed
};

#endif /*__LABEL_HPP__*/