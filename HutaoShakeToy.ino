/*    0 Hutao Quick
      1 Hutao slow
      2 shake quick
      3 shake slow
      4 quit quick
      5 quit slow
      6 quit [机械音]
      7 bright quick
      8 bright slow
      9 dark quick
      10 dark slow

                                   "Hutao"
                           ---------------------->    "Shake"       "quit"
      State Machine  :   0                        1 ----------> 2 ---------> 0
                           <----------------------
                                   “quit”

      0||1 : LOW
      2 : HIGH
*/
#include <SoftwareSerial.h>
#include <VoiceRecognitionV3.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

/**
  Connection
  Arduino    VoiceRecognitionModule
   2   ------->     TX
   3   ------->     RX
*/
VR myVR(2, 3); // 2:RX 3:TX, you can choose your favourite pins.

uint8_t records[7]; // save record
uint8_t buf[64];

uint8_t generalState = 0;
uint8_t lightState = 0; // 1 light, 0 dark
uint8_t counter = 0;

uint8_t hutaoRGBVal[3] = {121, 2, 2};
float currentTemperature;
float currentHumidity;
const PROGMEM int fluxHigherLimit = 400;
const PROGMEM int fluxLowerLimit = 360;
#define hutao1Cmd (0)
#define hutao2Cmd (1)
#define shake1Cmd (2)
#define shake2Cmd (3)
#define quit1Cmd (4)
#define quit2Cmd (5)
#define quit3Cmd (6)
#define bright1Cmd (7)
#define bright2Cmd (8)
#define dark1Cmd (9)
#define dark2Cmd (10)

#define initialState (0)
#define intermediateState (1)
#define workState (2)

#define selfCheckSuccessMode (2)
#define selfCheckErrorMode (3)

#define motor 6
#define DHTPIN 12
#define bluePin 8
#define greenPin 9
#define redPin 10
#define DHTTYPE    DHT22
#define I2C_ADDRESS 0x3C
#define RST_PIN -1
SSD1306AsciiAvrI2c oled;
DHT_Unified dht(DHTPIN, DHTTYPE);
/**
  @brief   Print signature, if the character is invisible,
           print hexible value instead.
  @param   buf     --> command length
           len     --> number of parameters
*/
void printSignature(uint8_t *buf, int len)
{
  int i;
  for (i = 0; i < len; i++)
  {
    if (buf[i] > 0x19 && buf[i] < 0x7F)
    {
      Serial.write(buf[i]);
    }
    else
    {
      Serial.print(F("["));
      Serial.print(buf[i], HEX);
      Serial.print(F("]"));
    }
  }
}

/**
  @brief   Print signature, if the character is invisible,
           print hexible value instead.
  @param   buf  -->  VR module return value when voice is recognized.
             buf[0]  -->  Group mode(FF: None Group, 0x8n: User, 0x0n:System
             buf[1]  -->  number of record which is recognized.
             buf[2]  -->  Recognizer index(position) value of the recognized record.
             buf[3]  -->  Signature length
             buf[4]~buf[n] --> Signature
*/
void printVR(uint8_t *buf)
{
  Serial.println(F("VR Index\tGroup\tRecordNum\tSignature"));

  Serial.print(buf[2], DEC);
  Serial.print(F("\t\t"));

  if (buf[0] == 0xFF)
  {
    Serial.print(F("NONE"));
  }
  else if (buf[0] & 0x80)
  {
    Serial.print(F("UG "));
    Serial.print(buf[0] & (~0x80), DEC);
  }
  else
  {
    Serial.print(F("SG "));
    Serial.print(buf[0], DEC);
  }
  Serial.print(F("\t"));

  Serial.print(buf[1], DEC);
  Serial.print(F("\t\t"));
  if (buf[3] > 0)
  {
    printSignature(buf + 4, buf[3]);
  }
  else
  {
    Serial.print(F("NONE"));
  }
  Serial.println(F("\r\n"));
}

