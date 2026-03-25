
#include <Streaming.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>
#include <Chrono.h>

#define dirPinA 30
#define stepPinA 31

#define dirPinB 32
#define stepPinB 33

#define dirPinC 34
#define stepPinC 35

#define dirPinD 36
#define stepPinD 37

#define dirPinE 38
#define stepPinE 39

#define dirPinF 40
#define stepPinF 41


#define limitA 7
#define limitB 5
#define limitC 8
#define limitD 9

#define limitE 6

#define hallIC 10


#define cutter 47
#define buzzer 43
#define led 45

#define threadingPin 53
#define cutterArmPin 51
#define gripperPin 50


#define enA 4
#define in1 3
#define in2 2

#define ldr1 48
#define ldr2 44
#define ldr3 46
#define ldr4 42

#define knob A15

#define maxStepA 925
#define maxStepB 900
#define maxStepC 925
#define maxStepD 925
#define maxStepE 675
#define upLimitE 400
#define maxStepF 400

#define beepMax 20

#define threadExtendDelay 5000
#define cutterTime 6000

LiquidCrystal_I2C lcd(0x27, 20, 4);

Chrono myChrono;
Chrono testChrono;
Chrono displayChrono;
Chrono beepChrono;
Chrono bobbinChrono;

Servo threading;
Servo cutterArm;
Servo gripper;

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};


byte rowPins[ROWS] = { 29, 27, 25, 23 };
byte colPins[COLS] = { 22, 24, 26, 28 };

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

bool currentStepDirA;
bool currentStepDirB;
bool currentStepDirC;
bool currentStepDirD;
bool currentStepDirE;
bool currentStepDirF;

int stepPosA;
int stepPosB;
int stepPosC;
int stepPosD;
int stepPosE;
int stepPosF;


int threadingPos;
int armCutterPos=50;

int mode;
int initMode;
int displayMode;
int testMode;

int currentDisplay = -1;

int selectedBobbin;

int spinningTime = 3000;
bool threadingDir;
int speed;
String numberInput;
String secondInput;
int number;
int second = 10;
bool paused;
bool pausedPrevState;

int test;

int beepCount;
int ledCount;
int count;
int offset = 0;

bool checkSpool;
int prevKnob;

bool onStart=true;

void setup() {
  pinMode(dirPinA, OUTPUT);
  pinMode(stepPinA, OUTPUT);
  pinMode(dirPinB, OUTPUT);
  pinMode(stepPinB, OUTPUT);
  pinMode(dirPinC, OUTPUT);
  pinMode(stepPinC, OUTPUT);
  pinMode(dirPinD, OUTPUT);
  pinMode(stepPinD, OUTPUT);
  pinMode(dirPinE, OUTPUT);
  pinMode(stepPinE, OUTPUT);
  pinMode(dirPinF, OUTPUT);
  pinMode(stepPinF, OUTPUT);

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(cutter, OUTPUT), digitalWrite(cutter, HIGH);
  pinMode(buzzer, OUTPUT), digitalWrite(buzzer, HIGH);
  pinMode(led, OUTPUT), digitalWrite(led, HIGH);

  pinMode(limitA, INPUT_PULLUP);
  pinMode(limitB, INPUT_PULLUP);
  pinMode(limitC, INPUT_PULLUP);
  pinMode(limitD, INPUT_PULLUP);
  pinMode(limitE, INPUT_PULLUP);

  pinMode(hallIC, INPUT);
  pinMode(knob, INPUT);

  pinMode(ldr1, INPUT);
  pinMode(ldr2, INPUT);
  pinMode(ldr3, INPUT);
  pinMode(ldr4, INPUT);

  Serial.begin(9600);

  Serial << "begin" << endl;
  
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0), lcd << F("====================");
  lcd.setCursor(0, 1), lcd << F("       BOBBIN       ");
  lcd.setCursor(0, 2), lcd << F("       MACHINE      ");
  lcd.setCursor(0, 3), lcd << F("====================");
  // getI2CAdress();

  threading.attach(threadingPin);
  cutterArm.attach(cutterArmPin);
  gripper.attach(gripperPin);

  speed = knobValue();

  setSpeed(map(speed, 20, 100, 128, 150));
  Serial << "Speed :: " << speed << endl;
  displayChrono.restart();
  cutterArm.write(50);
  threadingPos = 100;
  threading.write(threadingPos);
  delay(2000);
}
void loop() {

  int value = knobValue();

  if (value != speed) {
    Serial << "speed :: " << speed << endl;
    speed = value;
    setSpeed(map(speed, 20, 100, 128, 150));
    displayChrono.restart();
  }

  normalProcess();
  bobbinCheck();
  display();
}


