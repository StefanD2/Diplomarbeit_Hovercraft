#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_ADS1015.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Adafruit_ADS1115 adcleft = Adafruit_ADS1115(0x48);
Adafruit_ADS1115 adcright = Adafruit_ADS1115(0x49);

#include <mcp2515.h>

#define CAN_ID_CONTROL_MOTORS_SERVOS 0xC0
#define CAN_ID_BATTERY_TEMPS 0xE0
#define SERVO_FREQ 60

#define ADC_ENABLED
//#define SERIAL_DEBUG
/*
Servo 1 Grenzwerte:
163 - 322 - 446
*/

/*
Servo 2 (Mitte) Grenzwerte:
223 - 355 - 490
*/

/*
Servo 3 Grenzwerte:
231 - 355 - 493
*/

struct can_frame canMsg;
struct can_frame cantemps;

MCP2515 mcp2515(7);

unsigned long lastsentadc = 0;

void setup()
{
  Serial.begin(115200);

  pwm.begin();

  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ); // Analog servos run at ~50 Hz updates

  delay(10);
  pwm.setPWM(0, 0, 355);
  pwm.setPWM(4, 0, 355);
  pwm.setPWM(8, 0, 322);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

#ifdef ADC_ENABLED
  adcleft.begin();
  adcleft.setGain(GAIN_FOUR);
  adcright.setGain(GAIN_FOUR);
  adcright.begin();
#endif
  cantemps.can_id = CAN_ID_BATTERY_TEMPS;
  cantemps.can_dlc = 8;
  //canMsg.can_id=CAN_ID_CONTROL_MOTORS_SERVOS;
}

void loop()
{

  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK)
  {
    if ((canMsg.can_id == CAN_ID_CONTROL_MOTORS_SERVOS) && (canMsg.can_dlc == 3))
    {
      int anw = map(canMsg.data[2], 0, 255, -127, 127);
      pwm.setPWM(0, 0, 355 + anw);
      pwm.setPWM(4, 0, 355 + anw);
      pwm.setPWM(8, 0, 322 + anw);
    }
  }
#ifdef ADC_ENABLED
  if (millis() - lastsentadc > 2000)
  {
    int16_t battemps_left[4];
    int16_t battemps_right[4];
    for (int i = 0; i <= 3; i++)
    {
      int16_t c_adc_left, c_adc_right;
      int8_t c_temp_left, c_temp_right;
      c_adc_left = adcleft.readADC_SingleEnded(i);
      c_adc_right = adcright.readADC_SingleEnded(i);
      c_temp_left = (int)map(c_adc_left, 0, 52428, 0, 100);
      c_temp_right = (int)map(c_adc_right, 0, 52428, 0, 100);
#ifdef SERIAL_DEBUG
      Serial.print("CH" + String(i) + ": l:" + c_temp_left + " r:" + c_temp_right + "  ");
#endif
      cantemps.data[i] = c_adc_left & 0xFF;

      cantemps.data[4 + i] = c_adc_right & 0xFF;

      lastsentadc = millis();
    }
#ifdef SERIAL_DEBUG
    Serial.println();
#endif
    mcp2515.sendMessage(&cantemps);
    lastsentadc = millis();
  }
#endif
}
