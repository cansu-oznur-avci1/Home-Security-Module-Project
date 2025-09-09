#include <Keypad.h>
#include <LiquidCrystal.h>

// LCD pins
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

// Keypad definition
const byte ROWS = 4; 
const byte COLS = 3; 
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5, 6, 7, 8};
byte colPins[COLS] = {9, 10, 11};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Pin definitions
const int door1 = 2;
const int win1 = 3;
const int win2 = 4;
const int alarmLED = 12;

//Password 
String correctPassword = "1234";
String enteredPassword = "";

unsigned long countdownStart = 0;
const int countdownTime = 10000; // 10 second
bool countdownActive = false;
bool alarmTriggered = false;
bool doorsClosed = true;

void setup() {
  pinMode(door1, INPUT_PULLUP);
  pinMode(win1, INPUT_PULLUP);
  pinMode(win2, INPUT_PULLUP);
  pinMode(alarmLED, OUTPUT);
  lcd.begin(16, 2);
  lcd.print("System is Ready...");
}

void loop() {
  char key = keypad.getKey();

  // If Alarm is active the system can be resetting manually with *
  if (alarmTriggered && key == '*') {
    lcd.clear();
    lcd.print("Resetting...");
    delay(1000);
    resetSystem();
    return;
  }

  if (countdownActive) {
    unsigned long elapsed = millis() - countdownStart;
    int remaining = (countdownTime - elapsed) / 1000;

    lcd.setCursor(0, 0);
    lcd.print("Enter Password:");
    lcd.setCursor(0, 1);
    lcd.print("Remaining: ");
    lcd.print(remaining);
    lcd.print(" sec   ");

    if (key) {
      if (key == '#') {
        if (enteredPassword == correctPassword) {
          lcd.clear();
          lcd.print("Correct Password!");
          delay(2000);
          resetSystem();
          return;
        } else {
          lcd.clear();
          lcd.print("Wrong Password!");
          delay(1000);
          triggerAlarm();
          return;
        }
      } else if (key == '*') {
        enteredPassword = "";
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Resetting...");
        delay(1000); 
        return;
      } else {
        enteredPassword += key;
        lcd.setCursor(10, 1);
        lcd.print(enteredPassword);
      }
    }

    if (elapsed > countdownTime) {
      lcd.clear();
      lcd.print("Time is Up!");
      delay(1000);
      triggerAlarm();
      return;
    }
  }

   // Start the time when one of the switches are opened
  if (!countdownActive && !alarmTriggered && doorsClosed) {
    if (digitalRead(door1)==LOW || digitalRead(win1)==LOW || digitalRead(win2)==LOW) {
      countdownStart = millis();
      countdownActive = true;
      enteredPassword = "";
      lcd.clear();
      doorsClosed = false;
    }
  }

  // System can be return to ready mode when the switches are closed
  if(!doorsClosed){
    if(digitalRead(door1)==HIGH && digitalRead(win1)==HIGH && digitalRead(win2)==HIGH){
      doorsClosed = true;
    }
  }
}

void triggerAlarm() {
  digitalWrite(alarmLED, HIGH);
  alarmTriggered = true;
  countdownActive = false;
  lcd.clear();
  lcd.print("ALARM!!!");
}

void resetSystem() {
  digitalWrite(alarmLED, LOW);
  countdownActive = false;
  alarmTriggered = false;
  enteredPassword = "";
  lcd.clear();
  lcd.print("System is Ready...");
}