void bobbinCheck() {
  if (withBobbin1() && withBobbin2() && withBobbin3() && withBobbin4()) {
    bobbinChrono.restart();
  }
  if (bobbinChrono.hasPassed(5000)) {
    checkSpool = true;
    if (beepCount <= beepMax) {
      if (beepChrono.hasPassed(1000)) {
        beepChrono.restart();
        buzzerChange();
        beepCount++;
      }
    } else {
      buzzerOff();
    }

    ledOn();
  } else {
    if (mode != 100)
      ledOff();
    buzzerOff();
    beepChrono.restart();
    beepCount = 0;
    checkSpool = false;
  }
}

void display() {
  if (checkSpool) {
    displayMode = -10;


  } else {
    if (displayChrono.hasPassed(2000)) {
      if (paused) {
        displayMode = -11;
      }
    } else {
      displayMode = -12;
      currentDisplay = -1;
    }
  }


  if (currentDisplay != displayMode) {
    currentDisplay = displayMode;
    switch (displayMode) {
      case -12:
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("        SPEED       ");
        lcd.setCursor(0, 2), lcd << F("         ") << speed << F("         ");
        lcd.setCursor(0, 3), lcd << F("====================");
        break;

      case -11:
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("       PAUSED       ");
        lcd.setCursor(0, 2), lcd << F("COUNTER: ") << count << " ";
        lcd.setCursor(0, 3), lcd << F("(*)RESUME    (#)EXIT");
        break;

      case -10:
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("        CHECK       ");
        lcd.setCursor(0, 2), lcd << F("     THREAD SPOOL   ");
        lcd.setCursor(0, 3), lcd << F("====================");
        break;

      case 0:
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("   INITIALIZING...  ");
        lcd.setCursor(0, 2), lcd << F("                    ");
        lcd.setCursor(0, 3), lcd << F("====================");
        break;


      case 1:
        lcd.setCursor(0, 0), lcd << F("=======Choose=======");
        lcd.setCursor(0, 1), lcd << F("A. MANUAL           ");
        lcd.setCursor(0, 2), lcd << F("B. START            ");
        lcd.setCursor(0, 3), lcd << F("====================");
        break;


      case 2:
        lcd.setCursor(0, 0), lcd << F("=======Choose=======");
        lcd.setCursor(0, 1), lcd << F("A. THREAD           ");
        lcd.setCursor(0, 2), lcd << F("B. SPINNER Z-AXIS   ");
        lcd.setCursor(0, 3), lcd << F("====================");
        break;


      case 3:
        lcd.setCursor(0, 0), lcd << F("A. THREAD 1         ");
        lcd.setCursor(0, 1), lcd << F("B. THREAD 2         ");
        lcd.setCursor(0, 2), lcd << F("C. THREAD 3         ");
        lcd.setCursor(0, 3), lcd << F("D. THREAD 4         ");
        break;

      case 4:
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("  SHOWING SELECTED  ");
        lcd.setCursor(0, 2), lcd << F("       THREAD       ");
        lcd.setCursor(0, 3), lcd << F("====================");
        break;

      case 5:
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("   YOU MAY GET THE  ");
        lcd.setCursor(0, 2), lcd << F("       THREAD       ");
        lcd.setCursor(0, 3), lcd << F("====================");
        break;

      case 6:
        lcd.setCursor(0, 0), lcd << F("=======Spinner======");
        lcd.setCursor(0, 1), lcd << F("A. MOVE UP          ");
        lcd.setCursor(0, 2), lcd << F("B. MOVE DOWN        ");
        lcd.setCursor(0, 3), lcd << F("====================");
        break;

      case 7:
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("       SPINNER      ");
        lcd.setCursor(0, 2), lcd << F("      MOVING UP     ");
        lcd.setCursor(0, 3), lcd << F("====================");
        break;

      case 8:
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("       SPINNER      ");
        lcd.setCursor(0, 2), lcd << F("     MOVING DOWN    ");
        lcd.setCursor(0, 3), lcd << F("====================");
        break;

      case 9:
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("        MOVE        ");
        lcd.setCursor(0, 2), lcd << F("    SPINNER DOWN    ");
        lcd.setCursor(0, 3), lcd << F("====================");
        break;

      case 10:
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("     ON PROCESS     ");
        lcd.setCursor(0, 2), lcd << F("COUNTER: ") << count << " ";
        lcd.setCursor(0, 3), lcd << F("PRESS (*) TO PAUSE  ");
        break;

      case 11:
        lcd.setCursor(0, 0), lcd << F("=======INPUT========");
        lcd.setCursor(0, 1), lcd << F("  NUMBER OF BOBBIN  ");
        lcd.setCursor(0, 2), lcd << numberInput << F("   ");
        lcd.setCursor(0, 3), lcd << F("(A) OK     (D)Delete");
        break;

      case 12:
        lcd.setCursor(0, 0), lcd << F("=======INPUT========");
        lcd.setCursor(0, 1), lcd << F("  WINDING TIME(sec) ");
        lcd.setCursor(0, 2), lcd << secondInput << F("   ");
        lcd.setCursor(0, 3), lcd << F("(A) OK     (D)Delete");
        break;

      case 100:
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("      COMPLETED     ");
        lcd.setCursor(0, 2), lcd << F("COUNTER: ") << count << " ";
        lcd.setCursor(0, 3), lcd << F("   PRESS A TO EXIT  ");
        break;
    }
  }
}

