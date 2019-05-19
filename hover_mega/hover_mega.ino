#include<Servo.h>
#include <SPI.h>
#include "RF24.h"

#define MOTORES 3
#define MAX_MILLIS_TO_WAIT 1000

Servo ESC[MOTORES];

unsigned long starttime;
byte msg[3];

//RADIO

bool radioNumber = 0;
RF24 radio(9, 10);

byte addresses[][6] = {"1Node", "2Node"};

//END RADIO

void blink(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

void initializeESC() {
  for (int i = 0; i < MOTORES; i++) {
    ESC[i].writeMicroseconds(1000);
  }

  delay(1000);

  for (int i = 0; i < MOTORES; i++) {
    ESC[i].writeMicroseconds(2400);
  }

  delay(2000);

  for (int i = 0; i < MOTORES; i++) {
    ESC[i].writeMicroseconds(1000);
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  //Motores
  ESC[0].attach(3);
  ESC[1].attach(5);

  //Servo
  ESC[2].attach(6);
  ESC[2].write(70);

  delay(1000);
  ///Activar los ESC
  initializeESC();

  //Iniciar puerto serial
  Serial.begin(9600);
  Serial.setTimeout(10);

  //Done
  blink(5);

  initRF();
}

/*
  int getPpm(int bytes) {
  int x = ((float)bytes/(float)255)*800;
  return x + 1000;
  }*/

//
//Protocol 1º byte id, 2º and 3º byte int
//

void loop() {
  starttime = millis();

  /*//Max wait
    while ( (Serial.available()<3) && ((millis() - starttime) < MAX_MILLIS_TO_WAIT) )
    {
    }*/

  //if(readSerial())
  if (readRF())
  {
    unsigned char tempChar;
    int id = msg[0];
    int value = 0;
    value |= msg[1];
    value <<= 8;
    tempChar = (unsigned char) msg[2];
    value |= tempChar;
    
    if (checkErrors( tempChar)) 
      return;      
    
    Serial.print("ID: ");
    Serial.println(id);
    Serial.print("Value: ");
    Serial.println(value);

    if (id == 2){
      ESC[id].write(value);
      }
    else {
      ESC[id].writeMicroseconds(value);
    }
  }
}
bool checkErrors(int msg) {
  switch(msg){
    case 0:
    case 1:
    case 2:
      return true;
    break;
    }
    return false;
}

void initRF() {
  radio.begin();

  radio.enableAckPayload();
  radio.enableDynamicPayloads();

  if (radioNumber) {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1, addresses[0]);
  } else {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
  }
  radio.startListening();
}

bool readSerial() {
  if (Serial.available() >= 3) {
    Serial.readBytes(msg, 3);
    Serial.write(msg, 3);
    return true;
  }
  return false;
}

bool readRF() {
  byte pipeNo;
  if (radio.available(&pipeNo)) {
    radio.read( &msg, 3 );
    return true;
  }
  return false;
}
