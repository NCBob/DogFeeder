// Dog Feeder for Henry Heidtmann
// Uses a relay to control a motor that deposits food, and a
// RTDC module from SainSmart to keep track of the time
// and an LCD display for status updates.
#include <Wire.h>
//#include "Time.h"
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>

const int motorPin = 3;
const int feedButtonPin = 8;
const int upButtonPin = 10;
const int downButtonPin = 11;
const int rightButtonPin = 12;
const int leftButtonPin = 9;
int motorSpeed = 100;
int motorRunTime = 2.9;
int motorNumberOfTurns = 4;
int feedButtonState = HIGH;
int upButtonState = HIGH;
int downButtonState = HIGH;
int leftButtonState = HIGH;
int rightButtonState = HIGH;
int lastButtonState = LOW;
int lastSecond = 0;
RTC_DS1307 rtc;
String feederTime1 = "07:00:00";
String feederTime2 = "18:00:00";
//String feederTime1;
//String feederTime2;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup() {
    pinMode(motorPin, OUTPUT);
    pinMode(feedButtonPin, INPUT_PULLUP);
    pinMode(upButtonPin, INPUT_PULLUP);
    pinMode(downButtonPin, INPUT_PULLUP);
    pinMode(leftButtonPin, INPUT_PULLUP);
    pinMode(rightButtonPin, INPUT_PULLUP);
    digitalWrite(motorPin, LOW);
    
    Serial.begin(9600);
    Wire.begin();
    rtc.begin();

    lcd.begin(20,4);
    for(int i = 0; i < 3; i++)
    {
        lcd.backlight();
        delay(250);
        lcd.noBacklight();
        delay(250);
    }
    lcd.backlight();
    SetTime();
}

void loop() {
    feedButtonState = !digitalRead(feedButtonPin);
    upButtonState = !digitalRead(upButtonPin);
    downButtonState = !digitalRead(downButtonPin);
    leftButtonState = !digitalRead(leftButtonPin);
    rightButtonState = !digitalRead(rightButtonPin);
    
    if(lastButtonState != feedButtonState)
        runFeederMotor();
    
    if(lastSecond != rtc.now().second()){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Time: " + GetTime());
        lcd.setCursor(0,1);
        lcd.print("Feed 1: " + feederTime1);
        lcd.setCursor(0,2);
        lcd.print("Feed 2: " + feederTime2);
        lastSecond = rtc.now().second();
        if(feederTime1.equals(GetTime()) || feederTime2.equals(GetTime()))
            runFeederMotor();
    }
    delay(100);
}

void runFeederMotor()
{
    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,1);
    lcd.print("Feeding Time!");
    analogWrite(motorPin, motorSpeed);
    delay(motorRunTime*1000 * motorNumberOfTurns);
    analogWrite(motorPin, 0);
    lcd.setCursor(0,1);
    lcd.print("                  ");
}

String GetTime()
{
    String timeString;
    DateTime now = rtc.now();
    
    timeString += PadValue(now.hour());
    timeString += ":";
    timeString += PadValue(now.minute());
    timeString += ":";
    timeString += PadValue(now.second());
    return timeString;
}

void SetTime()
{
    int hourValue = rtc.now().hour();
    int minuteValue = rtc.now().minute();
    int secondValue = rtc.now().second();
    boolean setTimeComplete = false;
    boolean displayCursor = false;
    boolean buttonPress = false;
    int cursor = 0;
    String timeString;
    String pressedButton;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set the time!");
    timeString = PadValue(hourValue);
    timeString += ":";
    timeString += PadValue(minuteValue);
    timeString += ":";
    timeString += PadValue(secondValue);
    int lastSecond = rtc.now().second();

    lcd.setCursor(0,1);
    lcd.print(timeString);
    
    while(!setTimeComplete){
        Serial.println(pressedButton);
        pressedButton = GetButtonPress();
        if(displayCursor && pressedButton == "")
        {
            lcd.setCursor(cursor,1);
            lcd.print("__");
        }
        else
        {
            lcd.setCursor(cursor,1);
            switch (cursor){
                case 0:
                    if(pressedButton == "UP" && hourValue < 23)
                    {
                        hourValue += 1;
                    }
                    else if (pressedButton == "DOWN" && hourValue > 1)
                    {
                        hourValue -= 1;
                    }
                    lcd.print(PadValue(hourValue));
                    break;
                case 3:
                    if(pressedButton == "UP" && minuteValue < 59)
                    {
                        minuteValue += 1;
                    }
                    else if (pressedButton == "DOWN" && minuteValue > 1)
                    {
                        minuteValue -= 1;
                    }
                    lcd.print(PadValue(minuteValue));
                    break;
                case 6:
                    if(pressedButton == "UP" && secondValue < 59)
                    {
                        secondValue += 1;
                    }
                    else if (pressedButton == "DOWN" && secondValue > 1)
                    {
                        secondValue -= 1;
                    }
                    lcd.print(PadValue(secondValue));
                    break;
            }
        }
        if(lastSecond != rtc.now().second())
        {
          lastSecond = rtc.now().second();
          displayCursor = !displayCursor;
        }
        if(pressedButton == "RIGHT" && cursor < 6)
        {
            cursor += 3;
        }
        else if (pressedButton == "LEFT" && cursor > 0)
        {
            cursor -= 3;
        }
        if(pressedButton == "FEED")
        {
            rtc.adjust(DateTime(2015,1,1,hourValue,minuteValue,secondValue));
            setTimeComplete == true;
            return;
        }
    }
}

String PadValue(int value)
{
    String returnVal;
    
    if(value < 10){
        returnVal = "0";
        returnVal += value;
    }
    else
    {
        returnVal = String(value);
    }
    return returnVal;
}

String GetButtonPress()
{
    feedButtonState = !digitalRead(feedButtonPin);
    upButtonState = !digitalRead(upButtonPin);
    downButtonState = !digitalRead(downButtonPin);
    leftButtonState = !digitalRead(leftButtonPin);
    rightButtonState = !digitalRead(rightButtonPin);
    Serial.println("Checking buttons");
    if(upButtonState){
      delay(250);
        return "UP";
    }
    else if(downButtonState){
      delay(250);  
      return "DOWN";
    }
    else if(leftButtonState){
      delay(500);
      return "LEFT";
    }
    else if(rightButtonState){
      delay(500);
      return "RIGHT";
    } 
    else if(feedButtonState)
    {
      delay(1000);
        return "FEED";
    }
    else
    {
        return "";
    }
}