void normalProcess() {
  if (mode > 19 && mode < 100) {
    char customKey = customKeypad.getKey();
    if (customKey) {
      currentDisplay = -1;
      if (customKey == '*') {
        paused = !paused;
        if (paused) {
          myChrono.stop();
        } else {
          myChrono.resume();
        }
      }
      if (customKey == '#' && paused) {
        mode = 101;
        paused = false;
        myChrono.resume();
      } else if (customKey == 'D') {
        secondInput = "";
        lcd.clear();
      } else if (customKey != 'A' && customKey != 'B' && customKey != 'C' && customKey != 'D' && customKey != '*' && customKey != '#') {
        secondInput = secondInput + customKey;
      }
    }
  }


  if (paused) {
    motorOff();
    cutterOff();
    ledOff();
    buzzerOff();

  } else {
    switch (mode) {
      case 0:
        {
          initialize();
          break;
        }
      case 1:
        {
          displayMode = 1;
          char customKey = customKeypad.getKey();
          if (customKey) {
            if (customKey == 'A') {
              mode = 2;
            } else if (customKey == 'B') {
              myChrono.restart();
              lcd.clear();
              count = 0;
              mode = 15;
            }
          }
          break;
        }

      case 2:
        {
          displayMode = 2;
          char customKey = customKeypad.getKey();

          if (customKey) {
            if (customKey == 'A') {
              mode = 3;
            } else if (customKey == 'B') {
              mode = 6;
            }
          }
          break;
        }


      case 3:
        {
          displayMode = 3;
          char customKey = customKeypad.getKey();

          if (customKey) {
            if (customKey == 'A') {
              selectedBobbin = 1;
              mode = 4;
            } else if (customKey == 'B') {
              mode = 4;
              selectedBobbin = 2;
            } else if (customKey == 'C') {
              mode = 4;
              selectedBobbin = 3;
            } else if (customKey == 'D') {
              mode = 4;
              selectedBobbin = 4;
            }
          }
          break;
        }


      case 4:
        {
          displayMode = 4;
          bool a;
          bool b;
          bool c;
          bool d;
          if (selectedBobbin == 1) {
            a = MoveDownA();
          } else {
            a = MoveUpA();
          }
          if (selectedBobbin == 2) {
            b = MoveDownB();
          } else {
            b = MoveUpB();
          }
          if (selectedBobbin == 3) {
            c = MoveDownC();
          } else {
            c = MoveUpC();
          }

          if (selectedBobbin == 4) {
            d = MoveDownD();
          } else {
            d = MoveUpD();
          }
          if (a && b && c && d) {
            mode = 5;
            myChrono.restart();
          }

          break;
        }

      case 5:
        {
          displayMode = 5;
          if (myChrono.hasPassed(threadExtendDelay)) {
            if (MoveUpA() && MoveUpB() && MoveUpC() && MoveUpD()) {
              myChrono.restart();
              mode = 1;
            }
          }
          break;
        }

      case 6:
        {
          displayMode = 6;
          char customKey = customKeypad.getKey();
          if (customKey) {
            if (customKey == 'A') {
              mode = 7;
            } else if (customKey == 'B') {
              mode = 8;
            }
          }
          break;
        }

      case 7:
        {
          displayMode = 7;

          if (MoveUpE(false)) {
            mode = 1;
          }
          break;
        }

      case 8:
        {
          displayMode = 8;
          if (MoveDownE()) {
            mode = 1;
          }
          break;
        }

      case 15:
        {
          if (stepPosE >= maxStepE) {
            mode = 16;
          } else {
            displayMode = 9;
            if (myChrono.hasPassed(2000)) {
              mode = 1;
            }
          }
        }
        break;

      case 16:
        {
          displayMode = 12;
          char customKey = customKeypad.getKey();
          if (customKey) {
            currentDisplay = -1;
            if (customKey == 'A') {
              if (secondInput != "") {
                second = secondInput.toInt();
                mode = 17;
              }

            } else if (customKey == 'D') {
              secondInput = "";
              lcd.clear();
            } else if (customKey != 'A' && customKey != 'B' && customKey != 'C' && customKey != 'D' && customKey != '*' && customKey != '#') {
              secondInput = secondInput + customKey;
            }
          }
          break;
        }
      case 17:
        {
          displayMode = 11;
          char customKey = customKeypad.getKey();
          if (customKey) {
            currentDisplay = -1;
            if (customKey == 'A') {
              if (numberInput != "") {
                number = numberInput.toInt();
                myChrono.restart();
                onStart=true;
                mode = 18;
              }

            } else if (customKey == 'D') {
              numberInput = "";
              lcd.clear();
            } else if (customKey != 'A' && customKey != 'B' && customKey != 'C' && customKey != 'D' && customKey != '*' && customKey != '#') {
              numberInput = numberInput + customKey;
      

            }
          }
          break;
        }


      case 18:
        {
          displayMode = 10;
          if (myChrono.hasPassed(2000)) {
            myChrono.restart();
            mode = 19;
          }

          break;
        }

      case 19:
        {
          if (number == 0) {
            threadingPos = 45;
            offset = 52;
            mode = 20;
            if(onStart){
              onStart=false; 
              mode=195;       
            }else{
              mode = 20;
            }
          } else {
            if (number <= count) {
              mode = 100; 
              ledCount = 0;
            } else {
              threadingPos = 45;
              offset = 52;
              mode = 20;
            }
          }

          myChrono.restart();

          break;
        }

          case 195:
        {
          cutterOn();
          if (myChrono.hasPassed(cutterTime)) {
            cutterOff();
            mode = 20;
          }
          break;
        }


      case 20:
        {
          displayMode = 10;

          if (myChrono.hasPassed(second * 1000)) {
            myChrono.restart();
            mode = 21;
          }
          motorOn();
          if (threadingDir) {
            if (threadingCCW()) {
              if (offset <= 0) {
                offset = 52;
              } else {
                offset = offset - 5;
              }

              threadingDir = !threadingDir;
            }

          } else {
            if (threadingCW()) {
              if (offset <= 0) {
                offset = 52;
              } else {
                offset = offset - 5;
              }

              threadingDir = !threadingDir;
            }
          }
          break;
        }
      case 21:
        {
          threading.write(90);
          if (myChrono.hasPassed(1000)) {
            motorOff();
            mode = 22;
          }
          break;
        }

      case 22:
        {
          if (MoveUpE(false)) {
            mode = 23;
          }
          break;
        }

      case 23:
        {
          if (MoveConveyourUntilNotDetected()) {
            mode = 24;
          }
          break;
        }

      case 24:
        {
          if (MoveConveyourUntilDetected()) {
            mode = 25;
          }
          break;
        }

      case 25:
        {
          if (cutterArmExtend()) {
            mode = 26;
            myChrono.restart();
            gripper.write(0);
          }
          break;
        }

      case 26:
        {

          if (myChrono.hasPassed(1500)) {
            mode = 27;
          }
          break;
        }

      case 27:
        if (MoveDownE()) {
          myChrono.restart();

          mode = 28;
        }
        break;

      case 28:
        {
          cutterOn();
          if (myChrono.hasPassed(cutterTime)) {
            cutterOff();
            mode = 29;
          }
          break;
        }

      case 29:
        {
          if (cutterArmRetract()) {
            gripper.write(100);
            mode = 30;
            myChrono.restart();
          }
          break;
        }

      case 30:
        {

          if (myChrono.hasPassed(1500)) {
            mode = 19;
            count++;
            currentDisplay = -1;
          }
          break;
        }

      case 100:
        {
          displayMode = 100;
          cutterOff();
          motorOff();
          if (myChrono.hasPassed(1000)) {
            myChrono.restart();
            if (ledCount < 4) {
              ledChange();
              ledCount++;
            } else {
              ledOff();
            }
          }
          char customKey = customKeypad.getKey();
          if (customKey) {
            if (customKey == 'A') {
              mode = 1;
              ledOff();
            }
          }

          break;
        }

      case 101:
        {
          fastInitialize();

          break;
        }
    }
  }
}

