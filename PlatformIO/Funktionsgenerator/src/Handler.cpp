#include "Handler.hpp"

#include <avr/interrupt.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <../lib/SPI/SPI.h>
#ifdef __cplusplus
}
#endif

volatile bool Handler::timer = true;

ISR(TIMER1_COMPA_vect)
{
    Handler::timer = true;
}

Handler::Handler(Mode **modes, size_t length)
{
    this->modes = (Mode **)malloc(sizeof(Mode **) * length);
    this->modes = modes;
    this->length = length;
    this->currentMode = 0;
    this->offset = 0;
    this->amplitude = 2047;
    this->ocr1a = (uint16_t)(1./360./(8./F_CPU)/50.) -1;
}

void Handler::init()
{
    SPI_init();
    // Timer1 init
    TCCR1A = 0x0;
    TCCR1B = 1 << WGM12 | 2 << CS10; // 1/8 Prescaler CTC
    TCCR1C = 0x0;
    TIMSK1 = 1 << OCIE1A;
    setTimer();
}

void Handler::loop()
{
    if (timer)
    {
        timer = false;
        modes[currentMode]->up();
        setDAC((int)(modes[currentMode]->get() / 2047. * amplitude + offset + 2047.));
    }
}

void Handler::nextMode()
{
    currentMode++;
    if (currentMode >= length)
    {
        currentMode = 0;
    }
}

void Handler::previousMode()
{
    if (currentMode == 0)
    {
        currentMode = length;
    }
    currentMode--;
}

std::string Handler::getCurrentMode()
{
    return modes[currentMode]->getID();
}

bool Handler::increaseFrequency()
{
    ocr1a--;
    if(ocr1a < 20){
        ocr1a = 20;
        return false;
    }
    return setTimer();
}

bool Handler::decreaseFrequency()
{
    ocr1a++;
    if(ocr1a > 5554){
        ocr1a = 5554;
        return false;
    }
    return setTimer();
}


uint32_t Handler::getFrequency()
{
    return (uint32_t)(1./(8./(F_CPU)*(ocr1a+1)*360.)*10.);
}

bool Handler::setOffset(int offset)
{
    if (this->offset != offset)
    {
        if (2047 - amplitude < abs(offset))
        {
            return false;
        }
        this->offset = offset;
        return true;
    }
    return true;
}

int Handler::getOffset()
{
    return offset;
}

bool Handler::setAmplitude(uint16_t amplitude)
{
    if (this->amplitude != amplitude)
    {
        if (2047 - (uint16_t)abs(offset) < amplitude || amplitude < 0)
        {
            return false;
        }
        this->amplitude = amplitude;
        return true;
    }
    return true;
}

uint16_t Handler::getAmplitude()
{
    return amplitude;
}

void Handler::setDAC(int value)
{
    SPI_DAC_out(SPI_OUT_A, value);
}

bool Handler::setTimer()
{
    // timer changes on writing low byte
    OCR1AH = ocr1a>>8;
    OCR1AL = ocr1a;
    return false;
}
