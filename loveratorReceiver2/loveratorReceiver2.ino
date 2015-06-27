/*
loverator 

 2015.6.27
 */

#include <Ports.h>
#include <RF12.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <math.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))  
#endif


#define motorPin 3
#define SWITCH_PIN 8

#define RF12_SLEEP 0
#define RF12_WAKEUP -1

#define INITIAL_SYNC_PERIOD 1000000 // Duration in microseconds of the blink cycle
#define PERIOD_TOLERANCE (INITIAL_SYNC_PERIOD / 10)

//#define ALONE_PERIOD 300000
//#define HALF_PERIOD 500000 
//#define motorStrongOnDuration 90000 // Duration in microseconds of the LED blink
//#define motorWeakOnDuration 3000
#define PHASE_SHIFT_FACTOR 10 // Strength coefficient of adjustment made when receiving other signals
#define PERIOD_CHANGE_FACTOR 40
#define SEND_FAIL_LIMIT 20
#define MAX_CORRECTION INITIAL_SYNC_PERIOD / 5
//#define sp 3; // pwm speed factor
unsigned int payload = 5;
unsigned long int period = INITIAL_SYNC_PERIOD;

struct filter {
  unsigned long int current;
  unsigned long int sum;
  unsigned long int filtered;
};

unsigned long motorTriggerTime = 0;
unsigned long lastReportTime = 0;

boolean motorStrongOn = false;
boolean motorWeakOn = false;

int consecutiveSendFailures = 0;


filter peers;
//filter coherence;



//power button variables
unsigned int buttonState = 0;
boolean waitingForSleep = false;
boolean buttonIsDown = false;
boolean shutdownEnabled = false;




void rampUpLED(){ 
  for (int j = 0; j <= 255; j++){
    analogWrite(motorPin, j);
    delay(2);
  }  
}

void rampDownLED(){
  for (int j = 255; j >= 0; j--){
    analogWrite(motorPin, j);
    delay(2);
  }  

}

void startupBlinks(){
  for (int i = 0; i< 3; i++){
    rampUpLED();
    rampDownLED();
  }

}

void setup() {     
  MCUSR = 0;
  wdt_enable(WDTO_8S); // enable the watchdog - for safety!
  pinMode(SWITCH_PIN,INPUT);
  digitalWrite(SWITCH_PIN, HIGH);
  sbi(GIMSK,PCIE0); // Enable PCI
  sbi(PCMSK0,PCINT2); // Set PCInt2 as the PCI pin  

 
  pinMode(motorPin, OUTPUT);
  rf12_initialize(4, RF12_433MHZ, 20);
  rf12_control(0xC040); // low battery detect set to 2.1
  startupBlinks();
 // ledTriggerTime = micros();

}

/** Gets the raw (un-debounced) value from the button.
 Returns 1 if button is pressed, 0 otherwise. **/
unsigned int getRawButtonValue(){
  return digitalRead( SWITCH_PIN) == LOW;
}

/** Non-standard things to do before sleeping (e.g. shut off peripherals, etc.)
 */
void getReadyForBed(){
  waitingForSleep = false;
  rampDownLED();
  rf12_sleep(RF12_SLEEP);
  digitalWrite(motorPin, LOW);
  MCUSR = 0;
  wdt_disable();
}

/** Non-standard things to do after awakening (e.g. turn on peripherals, etc.)
 */
void startTheDay(){

  reboot(); // do a reset
  //rf12_sleep(RF12_WAKEUP);

  //digitalWrite(SWITCH_PIN, HIGH);
  //digitalWrite(LED_PIN, HIGH);
}

void reboot(){
  MCUSR = 0;
  wdt_enable(WDTO_15MS);
  for (;;){
  } // INFINNITE LOOP, causes watchdog restart.
}


void goToSleep() {
  getReadyForBed();
  cbi(ADCSRA,ADEN); // Turn off ADC

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Go to sleep
  sleep_mode(); // Sleep starts after this line
  //sleep_cpu();

  sbi(ADCSRA,ADEN);  // Turn on ADC
  startTheDay();
}

void checkPowerToggle(){
  unsigned int currentValue = getRawButtonValue();
  buttonState <<= 1;
  buttonState |= currentValue;

  buttonIsDown = (buttonState == 0xffff);
  shutdownEnabled = shutdownEnabled | (buttonState == 0); 
  if (buttonIsDown && shutdownEnabled){
    waitingForSleep = true;

  } 
  else {

    if (waitingForSleep){
      shutdownEnabled = false;
      waitingForSleep = false;
      goToSleep();

      shutdownEnabled = false;
      waitingForSleep = false;
    } 
  }  
}



boolean listen(){
  boolean received = rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof payload;  //-disable CRC and len check - noise is ok.
  if (received) lastReportTime = micros();
  return received;
  //return rf12_recvDone();
  //return false;
}

boolean broadcast(){
  boolean canSend = rf12_canSend();
  if (canSend){
    rf12_sendStart(0, &payload, sizeof payload);
    consecutiveSendFailures = 0;
  }
  else {
    consecutiveSendFailures++;  
  }
  return canSend;
}

void updateFilter(struct filter *value, unsigned int sampleCount){
  value->sum += value->current - value->filtered;
  value->filtered = value->sum / sampleCount;
  value->current = 0;
}

void loop() {
  checkPowerToggle();
  /*if (micros() - lastReportTime > 5000000) period = ALONE_PERIOD;
   else period = SYNC_PERIOD;
   */


  if (rf12_recvDone() && rf12_crc == 0) 
  {
    if(rf12_data[0] == '4')
    {
        digitalWrite(motorPin, HIGH);
        delay(200);
    }
    else 
    {
      digitalWrite(motorPin, LOW);

    }

  }
   else 
    {
      digitalWrite(motorPin, LOW);

    }
  wdt_reset();
}

ISR(PCINT0_vect) {
} //no-op interrupt