void fastInitialize() {

  displayMode = 0;

  switch (initMode) {
    case 0:
      threading.write(100);
      cutterArm.write(50);
      gripper.write(100);
      threadingPos = 0;
      armCutterPos = 50;
      initMode = 1;
      break;

    case 1:
      if (MoveUpE(false)) {
        initMode = 2;
      }
      break;

    case 2:
      if (MoveConveyourUntilNotDetected()) {
        initMode = 3;
      }
      break;

    case 3:
      if (MoveUpA() && MoveUpB() && MoveUpC() && MoveUpD() && MoveConveyourUntilDetected()) {
        initMode = 0;
        mode = 1;
        test = 0;
      }
      break;
  }
}
void initialize() {
  displayMode = 0;
  switch (initMode) {

    case 0:
      threading.write(100);
      cutterArm.write(50);
      gripper.write(100);

      delay(1000);
      initMode = 1;
      break;

    case 1:
      if (MoveDownA() && MoveDownB() && MoveDownC() && MoveDownD() && MoveUpE(true) && MoveConveyourUntilNotDetected()) {
        stepPosA = 0;
        stepPosB = 0;
        stepPosC = 0;
        stepPosD = 0;
        stepPosE = 0;

        threadingPos = 0;
        armCutterPos = 50;
        initMode = 2;
      }
      break;
    case 2:
      if (MoveUpA() && MoveUpB() && MoveUpC() && MoveUpD() && MoveConveyourUntilDetected()) {
        initMode = 0;
        mode = 1;
        test = 0;
      }
      break;

    default:
      // statements
      break;
  }
}


