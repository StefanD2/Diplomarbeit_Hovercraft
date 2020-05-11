#include <inttypes.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <Modes.hpp>
#include <Handler.hpp>
#include <Menu.hpp>

int main()
{
  Triangle dreieck = Triangle();
  Rectangle rechteck = Rectangle();
  Sinus sinus = Sinus();
  Mode *array[] = {&sinus, &rechteck, &dreieck};
  Handler handler(array, 3);
  // init pin modes for LCD
  DDRB |= 0b101;
  DDRD |= 0b11101000;
  LiquidCrystal lcd = LiquidCrystal(8, 10, 3, 5, 6, 7);
  Menu menu = Menu(&handler, &lcd);

  // initialize TIMER 1 (2ms)
/*  TCCR1A = 0x0;
  TCCR1B = 0x4 << WGM10 | 2 << CS10;
  TCCR1C = 0x0;
  OCR1AL = 126;
  OCR1AH = 0;
  TIMSK1 = 1 << OCIE1A;
*/
  lcd.begin(16, 2);

  sinus.preCalc();

  handler.init(); // timer init in

  menu.init();

  sei();

  while (true)
  {
    menu.loop();
    handler.loop();
  }
}
