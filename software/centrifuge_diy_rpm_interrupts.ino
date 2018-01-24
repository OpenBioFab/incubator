#include "ServoTimer2.h"
#include "PinChangeInterrupt.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 7, 6, 5, 4);
ServoTimer2 myservo;

// Encoder variables
#define encoder0PinA 9
#define encoder0PinB 8
volatile int encoderPos = 0; //this variable stores our current value of encoder position. Change to int or uin16_t instead of byte if you want to record a larger range than 0-255
volatile int oldEncPos = 0; //stores the last encoder position value so we can compare to the current reading and see if it has changed (so we know when to print to the serial monitor)

volatile byte aFlag = 0; // let's us know when we're expecting a rising edge on pinA to signal that the encoder has arrived at a detent
volatile byte bFlag = 0; // let's us know when we're expecting a rising edge on pinB to signal that the encoder has arrived at a detent (opposite direction to when aFlag is set)
volatile byte reading = 0; 

volatile unsigned int spinsCounter = 0;
unsigned int rpm = 0;

const int buttonPin = 11;
const int pinPhotoresistor = 3; // interrupt by hardware
const int pinServo = 10;
int buttonState = 0;
// Servo variables
int valServo;    // variable to read the value from the analog pin
long servoPulse;
long centrifugePeriod;
int selectedRPM;

int opcMenu1 = 0; // default selected
int opcMenu2 = 0;
int opcMenu3 = 0;
int currentGUI = 1;

void setup() {

  pinMode(pinPhotoresistor, INPUT);
  // Encoder config
  pinMode(encoder0PinA, INPUT); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(encoder0PinB, INPUT); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  digitalWrite(encoder0PinA, HIGH);  // turn on pull-up resistor
  digitalWrite(encoder0PinB, HIGH);  // turn on pull-up resistor

  attachPCINT(digitalPinToPCINT(encoder0PinA), doEncoderA, CHANGE);
  attachPCINT(digitalPinToPCINT(encoder0PinB), doEncoderB, CHANGE);
  
  digitalWrite(buttonPin, HIGH);
  myservo.attach(pinServo);
  Serial.begin(9600);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  lcd.print("DIY Bio!");
  lcd.setCursor(0, 1);
  lcd.print("Jelly Lab");
  delay(1400);
/*
  // timer interupts
  cli();//stop interrupts
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  sei();*/
}

void loop() {
  
  switch(currentGUI) {
    case 1:
      showTimeMenu();
      break;

    case 2: 
      showRPMMenu();
      break;

    case 3:
      showConfirmMenu();
      break;

    case 4:
      playCentrifuge();
      break;
  }
  
}

void doEncoderA() {
  
  cli();
  if (digitalRead(encoder0PinA) == HIGH && digitalRead(encoder0PinB) == HIGH && aFlag) {
    encoderPos--; //decrement the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0;
  } else if (digitalRead(encoder0PinA) == HIGH) bFlag = 1;
  sei();
  
  
  /*cli(); //stop interrupts happening before we read pin values
  reading = PINB & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && aFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos --; //decrement the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00000100) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
  sei(); //restart interrupts */
  
}

void doEncoderB() {

  cli();
  if (digitalRead(encoder0PinA) == HIGH && digitalRead(encoder0PinB) == HIGH && bFlag) {
    encoderPos++; //decrement the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0;
  } else if (digitalRead(encoder0PinB) == HIGH) aFlag = 1;
  sei();
  
  /*cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos ++; //increment the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00001000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
  sei(); //restart interrupts */
}
  