void setSpeed(int value) {
  analogWrite(enA, value);
}
void motorOn() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void motorOff() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

bool cutterArmRetract() {

  if (armCutterPos >= 50) {
    return true;
  }
  armCutterPos++;
  cutterArm.write(armCutterPos);
  delay(15);
  return false;
}
bool cutterArmExtend() {

  if (armCutterPos < 0) {
    return true;
  }
  --armCutterPos;
  cutterArm.write(armCutterPos);
  delay(15);
  return false;
}

bool threadingCW() {
  if (threadingPos >= 100 - offset) {
    return true;
  }
  threadingPos++;
  threading.write(threadingPos);
  delay(30 * (200 - speed) / 100);
  return false;
}

bool threadingCCW() {

  if (threadingPos <= 5 + offset) {
    return true;
  }
  --threadingPos;
  threading.write(threadingPos);
  delay(30 * (200 - speed) / 100);
  return false;
}

void cutterOn() {
  digitalWrite(cutter, LOW);
}

void cutterOff() {
  digitalWrite(cutter, HIGH);
}

void buzzerOn() {
  digitalWrite(buzzer, LOW);
}

void buzzerOff() {
  digitalWrite(buzzer, HIGH);
}

void buzzerChange() {
  digitalWrite(buzzer, !digitalRead(buzzer));
}
void ledOn() {
  digitalWrite(led, LOW);
}

