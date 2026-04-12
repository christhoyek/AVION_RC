#include <SPI.h>
#include <RF24.h>
#include <Arduino.h>
#include <Servo.h>


RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";


Servo servoD5;
Servo servoD2;
Servo servoD3;
Servo esc;


struct DataPacket {
 int joyGaucheY;
 int joyDroitX;
 int joyDroitY;
 bool secuMoteur;
 bool pidActif;
};


DataPacket data;


#define DEADZONE 25
#define JOY_CENTER 523


int appliquerZoneMorte(int valeur) {
 if (abs(valeur - JOY_CENTER) < DEADZONE) {
   return JOY_CENTER;
 }
 return valeur;
}


int dernierAngleD5 = -1;
int dernierAngleD2 = -1;
int dernierAngleD3 = -1;
int dernierPwmESC  = -1;


void setup() {
 Serial.begin(115200);


 servoD5.attach(5);
 servoD2.attach(2);
 servoD3.attach(3);


 // ESC sur D6 — plage 1000µs à 2000µs
 esc.attach(6, 1000, 2000);


 // Séquence d'armement ESC
 Serial.println("Armement ESC...");
 esc.writeMicroseconds(2000);
 delay(2000);
 esc.writeMicroseconds(1000);
 delay(2000);
 Serial.println("ESC arme !");


 radio.begin();
 radio.openReadingPipe(0, address);
 radio.setPALevel(RF24_PA_LOW);
 radio.startListening();


 Serial.println("Test NRF24...");
 if (radio.isChipConnected()) {
   Serial.println("NRF24 detecte OK");
 } else {
   Serial.println("NRF24 NON detecte !");
 }
 Serial.print("Canal : ");
 Serial.println(radio.getChannel());
 Serial.print("PA Level : ");
 Serial.println(radio.getPALevel());
}


void loop() {
 if (radio.available()) {
   DataPacket temp;
   radio.read(&temp, sizeof(DataPacket));


   // Validation basique
   if (temp.joyGaucheY < 0 || temp.joyGaucheY > 1023) return;
   if (temp.joyDroitX  < 0 || temp.joyDroitX  > 1023) return;
   if (temp.joyDroitY  < 0 || temp.joyDroitY  > 1023) return;


   data = temp;


   int gaucheY = appliquerZoneMorte(data.joyGaucheY);
   int droitX  = appliquerZoneMorte(data.joyDroitX);
   int droitY  = appliquerZoneMorte(data.joyDroitY);


   // Servos
   int angleD5 = constrain(map(gaucheY, 350, 700, 45, 135), 45, 135);
   int angleD2 = constrain(map(gaucheY, 350, 700, 45, 135), 45, 135);
   int angleD3 = constrain(map(droitX,  350, 700, 45, 135), 45, 135);


   if (angleD5 != dernierAngleD5) { servoD5.write(angleD5); dernierAngleD5 = angleD5; }
   if (angleD2 != dernierAngleD2) { servoD2.write(angleD2); dernierAngleD2 = angleD2; }
   if (angleD3 != dernierAngleD3) { servoD3.write(angleD3); dernierAngleD3 = angleD3; }


   // ESC — joyDroitY : moitié basse = arrêt, moitié haute = gaz
   int angleGaz = constrain(map(droitY, 350, 700, 0, 180), 0, 180);
   int pwmESC;
   if (angleGaz <= 90) {
     pwmESC = 1000; // en dessous du centre = arrêt
   } else {
     pwmESC = map(angleGaz, 90, 180, 1000, 2000); // 90°→180° mappé sur 1000µs→2000µs
   }


   if (pwmESC != dernierPwmESC) { esc.writeMicroseconds(pwmESC); dernierPwmESC = pwmESC; }


   Serial.print("Gaz: ");      Serial.print(data.joyGaucheY);
   Serial.print(" | Ail: ");   Serial.print(data.joyDroitX);
   Serial.print(" | Prof: ");  Serial.print(data.joyDroitY);
   Serial.print(" | Secu: ");  Serial.print(data.secuMoteur);
   Serial.print(" | PID: ");   Serial.print(data.pidActif);
   Serial.print(" | D5: ");    Serial.print(angleD5);
   Serial.print(" | D2: ");    Serial.print(angleD2);
   Serial.print(" | D3: ");    Serial.print(angleD3);
   Serial.print(" | ESC: ");   Serial.println(pwmESC);
 }
}
