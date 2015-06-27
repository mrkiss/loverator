/*
 Loverator transmitter
*/

#include <Ports.h>
#include <RF12.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <math.h>


#define musicSyncLED 3

#define nodeSwitch1  5
#define nodeSwitch2  6
#define nodeSwitch3  7
#define nodeSwitch4  8

#define RF12_SLEEP 0
#define RF12_WAKEUP -1

#define SEND_FAIL_LIMIT 20

char payload = 'M';
char string[2];
boolean ledOn = false;
int consecutiveSendFailures = 0;

char commandString[10];


//power button variables
unsigned int buttonState = 0;
boolean waitingForSleep = false;
boolean buttonIsDown = false;
boolean shutdownEnabled = false;



void setup() {
  pinMode(nodeSwitch1, INPUT);
  pinMode(nodeSwitch2, INPUT);
  pinMode(nodeSwitch3, INPUT);
  pinMode(nodeSwitch4, INPUT);

  digitalWrite(nodeSwitch1, HIGH);
  digitalWrite(nodeSwitch2, HIGH);
  digitalWrite(nodeSwitch3, HIGH);
  digitalWrite(nodeSwitch4, HIGH);

  pinMode(musicSyncLED, OUTPUT);

  rf12_initialize(4, RF12_433MHZ, 20);
  rf12_control(0xC040); // low battery detect set to 2.1
  startupBlinks();
  Serial.begin(9600);

}


void loop() {

  readCommandString(commandString);

  if (commandString[0] != 0) {

    if (commandString[1] == 's') {
      for (int i = 0; i < 5; i++) {
        musicSyncBroadcast(commandString[0]);
        delay(300);
      }
    } else {
      for (int i = 0; i < 3; i++) {
        musicSyncBroadcast(commandString[0]);
        delay(300);
      }
    }
    
    commandString[0] = 0;
    commandString[1] = 0;

  }

//  if (digitalRead(nodeSwitch1) == LOW) {
//    Serial.println("on1");
//    musicSyncBroadcast('1');
//  }
//  if (digitalRead(nodeSwitch2) == LOW) {
//    Serial.println("on2");
//    musicSyncBroadcast('2');
//  }
//  if (digitalRead(nodeSwitch3) == LOW) {
//    Serial.println("on3");
//    musicSyncBroadcast('3');
//  }
//  if (digitalRead(nodeSwitch4) == LOW) {
//    Serial.println("on4");
//    musicSyncBroadcast('4');
//  }
}


void rampUpLED(int LED) {
  for (int j = 0; j <= 255; j++) {
    analogWrite(LED, j);
    delay(2);
  }
}

void rampDownLED(int LED) {
  for (int j = 255; j >= 0; j--) {
    analogWrite(LED, j);
    delay(2);
  }
}

void startupBlinks() {
  for (int i = 0; i < 2; i++) {
    rampUpLED(musicSyncLED);
    rampDownLED(musicSyncLED);
  }
}


boolean musicSyncBroadcast(char node)
{
  rf12_recvDone();
  boolean canSend = rf12_canSend();
  if (canSend) {
    payload = node;
    rf12_sendStart(0, &payload, sizeof payload);
    consecutiveSendFailures = 0;
  }
  else {
    consecutiveSendFailures++;
  }
  return canSend;
}


void readCommandString(char *strArray) {
  int i = 0;
  char c;
  if (Serial.available()) {
    while (Serial.available()) {
      strArray[i] = Serial.read();
      i++;
    }
  }
}