void ledOff() {
  digitalWrite(led, HIGH);
}
void ledChange() {
  digitalWrite(led, !digitalRead(led));
}
int knobValue() {
  //0-1023 0= 0volts  1023 = 5volts 
  int myVal = constrain(map(analogRead(knob), 0, 1023, 10, 120), 20, 100);
  if (abs(myVal - prevKnob) > 3) {
    prevKnob = myVal;
  }

  return roundToMultiple(prevKnob, 5);
}
int roundToMultiple(int toRound, int multiple) {
  return (toRound + (multiple / 2)) / multiple * multiple;
}
bool withBobbin1() {
  return digitalRead(ldr1);
}

bool withBobbin2() {
  return digitalRead(ldr2);
}

bool withBobbin3() {
  return digitalRead(ldr3);
}

bool withBobbin4() {
  return digitalRead(ldr4);
}

bool isOnContainer() {
  return !digitalRead(hallIC);
}
bool isOnLimitA() {
  return !digitalRead(limitA);
}

bool isOnLimitB() {
  return !digitalRead(limitB);
}

bool isOnLimitC() {
  return !digitalRead(limitC);
}

bool isOnLimitD() {
  return !digitalRead(limitD);
}

bool isOnLimitE() {
  return !digitalRead(limitE);
}


bool MoveDownA() {  //downward
  if (isOnLimitA()) {
    stepPosA = 0;
    return true;
  }
  if (currentStepDirA) {
    delay(500);
    currentStepDirA = false;
  }
  digitalWrite(dirPinA, LOW);

  digitalWrite(stepPinA, HIGH);
  delayMicroseconds(2000);
  digitalWrite(stepPinA, LOW);
  delayMicroseconds(2000);
  --stepPosA;
  return false;
}

bool MoveUpA() {  //upward
  if (stepPosA >= maxStepA) {
    return true;
  }
  if (!currentStepDirA) {
    delay(500);
    currentStepDirA = true;
  }
  digitalWrite(dirPinA, HIGH);
  digitalWrite(stepPinA, HIGH);
  delayMicroseconds(1000);
  digitalWrite(stepPinA, LOW);
  delayMicroseconds(1000);
  stepPosA++;
  return false;
}


bool MoveDownB() {  //downward
  if (isOnLimitB()) {
    stepPosB = 0;
    return true;
  }
  if (currentStepDirB) {
    delay(500);
    currentStepDirB = false;
  }
  digitalWrite(dirPinB, LOW);

  digitalWrite(stepPinB, HIGH);
  delayMicroseconds(1000);
  digitalWrite(stepPinB, LOW);
  delayMicroseconds(1000);
  --stepPosB;
  return false;
}

