/*
*         __                __     _______________  _______  _______   
  _______/  |______ _______|  | __/   __   \   _  \ \   _  \ \   _  \  
 /  ___/\   __\__  \\_  __ \  |/ /\____    /  /_\  \/  /_\  \/  /_\  \ 
 \___ \  |  |  / __ \|  | \/    <    /    /\  \_/   \  \_/   \  \_/   \
/____  > |__| (____  /__|  |__|_ \  /____/  \_____  /\_____  /\_____  /
     \/            \/           \/                \/       \/       \/ 
*-----------------------------------------------------------------------
*Arduino Automatic Wire Cutter.    
*modded by Saliya Ruchiranga.   
*2022-01-22.   
*stark9000@gmail.com.   
*/

#include  <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <Servo.h>

//------------------------------- stepper ----------------------------------
const int STEP_PIN = 5;
const int DIR_PIN = 4;
const int EN_PIN = 12;
const int SLEEP_PIN = 6;
const int RST_PIN = 8;
const int MS1 = 11;
const int MS2 = 10;
const int MS3 = 9;
//------------------------------- servo ----------------------------------
Servo snippers;
const int servo = 6;
const int openAngle = 180;
const int closedAngle = 0;

//------------------------------- input ----------------------------------

#define leftButton A0
#define rightButton A1
#define upButton A2
#define downButton A3

//------------------------------- user settings ----------------------------------
unsigned int wireLength = 0;
unsigned int wireQuantity = 0;

//------------------------------- system settings ----------------------------------
int state = 0;
int incrementSpeed = 1;
int previousWireLength = 0;
int previousWireQuantity = 0;
float mmPerStep = 0.18096;


void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.print("Wire Cutter");
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH);

  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);

  pinMode(SLEEP_PIN, OUTPUT);
  pinMode(RST_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(MS3, LOW);

  digitalWrite(DIR_PIN, HIGH);
  digitalWrite(SLEEP_PIN, HIGH);
  digitalWrite(RST_PIN, HIGH);

  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  pinMode(leftButton, INPUT);
  pinMode(rightButton, INPUT);


  snippers.attach(servo);
  snippers.write(openAngle);
  delay(1000);

}

void loop() {
  if (digitalRead(rightButton)) {
    if (state == 5) {
      state = 0;
    }
    else {
      state += 1;
    }
    delay(200);
    lcd.clear();
  }
  if (digitalRead(leftButton) && state > 0 && state < 4) {
    state -= 1;
    delay(200);
    lcd.clear();
  }


  switch (state) {
    case 0:
      homeScreen();
      break;
    case 1:
      chooseWireLength();
      break;
    case 2:
      chooseWireQuantity();
      break;
    case 3:
      confirm();
      break;
    case 4:
      currentlyCutting();
      break;
    case 5:
      finishedCutting();
      break;
  }

}


void homeScreen() {
  lcd.setCursor(0, 0);
  lcd.print("WIRE CUTTER");
  lcd.setCursor(11, 1);
  lcd.print("NEXT>");
  delay(100);
}


void chooseWireLength() {
  wireLength = changeValue(wireLength);

  //clear LCD if required
  if (previousWireLength != wireLength) {
    lcd.clear();
    previousWireLength = wireLength;
  }

  //Display information on LCD
  lcd.setCursor(0, 0);
  lcd.print("LENGTH:" + (String)wireLength + "mm");
  displayNavigation();
}

void chooseWireQuantity() {
  wireQuantity = changeValue(wireQuantity);

  //clear LCD if required
  if (previousWireQuantity != wireQuantity) {
    lcd.clear();
    previousWireQuantity = wireQuantity;
  }

  //Display information on LCD
  lcd.setCursor(0, 0);
  lcd.print("QUANTITY:" + (String)wireQuantity);
  displayNavigation();
}

void confirm() {
  lcd.setCursor(0, 0);
  lcd.print((String)wireLength + "mm x " + (String)wireQuantity + "pcs");
  lcd.setCursor(0, 1);
  lcd.print("<BACK");
  lcd.setCursor(10, 1);
  lcd.print("START>");
  delay(100);
}

void currentlyCutting() {
  lcd.setCursor(0, 0);
  lcd.print((String)0 + "/" + (String)wireQuantity);
  lcd.setCursor(0, 1);
  lcd.print("???s");
  digitalWrite(EN_PIN, LOW);
  int stepsToTake = (int)wireLength / mmPerStep;
  for (int i = 0; i < wireQuantity; i++) {
    unsigned long timeForOneCycle = millis();
    digitalWrite(DIR_PIN, HIGH);
    for (int x = 0; x < stepsToTake; x++) {
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(500);
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(500);
    }

    lcd.setCursor(0, 0);
    lcd.print((String)(i + 1) + "/" + (String)wireQuantity);

    snippers.write(closedAngle);
    delay(600);
    snippers.write(openAngle);
    delay(600);


    lcd.setCursor(0, 1);

    unsigned long timeRemaining = ((millis() - timeForOneCycle) * (wireQuantity - (i + 1))) / 1000;
    lcd.print((String)timeRemaining + "s    ");

  }
  wireLength = 0;
  wireQuantity = 0;
  state = 5;
}

void finishedCutting() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CUTTING COMPLETE");
  lcd.setCursor(11, 1);
  lcd.print("NEXT>");
  digitalWrite(EN_PIN, HIGH);
  delay(100);
}


int changeValue(int currentValue) {
  if (digitalRead(upButton)) {
    delay(100);
    currentValue += incrementSpeed;

  }
  if (digitalRead(downButton)) {
    if (currentValue - incrementSpeed >= 0) {
      delay(100);
      currentValue -= incrementSpeed;

    }
    else {
      currentValue = 0;
    }
  }
  if (digitalRead(downButton) && !digitalRead(upButton)) {
    incrementSpeed = 1;
  }
  return currentValue;
}

void displayNavigation() {
  lcd.setCursor(0, 1);
  lcd.print("<BACK");
  lcd.setCursor(11, 1);
  lcd.print("NEXT>");
  delay(100);
}
