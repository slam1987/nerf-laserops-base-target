#include <IRLibDecodeBase.h>
#include <IRLib_P01_NEC.h>
#include <IRLib_P02_Sony.h>
#include <IRLib_HashRaw.h>
#include <IRLibCombo.h>
#include <IRLibRecv.h>

IRdecode myDecoder;

//// GLOBAL VARIABLES

// Multiplier
int damageMultiplier = 1;

// IR Receiver Pin
IRrecv myReceiver(7);

// IR Blaster Codes
static const long alphaPointBlue   = 0x2FFEA610;
static const long alphaPointPurple = 0x67228B44;
static const long alphaPointRed    = 0x78653B0E;

static const long deltaBurstBlue   = 0x9BE00484;
static const long deltaBurstPurple = 0xD303E9B8;
static const long deltaBurstRed    = 0xE4469982;

// Ownership LED Pin Map
const int teamRedLedPin  = 4;
const int teamBlueLedPin = 5;

// LED Pin Map
const int ledPin9  = 9;
const int ledPin10 = 10;
const int ledPin11 = 11;
const int ledPin12 = 12;
const int ledPin13 = 13;

// Array for Hit Point LED Pins
const int ledPins[5] = {ledPin9, ledPin10, ledPin11, ledPin12, ledPin13};

// Push Button Mapping
const int resetButtonPin = 3;
const int teamButtonPin  = 8;

// Resting Push Button State
int resetButtonState = 0;
int teamButtonState  = 0;

// Hit Points
// maxHitPoint / blasterDamage must equal 5 (# of Hit Point LED)
int blasterDamage = 10;
int maxHitPoint   = 50;
int hitPoint = maxHitPoint;

// Ownership Identifier
// RED = 1
// BLUE = 2
int teamOwner = 1;

// Valid Blaster Code
long validBlasterCode = deltaBurstBlue;

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
    tone(2, 500, 100);
    delay(300);
  }
}

// Hit Response
void valid_hit_response() {
  --hitPoint;
  tone(2, 500, 250);
  if (hitPoint == maxHitPoint - blasterDamage * damageMultiplier) {
    analogWrite(ledPins[5 - damageMultiplier], 0);
    ++damageMultiplier;
  }
}

// Initial Setup Parameters
void setup() {
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(teamButtonPin, INPUT_PULLUP);

  // Ownership Identifier LED
  analogWrite(teamRedLedPin, 255);
  analogWrite(teamBlueLedPin, 0);

  led_hp_all_on();

  // IR Receiver Module
  Serial.begin(9600);
  delay(2000); while (!Serial);
  myReceiver.enableIRIn();
  Serial.println(F("Ready to receive IR signals"));
}

void loop() {

  // Digital Push Button State
  teamButtonState = digitalRead(teamButtonPin);
  resetButtonState = digitalRead(resetButtonPin);

  // Toggle Team Ownership
  if (teamButtonState == HIGH) {
    if (teamOwner == 1) {
      teamOwner = 2;
      validBlasterCode = deltaBurstRed;
      analogWrite(teamRedLedPin, 0);
      analogWrite(teamBlueLedPin, 255);
      delay(2000);
    }
    else if (teamOwner == 2) {
      teamOwner = 1;
      validBlasterCode = deltaBurstBlue;
      analogWrite(teamRedLedPin, 255);
      analogWrite(teamBlueLedPin, 0);
      delay(2000);
    }
  }

  // Activate piezo when Hit Point depleted
  if (hitPoint <= 0) {
    led_hp_all_off();
    tone (2, 500, 250);
  }

  // Reset Hit Point Button
  if (resetButtonState == HIGH) {
    for (int i = 0; i < 3; ++i) {
      led_hp_all_on();
      delay(500);
      led_hp_all_off();
      delay(500);
    }
    led_hp_all_on_seq();
    hitPoint = maxHitPoint;
    damageMultiplier = 1;
  }

  // Activate IR Receiver
  if (myReceiver.getResults()) {
    myDecoder.decode();
    myReceiver.enableIRIn();

    if (myDecoder.protocolNum == UNKNOWN) {
      Serial.print(F("0x"));
      Serial.println(myDecoder.value, HEX);

      // IR Receiver Valid Hit
      if (myDecoder.value == validBlasterCode) {
        valid_hit_response();
      }
    }
    else {
      myDecoder.dumpResults(false);
    }
  }
}