void setup()
{
  /** initialize */
  myVR.begin(9600);

  Serial.begin(115200);
  Serial.println(F("HutaoShakeToy"));

  // put your setup code here, to run once:
  // Initialize device.
  dht.begin();
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
  initialDisplay();
  pinMode(motor, OUTPUT);

  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);

  records[0] = hutao1Cmd;
  records[1] = hutao2Cmd;
  records[2] = bright1Cmd;
  records[3] = bright2Cmd;
  records[4] = dark1Cmd;
  records[5] = dark2Cmd;


  if (myVR.clear() == 0)
  {
    Serial.println(F("Recognizer cleared."));
  }
  else
  {
    Serial.println(F("Not find VoiceRecognitionModule."));
    RGBLightColorChange(selfCheckErrorMode);
    delay(1000);
    while (1)
      ;
  }

  if (myVR.load(records, 7) >= 0)
  {
    Serial.println(F("CMDs loaded"));
    RGBLightColorChange(selfCheckSuccessMode);
    delay(1000);
  }

}

uint8_t* RGBLightFluxChange(uint8_t hutaoRGBVal[3], int delta)//Flux
{
  switch (hutaoRGBVal[0])
  {

    case 121:
      if (delta > 0)
      {
        hutaoRGBVal[0] = 201;
        hutaoRGBVal[1] = 3;
        hutaoRGBVal[2] = 3;
      }
      else
      {
        hutaoRGBVal[0] = 40;
        hutaoRGBVal[1] = 1;
        hutaoRGBVal[2] = 1;
      }
      break;
    case 201:
      if (delta <= 0)
      {
        hutaoRGBVal[0] = 121;
        hutaoRGBVal[1] = 2;
        hutaoRGBVal[2] = 2;
      }
      break;
    case 40:
      if (delta > 0)
      {
        hutaoRGBVal[0] = 121;
        hutaoRGBVal[1] = 2;
        hutaoRGBVal[2] = 2;
      }
      break;

    default: //default is the medium light
      hutaoRGBVal[0] = 121;
      hutaoRGBVal[1] = 2;
      hutaoRGBVal[2] = 2;
      break;
  }
  return hutaoRGBVal;
}
void RGBLightColorChange(uint8_t mode)//Color、Behavior
{
  uint8_t RGBVal[3];
  if (analogRead(0) > fluxHigherLimit)
    lightState = 1;
  else if (analogRead(0) < fluxLowerLimit)
    lightState = 0;
  if (mode == 0)
  {
    if (lightState) // dark --> light on
    {
      RGBVal[2] = hutaoRGBVal[2];
      RGBVal[1] = hutaoRGBVal[1];
      RGBVal[0] = hutaoRGBVal[0];
    }
    else // bright --> light off
    {
      RGBVal[2] = 0;
      RGBVal[1] = 0;
      RGBVal[0] = 0;
    }
  }
  else if (mode == 1)
  {

    if (lightState) // dark --> blink
    {
      if (millis() / 200 % 2 == 0)
      {
        RGBVal[2] = hutaoRGBVal[2];
        RGBVal[1] = hutaoRGBVal[1];
        RGBVal[0] = hutaoRGBVal[0];
      }
      else
      {
        RGBVal[2] = 0;
        RGBVal[1] = 0;
        RGBVal[0] = 0;
      }
    }
    else // bright --> light off
    {
      RGBVal[2] = 0;
      RGBVal[1] = 0;
      RGBVal[0] = 0;
    }
  }
  else if (mode == 2)
  {
    RGBVal[2] = 0;
    RGBVal[1] = 255;
    RGBVal[0] = 0;
  }
  else if (mode == 3)
  {
    RGBVal[2] = 0;
    RGBVal[1] = 0;
    RGBVal[0] = 255;
  }
  analogWrite(bluePin, 255 - RGBVal[2]);  // blue 255 - 26
  analogWrite(greenPin, 255 - RGBVal[1]); // green  255 - 26
  analogWrite(redPin, 255 - RGBVal[0]);   // red 255 - 151
}
void printState()
{
  Serial.println((String)generalState + "," + (String)buf[1]);

}
void voiceRecognition()
{
  int ret;
  uint8_t* hutaoRGBValCopy;
  ret = myVR.recognize(buf, 50);
  if (ret > 0)
  {
    switch (generalState)
    { // buf[1]
      case (initialState):
        {
          if (buf[1] == hutao1Cmd or buf[1] == hutao2Cmd)
          {
            generalState = intermediateState;
            myVR.clear();
            records[0] = shake1Cmd;
            records[1] = shake2Cmd;
            records[2] = bright1Cmd;
            records[3] = bright2Cmd;
            records[4] = dark1Cmd;
            records[5] = dark2Cmd;
            myVR.load(records, 7);
          }
          break;
        }

      case (intermediateState):
        {
          if (buf[1] == shake1Cmd or buf[1] == shake2Cmd)
          {
            generalState = workState;
            myVR.clear();
            records[0] = quit1Cmd;
            records[1] = quit2Cmd;
            records[2] = quit3Cmd;
            records[3] = bright1Cmd;
            records[4] = bright2Cmd;
            records[5] = dark1Cmd;
            records[6] = dark2Cmd;
            myVR.load(records, 7);
          }
          break;
        }

      case (workState):
        {
          if (buf[1] == quit1Cmd or buf[1] == quit2Cmd or buf[1] == quit3Cmd)
          { // quit
            generalState = initialState;
            myVR.clear();
            records[0] = hutao1Cmd;
            records[1] = hutao2Cmd;
            records[2] = bright1Cmd;
            records[3] = bright2Cmd;
            records[4] = dark1Cmd;
            records[5] = dark2Cmd;
            myVR.load(records, 7);

          }
          break;
        }
    }

    if (buf[1] == bright1Cmd or buf[1] == bright2Cmd)
    {
      hutaoRGBValCopy = RGBLightFluxChange(hutaoRGBVal, 1);
      hutaoRGBVal[0] = hutaoRGBValCopy[0];
      hutaoRGBVal[1] = hutaoRGBValCopy[1];
      hutaoRGBVal[2] = hutaoRGBValCopy[2];
    }
    else if (buf[1] == dark1Cmd or buf[1] == dark2Cmd)
    {
      hutaoRGBValCopy = RGBLightFluxChange(hutaoRGBVal, -1);
      hutaoRGBVal[0] = hutaoRGBValCopy[0];
      hutaoRGBVal[1] = hutaoRGBValCopy[1];
      hutaoRGBVal[2] = hutaoRGBValCopy[2];
    }
    /** voice recognized */
    printState();
    stateDisplay();
  }
}

