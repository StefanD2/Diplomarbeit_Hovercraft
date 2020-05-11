#ifndef __MENU_HPP__
#define __MENU_HPP__

#include <Handler.hpp>
#include <LiquidCrystal.h>
#include <Label.hpp>
#include <string>

class Menu
{
public:
    Menu(Handler *handler, LiquidCrystal *lcd); // Constructor: handler: handler to configurate, lcd: 16x2 LCD display to show menu, constructor creates static varibales for 4 labels and stores them in labels array

    void init(); // init timer for polling
    void loop(); // funtion to call in loop, handle polling

    static volatile bool buttonUP;    // varibale to set in timer interrupt
    static volatile bool buttonDOWN;  // varibale to set in timer interrupt
    static volatile bool buttonENTER; // varibale to set in timer interrupt

private:
    void show();   // shows current menu status
    void update(); // updates current menu status (only second line)

    Handler *handler;    // handler
    LiquidCrystal *lcd;  // lcd display
    Label **labels;      // array of label, gets initalized with static labels in constructor
    bool main;           // bool if main menu is currently active or not
    size_t currentLabel; // current label which is active (array index for labels)

    bool buttonLUP;    // variable for polling
    bool buttonLDOWN;  // variable for polling
    bool buttonLENTER; // variable for polling
};

#endif /*__MENU_HPP__*/