bool MoveUpB() {
  if (stepPosB >= maxStepB) {
    return true;
  }
  if (!currentStepDirB) {
    delay(500);
    currentStepDirB = true;
  }
  digitalWrite(dirPinB, HIGH);
  digitalWrite(stepPinB, HIGH);
  delayMicroseconds(1000);
  digitalWrite(stepPinB, LOW);
  delayMicroseconds(1000);
  stepPosB++;
  return false;
}

bool MoveDownC() {
  if (isOnLimitC()) {
    stepPosC = 0;
    return true;
  }
  if (currentStepDirC) {
    delay(500);
    currentStepDirC = false;
  }
  digitalWrite(dirPinC, LOW);

  digitalWrite(stepPinC, HIGH);
  delayMicroseconds(2000);
  digitalWrite(stepPinC, LOW);
  delayMicroseconds(2000);
  --stepPosC;
  return false;
}

bool MoveUpC() {
  if (stepPosC >= maxStepC) {
    return true;
  }
  if (!currentStepDirC) {
    delay(500);
    currentStepDirC = true;
  }
  digitalWrite(dirPinC, HIGH);
  digitalWrite(stepPinC, HIGH);
  delayMicroseconds(1000);
  digitalWrite(stepPinC, LOW);
  delayMicroseconds(1000);
  stepPosC++;
  return false;
}


bool MoveDownD() {
  if (isOnLimitD()) {
    stepPosD = 0;
    return true;
  }
  if (currentStepDirD) {
    delay(500);
    currentStepDirD = false;
  }
  digitalWrite(dirPinD, LOW);

  digitalWrite(stepPinD, HIGH);
  delayMicroseconds(2000);
  digitalWrite(stepPinD, LOW);
  delayMicroseconds(2000);
  --stepPosD;
  return false;
}

bool MoveUpD() {
  if (stepPosD >= maxStepD) {
    return true;
  }
  if (!currentStepDirD) {
    delay(500);
    currentStepDirD = true;
  }
  digitalWrite(dirPinD, HIGH);
  digitalWrite(stepPinD, HIGH);
  delayMicroseconds(1000);
  digitalWrite(stepPinD, LOW);
  delayMicroseconds(1000);
  stepPosD++;
  return false;
}

bool MoveUpE(bool initialize) {  //upward
  if (isOnLimitE()) {
    stepPosE = 0;
    return true;
  }
  if (!initialize) {
    if (stepPosE <= upLimitE) {
      return true;
    }
  }

  if (currentStepDirE != digitalRead(dirPinE)) {
    delay(500);
    currentStepDirE = !currentStepDirE;
  }
  digitalWrite(dirPinE, LOW);

  digitalWrite(stepPinE, HIGH);
  delayMicroseconds(2000);
  digitalWrite(stepPinE, LOW);
  delayMicroseconds(2000);
  --stepPosE;
  return false;
}

bool MoveDownE() {  //downward

  if (stepPosE >= maxStepE) {
    return true;
  }
  if (currentStepDirE) {
    delay(500);
    currentStepDirE = false;
  }
  digitalWrite(dirPinE, HIGH);
  digitalWrite(stepPinE, HIGH);
  delayMicroseconds(1000);
  digitalWrite(stepPinE, LOW);
  delayMicroseconds(1000);
  stepPosE++;
  return false;
}


bool MoveConveyourUntilNotDetected() {
  if (!isOnContainer()) {
    return true;
  }
  digitalWrite(dirPinF, LOW);

  digitalWrite(stepPinF, HIGH);
  delayMicroseconds(500);
  digitalWrite(stepPinF, LOW);
  delayMicroseconds(500);
  return false;
}

bool MoveConveyourUntilDetected() {
  if (isOnContainer()) {
    stepPosF = 0;
    return true;
  }

  digitalWrite(dirPinF, LOW);

  digitalWrite(stepPinF, HIGH);
  delayMicroseconds(500);
  digitalWrite(stepPinF, LOW);
  delayMicroseconds(500);
  stepPosF++;
  return false;
}


void getI2CAdress() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");

      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");

      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("done");
  while (true)
    ;
}