/*ISR(TIMER1_COMPA_vect) {

  rpm = spinsCounter * 30;
    /*  
  lcd.setCursor(0, 1);
  lcd.print("RPM: ");

  // filling 5 places always
  lcd.setCursor(5, 1);
 
  if (rpm > 9999) {
    lcd.print(String(rpm));  
  } else if (rpm > 999) {
    lcd.print(" " + String(rpm));  
  } else if (rpm > 99) {
    lcd.print("  " + String(rpm));  
  } else if (rpm > 9) {
    lcd.print("   " + String(rpm));  
  } else {
    lcd.print("    " + String(rpm)); 
  }
     
  // reset counter
  spinsCounter = 0;

  lcd.setCursor(11, 1);
  lcd.print(getMinsString(secs) + ":" + getSecondsString(secs));     
  secs--;*/
//} 

void addSpinCount() {
  spinsCounter++;
  //Serial.println(spinsCounter);
}

// Menu 1
void showTimeMenu() {

  opcMenu1 = 0;
  oldEncPos = 0;
  encoderPos = 0;
  selectTimeOption(opcMenu1);
  boolean wasPressed = false;
  
  do {

    buttonState = digitalRead(buttonPin);

    if (oldEncPos > encoderPos) {
      oldEncPos = encoderPos;
      //Serial.println("backward rotated!");
        if (opcMenu1 == 0) {
          opcMenu1 = 12;
        } else {
          opcMenu1--;
        }
        
        selectTimeOption(opcMenu1);
        
    } else if (oldEncPos < encoderPos) {
      oldEncPos = encoderPos;
      //Serial.println("forward rotated!");
        if (opcMenu1 == 12) {
          opcMenu1 = 0;
        } else {
          opcMenu1++;
        }
        
        selectTimeOption(opcMenu1);
    }

    if (buttonState == 0) {
      wasPressed = true; 
    } 

    // clicked and released
    if (wasPressed && buttonState == 1) {
      wasPressed = false;
      // Pass to next menu
      currentGUI = 2;  
    }  
    
  } while(currentGUI == 1);
  
}

void selectTimeOption(int option) {

  switch(option) {
    case 0:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("15s");
      centrifugePeriod = 15000;
      break;
      
    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("30s");
      centrifugePeriod = 30000;
      break;

    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("45s");
      centrifugePeriod = 45000;
      break;

    case 3: 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("1min");
      centrifugePeriod = 60000;
      break;

    case 4:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("2min");
      centrifugePeriod = 120000;
      break;
      
    case 5:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("3min");
      centrifugePeriod = 180000;
      break;

    case 6:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("4min");
      centrifugePeriod = 240000;
      break;

    case 7:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("5min");
      centrifugePeriod = 300000;
      break;
 
    case 8:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("6min");
      centrifugePeriod = 360000;
      break;

    case 9:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("7min");
      centrifugePeriod = 420000;
      break;

    case 10:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("8min");
      centrifugePeriod = 480000;
      break;

    case 11:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("9min");
      centrifugePeriod = 540000;
      break;

    case 12:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("10min");
      centrifugePeriod = 600000;
      break;
  }
 
}

// Menu 2
void showRPMMenu() {

  opcMenu2 = 1;
  oldEncPos = 0;
  encoderPos = 0;
  selectRPMOption(opcMenu2);
  boolean wasPressed = false;
  
  do {

    // Reading switch from encoder
    buttonState = digitalRead(buttonPin);

    if (oldEncPos > encoderPos) {
      oldEncPos = encoderPos;
      //Serial.println("backward rotated!");
        if (opcMenu2 == 0) {
          opcMenu2 = 17;
        } else {
          opcMenu2--;
        }
        
        selectRPMOption(opcMenu2); 
        
    } else if (oldEncPos < encoderPos) {
      oldEncPos = encoderPos;
      //Serial.println("forward rotated!");
        if (opcMenu2 == 17) {
          opcMenu2 = 0;
        } else {
          opcMenu2++;
        }
        
        selectRPMOption(opcMenu2);
    }

    if (buttonState == 0) {
      wasPressed = true; 
    } 

    // clicked and released
    if (wasPressed && buttonState == 1) {
      wasPressed = false;

      if (opcMenu2 == 0) {
        currentGUI = 1;
      } else {
        currentGUI = 3;
      }
    }
    
  } while(currentGUI == 2);

}

