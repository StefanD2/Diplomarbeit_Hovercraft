#include "Modes.hpp"

#include <math.h>

Mode::Mode()
{
    current = 0;
}

void Mode::up()
{
    current++;
    if (current > 359)
        current = 0;
}

int Triangle::get()
{
    return (int)(((current < 179) ? (current - 90) : (90 - (current - 179))) * 2047. / 90.);
}

std::string Triangle::getID()
{
    return "TRIANGLE";
}

int Rectangle::get()
{
    return ((current < 179) ? 2047 : -2047);
}

std::string Rectangle::getID()
{
    return "RECTANGLE";
}

void Sinus::preCalc()
{
    values = (int *)malloc(sizeof(int) * 91);
    for (int i = 0; i < 91; i++)
    {
        values[i] = (int)(2047. * sin(i / 90. * M_PI_2));
    }
}

int Sinus::calc(int degree)
{
    char sign = 1;
    if (degree > 179)
    {
        degree -= 180;
        sign = -1;
    }
    return sign * values[((degree > 89) ? (90 - (degree - 90)) : degree)];
}

int Sinus::get()
{
    return calc(current);
}

std::string Sinus::getID()
{
    return "SINUS";
}
