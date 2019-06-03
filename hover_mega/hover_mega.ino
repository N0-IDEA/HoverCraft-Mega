#include<Servo.h>
#include <SPI.h>
#include "RF24.h"

#define MOTORES 3
#define N_ESC 2
#define MAX_MILLIS_TO_WAIT 1000

Servo ESC[MOTORES];

unsigned long starttime;
byte msg[3];
byte type;

//RADIO

bool radioNumber = 0;
RF24 radio(9,10);

byte addresses[][6] = {"1Node","2Node"};

//END RADIO

void blink(int n) {
  for(int i=0;i<n;i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

void initializeESC() {
  for(int i=0;i<N_ESC;i++){
    ESC[i].writeMicroseconds(1000);
  }

  delay(1000);

  for(int i=0;i<N_ESC;i++) {
    ESC[i].writeMicroseconds(2400);
  }

  delay(2000);

  for(int i=0;i<N_ESC;i++) {
    ESC[i].writeMicroseconds(1000);
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  //Motores
  ESC[0].attach(3);
  ESC[1].attach(5);

  ///Activar los ESC
  delay(1000);

  initializeESC();

  //Servo
  ESC[2].attach(6);
  ESC[2].write(50);

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

  int id;
  int value;
  if(readRF(&value, &id))
  {



    Serial.print("ID: ");
    Serial.println(id);
    Serial.print("Value: ");
    Serial.println(value);

    if(id == 2) {
      ESC[id].write(value);
      Serial.println("servo");
    }
    else {
      ESC[id].writeMicroseconds(value);
    }
  }
}
bool checkErrors(int id, int value) {
  if(id == 2) {
    if(value < 360 && value > 0)
      return false;
  } else {
    if(value > 1000 && value < 1900)
      return false;
  }
  return true;
}

void initRF() {
  radio.begin();

  radio.enableAckPayload();
  radio.enableDynamicPayloads();

  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  radio.startListening();
}

bool readSerial() {
  if(Serial.available() >= 3){
    Serial.readBytes(msg, 3);
    Serial.write(msg,3);
    return true;
  }
  return false;
}

bool readRF(int *valRead, int*idRead) {     
  byte pipeNo;
  if (radio.available(&pipeNo)){
      radio.read( &type, 1);
    if(type == 0) {
      radio.read(&msg, 4);
      
      unsigned char tempChar;
      int id = msg[1];
      int value = 0;
      value |= msg[2];
      value <<= 8;
      tempChar = (unsigned char) msg[3];
      value |= tempChar;
  
      if (!checkErrors(id, value)) {
        *idRead = id;
        *valRead = value;
        return true;
      }
    } else {
      byte cellID[2];
      radio.read(cellID, 2);
      byte prueba[2];
      prueba[0]= 0x14;
      prueba[1]= 0x14;
      radio.writeAckPayload(pipeNo, &prueba, 2);
      Serial.print(cellID[1]);
      Serial.println(" payload");
    }
  }
  return false;
}