void selectRPMOption(int option) {
  
  switch(option) {
    case 0:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("<Return");
      break;
      
    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~3000");
      selectedRPM = 3000;
      servoPulse = 1603; // 1600 is the starting point
      break;

    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~3500");
      selectedRPM = 3500;
      servoPulse = 1609;
      break;

    case 3: 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~4000");
      selectedRPM = 4000;
      servoPulse = 1617;
      break;

    case 4:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~4500");
      selectedRPM = 4500;
      servoPulse = 1625;
      break;

    case 5:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~5000");
      selectedRPM = 5000;
      servoPulse = 1635;
      break;

    case 6:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~5500");
      selectedRPM = 5500;
      servoPulse = 1648;
      break;

    case 7:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~6000");
      selectedRPM = 6000;
      servoPulse = 1660;
      break;

    case 8:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~6500");
      selectedRPM = 6500;
      servoPulse = 1673;
      break;

    case 9:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~7000");
      selectedRPM = 7000;
      servoPulse = 1688;
      break;

    case 10:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~7500");
      selectedRPM = 7500;
      servoPulse = 1670;
      break;

    case 11:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~8000");
      selectedRPM = 8000;
      servoPulse = 1720;
      break;

    case 12:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~8500");
      selectedRPM = 8500;
      servoPulse = 1740;
      break;

    case 13:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~9000");
      selectedRPM = 9000;
      servoPulse = 1765;
      break;

    case 14:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~9500");
      selectedRPM = 9500;
      servoPulse = 1787;
      break;

    case 15:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~10000");
      selectedRPM = 10000;
      servoPulse = 1803;
      break;

    case 16:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~10500");
      selectedRPM = 10500;
      servoPulse = 1810;
      break;

    case 17:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~11000");
      selectedRPM = 11000;
      servoPulse = 1828;
      break;
  }

}

// Menu 3
void showConfirmMenu() {

  opcMenu3 = 0;
  oldEncPos = 0;
  encoderPos = 0;
  selectConfirmMenuOption(opcMenu3);
  boolean wasPressed = false;
  int timeSelected = centrifugePeriod / 1000;

  lcd.clear();
  lcd.setCursor(0, 0);
    
  if (timeSelected >= 60) {
    timeSelected = timeSelected / 60;
    lcd.print(String(timeSelected) + "min" + " " + selectedRPM + "RPM ?");
  } else {
    lcd.print(String(timeSelected) + "s" + " " + selectedRPM + "RPM ?");
  }

  lcd.setCursor(0, 1);
  lcd.print(">Confirm  Return");

  do {

    // Reading switch from encoder
    buttonState = digitalRead(buttonPin);

    if (oldEncPos > encoderPos) {
      oldEncPos = encoderPos;
      //Serial.println("backward rotated!");
        if (opcMenu3 == 0) {
          opcMenu3 = 1;
        } else {
          opcMenu3--;
        }
        
        selectConfirmMenuOption(opcMenu3); 
        
    } else if (oldEncPos < encoderPos) {
      oldEncPos = encoderPos;
      //Serial.println("forward rotated!");
        if (opcMenu3 == 1) {
          opcMenu3 = 0;
        } else {
          opcMenu3++;
        }
        
        selectConfirmMenuOption(opcMenu3);
    }

    if (buttonState == 0) {
      wasPressed = true; 
    } 

    // clicked and released
    if (wasPressed && buttonState == 1) {
      wasPressed = false;

      if (opcMenu3 == 0) { // Confirm
        currentGUI = 4;
        lcd.clear();
        lcd.print("Preparing...");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Starting in 3!");
        delay(1000);
        lcd.clear();
        lcd.print("Starting in 2!");
        delay(1000);
        lcd.clear();
        lcd.print("Starting in 1!");
        delay(1000);
      } else if (opcMenu3 == 1) { // Return
        currentGUI = 1;
      }
    }
    
  } while(currentGUI == 3);
  
}

