/*
  Hello World.ino
  2013 Copyright (c) Seeed Technology Inc.  All right reserved.

  Author:Loovee
  2013-9-18

  Grove - Serial LCD RGB Backlight demo.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Wire.h>
#include <Servo.h>
#include "rgb_lcd.h"

#define COLOR_R 0
#define COLOR_G 0
#define COLOR_B 255

#define RESISTANCE_PIN A0
#define PHOTO_SENSOR_PIN 2
#define MOTOR_PIN 4

rgb_lcd lcd;
Servo esc;

/* Variance for resistance */
int resistanceVoltage = 0;
int resistanceValue = 0;
int resistanceValueForPrint = 0;

/* Variance for photosensor */
unsigned long mainTimer;
float windSpeedResult = 0.0;
int windSpeedState = true; //스위치의 상태를 저장한다.
int photoSensorCounter = 0; //스위치가 몇 번 눌렸는지 알 수 있는 변수
int tenSecondsCheckCounter = 0;
float ms = 0.0;
float circumference = 0.0;


void setup()
{
  Serial.begin(9600);

  /* LCD setup part */
  lcd.begin(16, 2); // set up the LCD's number of columns and rows:
  lcd.setRGB(COLOR_R, COLOR_G, COLOR_B);

  /* Motor setup part */
  esc.attach(MOTOR_PIN);

  /* Photo Sensor setup part */
  pinMode(PHOTO_SENSOR_PIN, INPUT);         // 스위치의 용도로 쓰기
  digitalWrite(PHOTO_SENSOR_PIN, HIGH);     // 위한 설정 (풀업 저항 가동)
}

void loop()
{
  motorControl();
  readWindSpeed();
  valuePrint();
}

void motorControl()
{
  resistanceVoltage = analogRead(RESISTANCE_PIN);                // bring voltage from potentiometer
  resistanceValue = map(resistanceVoltage, 0, 1023, 65, 165);    // convert signal from 65 to 165 ( reason why is motor responds from 66 - 170 )
  esc.write(resistanceValue);                                    // use mapped value
  resistanceValueForPrint = resistanceValue - 65;                // to make value as 0 - 100
}

void readWindSpeed()
{
  mainTimer = millis() + 100; // 0.1초를 주기로 반복

  while (millis() < mainTimer)
  {
    delay(1); //바운싱 제거를 위한 1ms 지연
    if ((windSpeedState == true) && !digitalRead(PHOTO_SENSOR_PIN)) {
      photoSensorCounter++;
      windSpeedState = false;
    }
    else if ((windSpeedState == false) && digitalRead(PHOTO_SENSOR_PIN)) {
      windSpeedState = true;
    }
  }
  
  windSpeedResult = (6 * photoSensorCounter);  // rpm 으로 보정
  circumference = (2.0 * 22.0 / 7.0 * 0.0333); // 3.4cm날개 반경을 보정 m로 보정

  // 2 x 22/7 (phi) x radian) of the wheel
  // example : the wheel radiant is 10 cm. so the circumference is 2*22/7*10 = 62.86 cm.
  // if the RPM number is 350 which is the length is 62.86*350/minute = 22000 cm or 220 m/minute = 220 m/ 60 second = 3.67 m/s.
  //ms = ((3.1415 * 0.03) * windSpeed ) / 60.0 ;  // circumfrance of the circle in meters : pi * the diameter(반경)
  ms = ((windSpeedResult * circumference) / 60.0 );
}

void valuePrint()
{
  /* Motorpower print part */
  lcd.setCursor(0, 0);    // set the cursor to column 0, line 1
  lcd.print("Motorpower : ");

  if (resistanceValueForPrint < 10) {
    lcd.print("00");
    lcd.print(resistanceValueForPrint);
  }
  else if (resistanceValueForPrint < 100) {
    lcd.print("0");
    lcd.print(resistanceValueForPrint);
  }
  else {
    lcd.print(resistanceValueForPrint);
  }

  /* Photosensor print part*/
  tenSecondsCheckCounter++;

  // after 10 seconds initialize counter
  if ( (tenSecondsCheckCounter % 100) == 0) {
    lcd.setCursor(0, 1);
    lcd.print(photoSensorCounter);
    lcd.print(" times ");
    lcd.print(ms);
    lcd.print(" m/s");

    photoSensorCounter = 0;
    tenSecondsCheckCounter = 0;
  }
}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
