#include <SPI.h>
#include <RF24.h>
#include <Servo.h>
#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

RF24 radio(9, 10);
const byte address[6] = "00001";

// Servos
Servo servoAileronG;
Servo servoAileronD;
Servo servoProfondeur;
Servo esc;

// Pins
#define PIN_AIL_G 3
#define PIN_AIL_D 5
#define PIN_PROF  6
#define PIN_ESC   9

// Structure DATA 
struct DataPacket {
  int joyGaucheY;
  int joyDroitX;
  int joyDroitY;
  bool secuMoteur;
  bool pidActif;
};

DataPacket data;

// Fail-safe
unsigned long lastReceiveTime = 0;
const unsigned long TIMEOUT = 500;

void setup() {
  Serial.begin(115200);

  servoAileronG.attach(PIN_AIL_G);
  servoAileronD.attach(PIN_AIL_D);
  servoProfondeur.attach(PIN_PROF);
  esc.attach(PIN_ESC);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_LOW);
  radio.startListening();

  // 🔥 Armement ESC (IMPORTANT)
  esc.writeMicroseconds(1000);
  delay(3000);

  lastReceiveTime = millis();
}

void loop() {

  // 📡 Réception
  if (radio.available()) {
    radio.read(&data, sizeof(DataPacket));
    lastReceiveTime = millis();

    // 🎮 Mapping
    int throttle = map(data.joyGaucheY, 300, 700, 1000, 2000);
    int aileron  = map(data.joyDroitX, 300, 700, 0, 180);
    int profondeur = map(data.joyDroitY, 300, 700, 0, 180);

    // 🔐 Sécurité moteur prioritaire
    if (data.secuMoteur) {
      throttle = 1000;
    }

    // ✈️ Commandes
    esc.writeMicroseconds(throttle);

    servoAileronG.write(aileron);
    servoAileronD.write(180 - aileron); // INVERSION

    servoProfondeur.write(profondeur);

    Serial.println("Signal OK");
  }

  // ⚠️ FAIL-SAFE
  if (millis() - lastReceiveTime > TIMEOUT) {

    Serial.println("FAIL SAFE ACTIVÉ");

    // Positions neutres
    servoAileronG.write(90);
    servoAileronD.write(90);
    servoProfondeur.write(90);

    esc.writeMicroseconds(1000);
  }
}