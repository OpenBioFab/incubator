// Original from: http://www.instructables.com/id/Low-cost-and-accurate-incubator-for-DIY-biology/

// Some parts of our code are based on Arduino’s 
// AC phase controlling tutorial and 
// we re-used some part of that code as it is including comments.


// AC Phase control is accomplished using the internal 
// hardware timer1 in the arduino
//
// Timing Sequence
// * timer is set up but disabled
// * zero crossing detected on pin 2
// * timer starts counting from zero
// * comparator set to "delay to on" value
// * counter reaches comparator value
// * comparator ISR turns on triac gate
// * counter set to overflow - pulse width
// * counter reaches overflow
// * overflow ISR truns off triac gate
// * triac stops conducting at next zero cross


// The hardware timer runs at 16MHz. Using a
// divide by 256 on the counter each count is 
// 16 microseconds.  1/2 wave of a 60Hz AC signal
// is about 520 counts (8,333 microseconds).#include <LiquidCrystal.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <SPI.h>
#include <PID_v1.h>
#include "SoftwareSerial.h"
#include <LiquidCrystal.h>
#include <LCDKeypad.h>
#include <Wire.h>
#include "max6675.h"


#define DETECT 2  //zero cross detect
#define GATE 3    //triac gate
#define PULSE 4   //trigger pulse width (counts)

// uses pins 4,5,6,7,8,9 | 10 Backlight on/off | pin 0 to read the buttons
LCDKeypad lcd;
// thermocoupler pins
int ktcSO = 11;
int ktcCS = 12;
int ktcCLK = 13;
MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

int i;
double currentTemperature;
//Define Variables we'll be connecting to
double setPoint;
int setPoint_temp;
double Input;
double Output;
//Specify the links and initial tuning parameters
double Kp = 2;
double Ki = 6; 
double Kd = 1;
PID myPID(&Input, &Output, &setPoint, Kp, Ki, Kd, DIRECT);

void setup() {
  Serial.begin(9600);

  pinMode(DETECT, INPUT);     //zero cross detect
  digitalWrite(DETECT, HIGH); //enable pull-up resistor
  pinMode(GATE, OUTPUT);      //triac gate control
  setPoint = 35;     // set the default temperature as the setPoint
  setPoint_temp = setPoint;

  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 449);
  
  Wire.begin();
  lcd.begin(16, 2);
  lcd.clear();
  setDefaultLCD();

  // set up Timer1 
  //(see ATMEGA 328 data sheet pg 134 for more details)
  OCR1A = 100;      //initialize the comparator
  TIMSK1 = 0x03;    //enable comparator A and overflow interrupts
  TCCR1A = 0x00;    //timer control registers set for
  TCCR1B = 0x00;    //normal operation, timer disabled

  // set up zero crossing interrupt
  attachInterrupt(0, zeroCrossingInterrupt, RISING);    
  //IRQ0 is pin 2. Call zeroCrossingInterrupt 
  //on rising signal
  
  // give the MAX a little time to settle
  // delay(500);
}

// Interrupt Service Routines
void zeroCrossingInterrupt() { //zero cross detect   
  TCCR1B = 0x04; //start timer with divide by 256 input
  TCNT1 = 0;   //reset timer - count from zero
}

ISR(TIMER1_COMPA_vect) { //comparator match
  digitalWrite(GATE, HIGH);  //set triac gate to high
  TCNT1 = 65536 - PULSE;      //trigger pulse width
}

ISR(TIMER1_OVF_vect) { //timer1 overflow
  digitalWrite(GATE, LOW); //turn off triac gate
  TCCR1B = 0x00;          //disable timer stopd unintended triggers
}

void loop() {
  listenButtons();
  double c = ktc.readCelsius();

  if (isnan(c)) {
    Serial.println("Something wrong with thermocouple!");
    i = 450;
  } else {
    currentTemperature = c;
    displayCurrentTemperature();
    Serial.print("C = "); 
    Serial.print(c);
     
    Input = c;
    myPID.Compute();
    Serial.print(Output);
    i = 450 - Output; 
  }
      
  Serial.print(" i=");
  Serial.println(i);
  
  OCR1A = i;  //set the compare register brightness desired.
  delay(400);
}

void listenButtons() {
  switch (lcd.buttonBlocking()) {
    case KEYPAD_LEFT:
      setPoint_temp--;
      setPoint = setPoint_temp;
      lcd.setCursor(10, 1); 
      lcd.print(setPoint);
      break;
      
    case KEYPAD_RIGHT:
      setPoint_temp++;
      setPoint = setPoint_temp;
      lcd.setCursor(10, 1); 
      lcd.print(setPoint);
      break;
      
    case KEYPAD_DOWN:
      setPoint_temp--;
      setPoint = setPoint_temp;
      lcd.setCursor(10, 1); 
      lcd.print(setPoint);
      break;
      
    case KEYPAD_UP:
      setPoint_temp++;
      setPoint = setPoint_temp;
      lcd.setCursor(10, 1); 
      lcd.print(setPoint);
      break;
      
    case KEYPAD_SELECT:
      setDefaultLCD();
      break;
  }
}

void displayCurrentTemperature() {
  lcd.setCursor(10, 0);
  lcd.print(currentTemperature, 2);
}

void setDefaultLCD() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Current:");
  lcd.setCursor(10, 0);
  lcd.print(currentTemperature, 2);
  lcd.setCursor(15, 0);
  lcd.print("C");
  lcd.setCursor(3, 1); 
  lcd.print("Target:");
  lcd.setCursor(10, 1);
  lcd.print(setPoint, 2);
  lcd.setCursor(15, 1);
  lcd.print("C");
}

