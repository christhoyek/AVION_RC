#include <SPI.h>
#include <RF24.h>
#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";

// Joysticks
#define JOY_GAUCHE_Y A0
#define JOY_DROIT_X  A1
#define JOY_DROIT_Y  A2

// Boutons
#define BTN_SECU 2
#define BTN_PID 3

// Structure DATA
struct DataPacket {
  int joyGaucheY;
  int joyDroitX;
  int joyDroitY;
  bool secuMoteur;
  bool pidActif;
};

DataPacket data;

void setup() {
  Serial.begin(115200);

  pinMode(BTN_SECU, INPUT_PULLUP);
  pinMode(BTN_PID, INPUT_PULLUP);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_LOW);
  radio.stopListening();
}

void loop() {
  // Lecture joysticks
  data.joyGaucheY = analogRead(JOY_GAUCHE_Y);
  data.joyDroitX  = analogRead(JOY_DROIT_X);
  data.joyDroitY  = analogRead(JOY_DROIT_Y);

  // Boutons PULLUP
  data.secuMoteur = !digitalRead(BTN_SECU);
  data.pidActif    = !digitalRead(BTN_PID);

  // Envoi
  radio.write(&data, sizeof(DataPacket));

  // Debug
  Serial.print("Gaz: "); Serial.print(data.joyGaucheY);
  Serial.print(" | Ail: "); Serial.print(data.joyDroitX);
  Serial.print(" | Prof: "); Serial.print(data.joyDroitY);
  Serial.print(" | Secu: "); Serial.println(data.secuMoteur);
  Serial.print(" | PID: "); Serial.println(data.pidActif);

  delay(20); 
}