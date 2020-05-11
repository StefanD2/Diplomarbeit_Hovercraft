#include "Menu.hpp"

#include <avr/interrupt.h>

volatile bool Menu::buttonUP = false;
volatile bool Menu::buttonDOWN = false;
volatile bool Menu::buttonENTER = false;

ISR(TIMER0_COMPA_vect)
{
    char in = PINC;
    Menu::buttonUP = !(in & (1 << 0));
    Menu::buttonENTER = !(in & (1 << 1));
    Menu::buttonDOWN = !(in & (1 << 2));
}
Menu::Menu(Handler *handler, LiquidCrystal *lcd)
{
    this->main = true;
    this->currentLabel = 0;
    this->handler = handler;
    this->lcd = lcd;
    this->buttonLUP = false;
    this->buttonLDOWN = false;
    this->buttonLENTER = false;
    this->labels = (Label **)malloc(sizeof(Label *) * 4);
    static LabelWaveform labelWaveform = LabelWaveform(this->handler, this->lcd);
    static LabelFrequency labelFrequency = LabelFrequency(this->handler, this->lcd);
    static LabelAmplitude labelAmplitude = LabelAmplitude(this->handler, this->lcd);
    static LabelOffset labelOffset = LabelOffset(this->handler, this->lcd);
    static Label *array[] = {&labelWaveform, &labelFrequency, &labelAmplitude, &labelOffset};
    labels = array;
}

void Menu::init()
{
    // init LCD
    lcd->clear();
    lcd->home();
    lcd->display();
    lcd->noAutoscroll();
    lcd->noBlink();
    lcd->noCursor();
    // Timer 0 CTC 1ms
    TCCR0A = 2 << WGM00;
    TCCR0B = 4 << CS00;
    OCR0A = 249;
    TIMSK0 = 1 << OCIE0A;

    // pins als input-pullup setzten
    DDRC = 0;
    PORTC = 0b111;

    show();
}

void Menu::show()
{
    if (main)
    {
        lcd->clear();
        lcd->println("MAIN MENU");
        update();
    }
    else
        labels[currentLabel]->show();
}

void Menu::update()
{
    if (main)
    {
        lcd->setCursor(0, 1);
        lcd->print(Label::fillChars(labels[currentLabel]->getFirstLine()).c_str());
    }
    else
        labels[currentLabel]->update();
}

void Menu::loop()
{
    if (buttonUP != buttonLUP)
    {
        if (buttonUP)
        {
            if (main)
            {
                currentLabel++;
                if (currentLabel >= 4)
                    currentLabel = 0;
            }
            else
                labels[currentLabel]->buttonUP();
            update();
        }
        buttonLUP = buttonUP;
    }
    if (buttonDOWN != buttonLDOWN)
    {
        if (buttonDOWN)
        {
            if (main)
            {
                if (currentLabel == 0)
                    currentLabel = 4;
                currentLabel--;
            }
            else
                labels[currentLabel]->buttonDOWN();
            update();
        }
        buttonLDOWN = buttonDOWN;
    }
    if (buttonENTER != buttonLENTER)
    {
        if (buttonENTER)
        {
            main = !main;
            show();
        }
        buttonLENTER = buttonENTER;
    }
}