void selectConfirmMenuOption(int option) {
  switch(option) {
    case 0:
      lcd.setCursor(0, 1);
      lcd.print("~");
      lcd.setCursor(9, 1);
      lcd.print(" ");
      break;
      
    case 1:
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(9, 1);
      lcd.print("~");
      break;
  }
}

// two space formatter
String getMinsString(int seconds) {
  int mins = seconds / 60;
  String minsString = "";

  if (seconds > 599) {
    minsString = String(mins);
  } else if (seconds > 59) {
    minsString = " " + String(mins);
  } else {
    minsString = "  ";
  }

  return minsString;
}

// two space formatter
String getSecondsString(int seconds) {
  int mins = seconds / 60;
  int secs = seconds - (mins * 60);
  String secsString = "";

  if (secs > 9) {
    secsString = String(secs);
  } else {
    secsString = "0" + String(secs);
  }

  return secsString;
}

void playCentrifuge() {

  unsigned long startMillis = millis();
  unsigned long endMillis = millis();
  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0;
  unsigned int secs = centrifugePeriod / 1000; 
  boolean wasPressed = false;
  rpm = 0;

  attachInterrupt(1, addSpinCount, RISING); // pin 3 by hardware

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press to stop");

  /*cli();
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  TCNT1 = 0;
  sei();//allow interrupts*/

  int i = 0;
  
  while (endMillis - startMillis <= centrifugePeriod) {

    currentMillis = millis();
    buttonState = digitalRead(buttonPin);
    
    // Each second
    if (currentMillis - previousMillis >= 1000) {
      previousMillis = currentMillis;

      rpm = spinsCounter * 30;
      //Serial.println(spinsCounter);

      lcd.setCursor(11, 1);
      lcd.print(getMinsString(secs) + ":" + getSecondsString(secs));
      
      lcd.setCursor(0, 1);
      lcd.print("RPM: ");

      // filling 5 places always
      lcd.setCursor(5, 1);

      if (rpm > 9999) {
        lcd.print(String(rpm));  
      } else if (rpm > 999) {
        lcd.print(" " + String(rpm));  
      } else if (rpm > 99) {
        lcd.print("  " + String(rpm));  
      } else if (rpm > 9) {
        lcd.print("   " + String(rpm));  
      } else {
        lcd.print("    " + String(rpm)); 
      }

      /*if (selectedRPM > rpm && i == 3) {
        servoPulse++;
      }
      
      i++;*/
         
      // reset counter
      spinsCounter = 0;      
      secs--;
    }
    
    myservo.write(servoPulse);
    //Serial.println(servoDegrees);

    if (buttonState == 0) {
      wasPressed = true; 
    } 
    
    // clicked and released
    if (wasPressed && buttonState == 1) {
      wasPressed = false;
      break;
    }
    

    /*if (currentMillis - previousMillis2 >= 2000) {
      previousMillis2 = currentMillis;
      //Serial.println(spinsCounter);

      rpm = spinsCounter * 15;
      //Serial.println(rpm);
      
      lcd.setCursor(0, 1);
      lcd.print("RPM: ");

      // filling 5 places always
      lcd.setCursor(5, 1);

      if (rpm > 9999) {
        lcd.print(String(rpm));  
      } else if (rpm > 999) {
        lcd.print(" " + String(rpm));  
      } else if (rpm > 99) {
        lcd.print("  " + String(rpm));  
      } else if (rpm > 9) {
        lcd.print("   " + String(rpm));  
      } else {
        lcd.print("    " + String(rpm)); 
      }
         
      // reset counter
      spinsCounter = 0;
    } */
    
    endMillis = millis();
  }

  myservo.write(0);
  // Done message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Done! :)");
  // detach interrupts
  //TIMSK1 = 0; // deactivate timer's interrupt.
  //TCNT1 = 0;
  detachInterrupt(1);
  
  delay(3000);
  // reinitialize values
  currentGUI = 1;
}

