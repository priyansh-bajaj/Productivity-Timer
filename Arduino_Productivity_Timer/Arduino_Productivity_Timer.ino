/*
Project: Productivity Timer
Author: Priyansh Bajaj
*/

//Arduino library for MAX7219
#include <LedControl.h>

int DataIn = 10;
int CS = 11;
int CLK = 12;
LedControl lc = LedControl(DataIn, CLK, CS, 0);

String serialStringData = "";
int digitalInterval;
bool boltAnimationEnabled = false;

//Hex Dump
byte timerByte[28][4] = {{\
// Numbers 0-9
       0x7C,0x44,0x7C,0x00}, {
       0x00,0x7c,0x00,0x00}, {
       0x74,0x54,0x5c,0x00}, {
       0x54,0x54,0x7C,0x00}, {
       0x1c,0x10,0x7C,0x00}, {
       0x5c,0x54,0x74,0x00}, {
       0x7c,0x54,0x74,0x00}, {
       0x04,0x04,0x7c,0x00}, {
       0x7c,0x54,0x7c,0x00}, {
       0x5c,0x54,0x7c,0x00}, {
// Symbol Cross 
       0x81,0x42,0x24,0x18}, {
       0x18,0x24,0x42,0x81}, {
// Bolt Animation
       0x18,0x00,0x24,0x18}, {
       0x42,0x3c,0x00,0xff}, {
       0x00,0x24,0x18,0x42}, {
       0x3c,0x00,0xff,0x00}, {
       0x24,0x18,0x42,0x3c}, {
       0x00,0xff,0x00,0x3c}, {
       0x18,0x42,0x3c,0x00}, {
       0xff,0x00,0x3c,0x42}, {
       0x42,0x3c,0x00,0xff}, {
       0x00,0x3c,0x42,0x18}, {
       0x3c,0x00,0xff,0x00}, {
       0x3c,0x42,0x18,0x24}, {
       0x00,0xff,0x00,0x3c}, {
       0x42,0x18,0x24,0x00}, {
       0xff,0x00,0x3c,0x42}, {
       0x18,0x24,0x00,0x18}  };

byte addAnalogCount(byte byteToOperate, int ledCount, int index) {
  byteToOperate++;
  for (int i = 8; i >= 0; i--) {
    if (ledCount <= i && index == i) {
      byteToOperate--;
    }
  }
  return byteToOperate;
}

String showBoltAnimation(){
  if (!boltAnimationEnabled) return "success";
  for (int i = 12; i < 28; i+=2){
    for (int j = 0; j < 4; j++) {
      lc.setRow(0, j, timerByte[i][j]);
      lc.setRow(0, j + 4, timerByte[i+1][j]);
    }
    if (i==12){
      delay(150); // extra delay in start of animation
    }
    delay(350);
  }
  for (int i = 26; i > 10; i-=2){
    for (int j = 0; j < 4; j++) {
      lc.setRow(0, j, timerByte[i][j]);
      lc.setRow(0, j + 4, timerByte[i+1][j]);
    }
    delay(350);
  }
  delay(500); // extra delay for end of animation
  lc.clearDisplay(0);
  return "success";
}

void updateTimerMatrix(int num, int count) {
  if (num > 99 || num < 0) {
    //if number invalid
    for (int i = 0; i < 4; i++) {
      // Printing X to display
      lc.setRow(0, i, timerByte[10][i]);
      lc.setRow(0, i + 4, timerByte[11][i]);
    }
  } else {
    //if number valid
    int tensDigit = (num / 10) % 10;
    int onesDigit = num - (tensDigit * 10);
    for (int i = 0; i < 4; i++) {
      lc.setRow(0, i, addAnalogCount(timerByte[tensDigit][i], count, i));
      lc.setRow(0, i + 4, addAnalogCount(timerByte[onesDigit][i], count, i + 4));
    }
  }
}

String digitalStartTimer(int digInterval) {
  //digInterval is timer duration in seconds
  digitalInterval = digInterval + 1; // to compensate while loop decrement
  int analogInterval = digInterval;
  int analogCount = 8; //LEDs that represent remaining time
  while (digitalInterval--) {
    updateTimerMatrix(digitalInterval, analogCount);
    if (analogInterval == digitalInterval) {
      analogInterval = digitalInterval - digInterval / 8;
      analogCount--;
    }
    if (analogCount==0){
      delay(650);
      lc.clearDisplay(0);
      delay(350);
      updateTimerMatrix(digitalInterval, analogCount);
    }
    else
      delay(1000);
    while (Serial.available()) {
      serialStringData = Serial.readString();
      serialStringData.trim();
      Serial.println(serialCommandHandler(serialStringData));
    }
  }
  delay(350);
  lc.clearDisplay(0);
  showBoltAnimation();
  return "success";
}

String digitalStopTimer() {
  digitalInterval = 0;
  lc.clearDisplay(0);
  return "success";
}

String serialCommandHandler(String data) {
  int param = (data.substring(data.indexOf(':')+1)).toInt();
  data = data.substring(0, data.indexOf(':')); // strip param from data
  if (data == "digitalStartTimer") {
    boltAnimationEnabled = false;
    return digitalStartTimer(param);
  }
  else if (data == "digitalStartAnimationTimer") {
    boltAnimationEnabled = true;
    return digitalStartTimer(param);
  }
  else if (data == "digitalStopTimer") {
    return digitalStopTimer();
  }
  return "fail";
}

void setup() {
  lc.shutdown(0, false); // Wakeup Display "0"
  lc.setIntensity(0, 1); // Minimum Brightness
  lc.clearDisplay(0); // Clear Display "0"
  Serial.begin(9600);
}

void loop() {
  delay(1000);
  while (Serial.available()) {
    serialStringData = Serial.readString();
    serialStringData.trim();
    Serial.println(serialCommandHandler(serialStringData));
  }
}
