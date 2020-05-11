#include "Label.hpp"

Label::Label(Handler *handler, LiquidCrystal *lcd)
{
    this->handler = handler;
    this->lcd = lcd;
}

void Label::show()
{
    lcd->clear();
    lcd->println(fillChars(getFirstLine()).c_str());
    update();
}

void Label::update()
{
    lcd->setCursor(0, 1);
    lcd->print(fillChars(getSecondLine()).c_str());
}

string Label::fillChars(string in)
{
    string empty = "";
    for (size_t i = 0; i < (16 - in.length()); i++)
    {
        empty = empty.append(" ");
    }
    return in.append(empty);
}

string Label::to_string(int x)
{
    int length = snprintf(NULL, 0, "%d", x);
    char *buf = new char[length + 1];
    snprintf(buf, length + 1, "%d", x);
    std::string str(buf);
    delete[] buf;
    return str;
}

string Label::to_string(int x, size_t digits)
{
    string tmp = to_string(x);
    for (size_t i = tmp.length(); i < digits; i++)
        tmp = "0" + tmp;
    return tmp;
}

LabelWaveform::LabelWaveform(Handler *handler, LiquidCrystal *lcd) : Label(handler, lcd) {}

string LabelWaveform::getFirstLine()
{
    return "WAVEFORM";
}

string LabelWaveform::getSecondLine()
{
    return handler->getCurrentMode();
}

void LabelWaveform::buttonUP()
{
    handler->nextMode();
}

void LabelWaveform::buttonDOWN()
{
    handler->previousMode();
}

LabelFrequency::LabelFrequency(Handler *handler, LiquidCrystal *lcd) : Label(handler, lcd) {}

string LabelFrequency::getFirstLine()
{
    return "FREQUENCY";
}

string LabelFrequency::getSecondLine()
{
    string temp = to_string((int)handler->getFrequency());
    return temp.substr(0, temp.length() - 1) + "," + temp.substr(temp.length() - 1) + " Hz";
}

void LabelFrequency::buttonUP()
{
    handler->increaseFrequency();
}

void LabelFrequency::buttonDOWN()
{
    handler->decreaseFrequency();
}

LabelAmplitude::LabelAmplitude(Handler *handler, LiquidCrystal *lcd) : Label(handler, lcd) {}

string LabelAmplitude::getFirstLine()
{
    return "AMPLITUDE";
}

string LabelAmplitude::getSecondLine()
{
    int amp = (int)(handler->getAmplitude() / 204.70 * 1000.);
    string tmp = to_string(amp, 4);
    if (tmp.size() == 4)
        tmp = " " + tmp;
    tmp = tmp.substr(0, 2) + "," + tmp.substr(2) + " V";
    return tmp;
}
void LabelAmplitude::buttonUP()
{
    handler->setAmplitude(handler->getAmplitude() + 1);
}

void LabelAmplitude::buttonDOWN()
{
    handler->setAmplitude((handler->getAmplitude() - 1 > 0 ? handler->getAmplitude() - 1 : 0));
}

LabelOffset::LabelOffset(Handler *handler, LiquidCrystal *lcd) : Label(handler, lcd) {}

string LabelOffset::getFirstLine()
{
    return "OFFSET";
}

string LabelOffset::getSecondLine()
{
    int off = (int)(handler->getOffset() / 204.70 * 1000.);
    string tmp = to_string(abs(off), 4);
    if (tmp.size() == 4)
        tmp = " " + tmp;
    tmp = (off < 0 ? "-" : " ") + tmp.substr(0, 2) + "," + tmp.substr(2) + " V";
    return tmp;
}
void LabelOffset::buttonUP()
{
    handler->setOffset(handler->getOffset() + 1);
}

void LabelOffset::buttonDOWN()
{
    handler->setOffset(handler->getOffset() - 1);
}