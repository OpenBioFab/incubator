
#include <LiquidCrystal.h>
#include <Servo.h>

// joystick variables
#define joyX A2
#define joyY A3

LiquidCrystal lcd(12, 11, 7, 6, 5, 4);
Servo myservo;

// Joystick values
int xValue, yValue;
const int buttonPin = 10;
int buttonState = 0;
// Servo variables
int valServo;    // variable to read the value from the analog pin
long servoDegrees;
long centrifugePeriod;

int opcMenu1 = 1; // default selected
int opcMenu2 = 1;
int currentGUI = 1;

void setup() {
  
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
  myservo.attach(9);
  //delay(3000);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  lcd.print("DIY Bio!");
  lcd.setCursor(0, 1);
  lcd.print("Jelly Lab");
  delay(1400);
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
      playCentrifuge();
      break;
  }
  
}

// Menu 1
void showTimeMenu() {

  selectTimeOption(opcMenu1);
  
  do {

    // Reading joystick
    xValue = analogRead(joyX);
    yValue = analogRead(joyY);
    buttonState = digitalRead(buttonPin);

    // UP
    if (yValue == 0) {
      Serial.println("UP");
      if (opcMenu1 == 1) {
        opcMenu1 = 4;
      } else {
        opcMenu1--;
      }
      selectTimeOption(opcMenu1);
      delay(200);
    
    // DOWN
    } else if (yValue == 1023) {
      Serial.println("DOWN");
      if (opcMenu1 == 4) {
        opcMenu1 = 1;
      } else {
        opcMenu1++;
      }
      selectTimeOption(opcMenu1);
      delay(200);
    }

    // If joystick switch pressed
    if (buttonState == 0) {
      // Pass to next menu
      currentGUI = 2;
      delay(200);
    }   
    
  } while(currentGUI == 1);
  
}

void selectTimeOption(int option) {

  switch(option) {
    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("15 s");
      centrifugePeriod = 15000;
      break;

    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("30 s");
      centrifugePeriod = 30000;
      break;

    case 3: 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("45 s");
      centrifugePeriod = 45000;
      break;

    case 4:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select time:");
      lcd.setCursor(0, 1);
      lcd.print("60 s");
      centrifugePeriod = 60000;
      break;
  }
 
}

// Menu 2
void showRPMMenu() {

  selectRPMOption(opcMenu2);
  
  do {

    // Reading joystick
    xValue = analogRead(joyX);
    yValue = analogRead(joyY);
    buttonState = digitalRead(buttonPin);

    // UP
    if (yValue == 0) {
      Serial.println("UP");
      if (opcMenu2 == 1) {
        opcMenu2 = 27;
      } else {
        opcMenu2--;
      }
      selectRPMOption(opcMenu2);
      delay(200);
    
    // DOWN
    } else if (yValue == 1023) {
      Serial.println("DOWN");
      if (opcMenu2 == 27) {
        opcMenu2 = 1;
      } else {
        opcMenu2++;
      }
      selectRPMOption(opcMenu2);
      delay(200);
    }
    
    // Servo operations
    /*valServo = analogRead(A1);      
    valServo = map(valServo, 0, 1023, 0, 180);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Select RPM:");
    lcd.setCursor(0, 1);
    lcd.print(valServo);
    delay(200); */

    if (buttonState == 0) {
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
      currentGUI = 3;
    }    
    
  } while(currentGUI == 2);

}

void selectRPMOption(int option) {
  
  switch(option) {
    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~2600");
      servoDegrees = 103;
      break;

    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~3200");
      servoDegrees = 104;
      break;

    case 3: 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~3720");
      servoDegrees = 105;
      break;

    case 4:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~4150");
      servoDegrees = 106;
      break;

    case 5:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~4600");
      servoDegrees = 107;
      break;

    case 6:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~5000");
      servoDegrees = 108;
      break;

    case 7:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~5350");
      servoDegrees = 109;
      break;

    case 8:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~5700");
      servoDegrees = 110;
      break;

    case 9:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~6000");
      servoDegrees = 111;
      break;

    case 10:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~6250");
      servoDegrees = 112;
      break;

    case 11:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~6500");
      servoDegrees = 113;
      break;

    case 12:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~6750");
      servoDegrees = 114;
      break;

    case 13:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~7000");
      servoDegrees = 115;
      break;

    case 14:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~7250");
      servoDegrees = 116;
      break;

    case 15:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~7400");
      servoDegrees = 117;
      break;

    case 16:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~7600");
      servoDegrees = 118;
      break;

    case 17:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~7800");
      servoDegrees = 119;
      break;

    case 18:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~7950");
      servoDegrees = 120;
      break;

    case 19:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~8250");
      servoDegrees = 122;
      break;

    case 20:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~8550");
      servoDegrees = 124;
      break;

    case 21:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~8800");
      servoDegrees = 126;
      break;

    case 22:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~9150");
      servoDegrees = 128;
      break;

    case 23:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~9500");
      servoDegrees = 130;
      break;

    case 24:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~9700");
      servoDegrees = 132;
      break;

    case 25:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~9800");
      servoDegrees = 134;
      break;

    case 26:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~10000");
      servoDegrees = 140;
      break;

    case 27:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select RPM:");
      lcd.setCursor(0, 1);
      lcd.print("~10100");
      servoDegrees = 145; 
      break;
  }

}

void playCentrifuge() {

  // Photoresistor variables
  int valRPM = 0;
  int count = 0;
  boolean shouldCount = true;
  // RPM variables
  unsigned long leftMarker;
  unsigned long rightMarker;
  unsigned int rpm;

  unsigned long startMillis = millis();
  unsigned long endMillis = millis(); 

  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0;
  unsigned int seconds = centrifugePeriod / 1000;
  
  while(endMillis - startMillis <= centrifugePeriod) {

    currentMillis = millis();
    
    // Timer
    if(currentMillis - previousMillis >= 1000) {
      // save the last time you blinked the LED 
      previousMillis = currentMillis;   

      lcd.setCursor(14, 1);

      if (seconds >= 10) {
        lcd.print(seconds);
      } else {
        lcd.print(" " + String(seconds));
      }
         
      seconds--;
    }
    
    myservo.write(servoDegrees);

    valRPM = analogRead(A0);
    
    if (valRPM <= 20 && shouldCount) {
      shouldCount = false;
      count++;
      //Serial.println(count);
  
      if (count == 1) {
        leftMarker = millis();
      } else if (count >= 2) {
        rightMarker = millis();
      }
      
    } else if (valRPM > 20 && !shouldCount) {
      shouldCount = true;
    }
  
    // Calculating RPM
    // when object enters
    if (count >= 2 && !shouldCount) {
      long interval = rightMarker - leftMarker;
  
      if (interval >= 2000) {
        rpm = count * 15;
        //Serial.println(rpm);
  
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("DIY Bio!");
        lcd.setCursor(0, 1);
        lcd.print("RPM: ");

        // filling 5 places always
        lcd.setCursor(5, 1);

        if (rpm > 9999) {
          lcd.print(" " + String(rpm));  
        } else {
          lcd.print(rpm);  
        }
      
        // reset counter
        count = 0;
        shouldCount = true;
      }
    }

    //Serial.println(endMillis - startMillis);
    endMillis = millis();
  }

  myservo.write(0);
  // Done message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Done! :)");
  delay(3000);
  // reinitialize values
  currentGUI = 1;
  opcMenu1 = 1;
  opcMenu2 = 1;
  
}

