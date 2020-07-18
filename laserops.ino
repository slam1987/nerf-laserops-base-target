#include <IRLibDecodeBase.h>
#include <IRLib_P01_NEC.h>
#include <IRLib_P02_Sony.h>
#include <IRLib_HashRaw.h>
#include <IRLibCombo.h>
#include <IRLibRecv.h>

IRdecode myDecoder;

//// GLOBAL VARIABLES

const int hitLedPin1 = A1;
const int hitLedPin2 = A2;
const int hitLedPin3 = A3;
const int hitLedPin4 = A4;
int hitLedCounter = 0;

// Misc
int damageMultiplier = 1;
int tierLedCounter = 3;
int healthTier = 500;
int healthTierCounter = 10;
int invulnerableTimer = 0;

// IR Receiver Pin
IRrecv myReceiver(A0);

// IR Blaster Codes
static const long alphaPointBlue   = 0x2FFEA610;
static const long alphaPointPurple = 0x67228B44;
static const long alphaPointRed    = 0x78653B0E;

static const long deltaBurstBlue   = 0x9BE00484;
static const long deltaBurstPurple = 0xD303E9B8;
static const long deltaBurstRed    = 0xE4469982;

// LED Pin Map
const int ledPin4  = 4;
const int ledPin5  = 5;
const int ledPin6  = 6;
const int ledPin7  = 7;
const int ledPin8  = 8;
const int ledPin9  = 9;
const int ledPin10 = 10;
const int ledPin11 = 11;
const int ledPin12 = 12;
const int ledPin13 = 13;

// Array for Hit Point LED Pins
const int bulbCount  = 10;
const int ledPins[bulbCount] = {ledPin4, ledPin5, ledPin6, ledPin7, ledPin8, ledPin9, ledPin10, ledPin11, ledPin12, ledPin13};

// Push Button Mapping
const int resetButtonPin = 3;
const int teamButtonPin  = 0;

// Resting Push Button State
int resetButtonState = 0;
int teamButtonState  = 0;

// Hit Points
// maxHitPoint
int bulbHealth = 1;
int maxHitPoint   = bulbHealth * bulbCount;
int hitPoint = maxHitPoint;

// Valid Blaster Code
long validBlasterCode = alphaPointBlue;

// Ownership Identifier
// RED = 1
// BLUE = 2
int teamOwner = 1;

//// FUNCTIONS

// Turn ON all Hit Point LEDs
void led_hp_all_on() {
  for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++) {
    analogWrite(ledPins[i], 255);
  }
}

// Turn OFF all Hit Point LEDs
void led_hp_all_off() {
  for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++) {
    analogWrite(ledPins[i], 0);
  }
}

// Sequentially Turn ON all Hit Point LEDs
void led_hp_all_on_seq() {
  for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++) {
    analogWrite(ledPins[i], 255);
    tone(2, 500, 250);
    delay(300);
  }
}

void dead() {
  delay(1000);
  led_hp_all_on();
  delay(1000);
  led_hp_all_off();
}

// Hit Response
void valid_hit_response() {
  --hitPoint;
  tone(2, 500, 250);
  hitLedCounter = 5000;
  if (healthTierCounter == 10) {
    invulnerableTimer = 10;
  }
  if (healthTierCounter == 7) {
    invulnerableTimer = 20;
  }
  if (healthTierCounter == 4) {
    invulnerableTimer = 30;
  }
  if (hitPoint == maxHitPoint - bulbHealth * damageMultiplier) {
    --healthTierCounter;
    for (int i = 0; i < invulnerableTimer; i++) {
      analogWrite(hitLedPin1, 255);
      analogWrite(hitLedPin2, 255);
      analogWrite(hitLedPin3, 255);
      analogWrite(hitLedPin4, 255);
      tone(2, 500, 250);
      analogWrite(ledPins[bulbCount - damageMultiplier], 255);
      delay(500);
      analogWrite(ledPins[bulbCount - damageMultiplier], 0);
      delay(500);
      if (i == 9) {
        analogWrite(hitLedPin1, 0);
        analogWrite(hitLedPin2, 0);
        analogWrite(hitLedPin3, 0);
        analogWrite(hitLedPin4, 0);
      }
    }
    ++damageMultiplier;
  }
}

// Initial Setup Parameters
void setup() {

  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(teamButtonPin, INPUT_PULLUP);

  analogWrite(A1, 0);
  analogWrite(A2, 0);
  analogWrite(A3, 0);
  analogWrite(A4, 0);
  led_hp_all_on_seq();

  // IR Receiver Module
  Serial.begin(9600);
  while (!Serial);
  myReceiver.enableIRIn();
  Serial.println(F("Ready to receive IR signals"));
}

void loop() {
  // Digital Push Button State
  teamButtonState = digitalRead(teamButtonPin);
  resetButtonState = digitalRead(resetButtonPin);

  //  // Toggle Team Ownership
  //  if (teamButtonState == HIGH) {
  //    if (teamOwner == 1) {
  //      teamOwner = 2;
  //      validBlasterCode = deltaBurstRed;
  //      delay(2000);
  //    }
  //    else if (teamOwner == 2) {
  //      teamOwner = 1;
  //      validBlasterCode = deltaBurstBlue;
  //      delay(2000);
  //    }
  //  }

  // Reset Hit Point Button
  if (resetButtonState == HIGH) {
    for (int i = 0; i < 3; ++i) {
      led_hp_all_on();
      delay(250);
      led_hp_all_off();
      delay(250);
      led_hp_all_on();
      delay(250);
      led_hp_all_off();
      delay(250);
    }
    led_hp_all_on_seq();
    hitPoint = maxHitPoint;
    damageMultiplier = 1;
    healthTierCounter = 10;
  }

  if (hitLedCounter != 0) {
    analogWrite(hitLedPin1, 255);
    analogWrite(hitLedPin2, 255);
    analogWrite(hitLedPin3, 255);
    analogWrite(hitLedPin4, 255);
    --hitLedCounter;
  }
  if (hitLedCounter == 0) {
    analogWrite(hitLedPin1, 0);
    analogWrite(hitLedPin2, 0);
    analogWrite(hitLedPin3, 0);
    analogWrite(hitLedPin4, 0);
  }

  // Activate piezo when Hit Point depleted
  if (hitPoint <= 0) {
    led_hp_all_off();
    dead();
  }

  // Activate IR Receiver
  if (myReceiver.getResults()) {
    myDecoder.decode();
    if (myDecoder.protocolNum == UNKNOWN) {
      Serial.print(F("IR recieved. Hash value is: 0x"));
      Serial.println(myDecoder.value, HEX);

      // IR Receiver Valid Hit
      if (myDecoder.value == validBlasterCode) {
        valid_hit_response();
      }
    }
    else {
      myDecoder.dumpResults(false);
    }
    myReceiver.enableIRIn();
  }
}