void loop()
{
  voiceRecognition();
  switch (generalState)
  {
    case (initialState):
      {
        digitalWrite(motor, LOW);
        RGBLightColorChange(0);
        break;
      }
    case (intermediateState):
      {
        digitalWrite(motor, LOW); // maxCurrent = 20 mA
        RGBLightColorChange(0);
        break;
      }
    case (workState):
      {
        digitalWrite(motor, HIGH);
        // RGB light
        RGBLightColorChange(1);
        break;
      }
    default:
      {
        digitalWrite(motor, LOW);
        RGBLightColorChange(0);
        break;
      }
  }
  counter = counter + 1;
  if ((int)counter == 10)
  {
    stateDisplay();
    counter = 0;
  }


}

void stateDisplay()
{
  // 清除屏幕
  oled.setFont(Callibri11);

  // 设置字体颜色,白色可见
  oled.clear();
  oled.set1X();

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (!isnan(event.temperature)) {
    currentTemperature = event.temperature;
  }
  dht.humidity().getEvent(&event);
  if (!isnan(event.relative_humidity)) {
    currentHumidity = event.relative_humidity;
  }


  //设置光标位置
  oled.print(F("Temperature: "));
  oled.print(currentTemperature);
  oled.println(F("C"));
  oled.print(F("Humidity: "));
  oled.print(currentHumidity);
  oled.println(F("%"));

  oled.print(F("Brightness: "));
  if (hutaoRGBVal[0] == 201)
    oled.println(F("---------"));
  else if (hutaoRGBVal[0] == 121)
    oled.println(F("------"));
  else
    oled.println(F("---"));

  oled.println(F(""));


  if (generalState == initialState)
    oled.println(F("ZZZ"));
  else if (generalState == intermediateState)
    oled.println(F("Hutao Heard you!"));
  else
    oled.println(F("Shake! Shake!"));
}
void initialDisplay()
{
  // 清除屏幕
  oled.setFont(Callibri14);

  // 设置字体颜色,白色可见
  oled.clear();
  oled.set1X();

  oled.println();
  oled.println(("    Hutao Shake Toy"));
  oled.println(("          Welcome!"));
}
