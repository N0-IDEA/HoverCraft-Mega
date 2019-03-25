#include<Servo.h>

Servo ESC[2];
#define MAX_MILLIS_TO_WAIT 1000

unsigned long starttime;
byte msg[4];

void blink(int n) {
  for(int i=0;i<n;i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);                      
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);  
  }
}

void initializeESC() {
  for(int i=0;i<2;i++){
      ESC[i].writeMicroseconds(1000); //1000 = 1ms
  }
  delay(1000);
  for(int i=0;i<2;i++) {
    ESC[i].writeMicroseconds(2400);
  }
  delay(2000);
  for(int i=0;i<2;i++) {
    ESC[i].writeMicroseconds(1000);
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); 
  
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
  
  //Max wait
  while ( (Serial.available()<3) && ((millis() - starttime) < MAX_MILLIS_TO_WAIT) )
  {      
  }
  
  if(Serial.available() >= 3)
  {
    Serial.readBytes(msg, 3);
    int id = msg[0];
    int value = 0;
    value |= msg[1];
    value <<= 8;
    value |= msg[2];
    
    if(id == 2)
      ESC[id].write(value);
    else {
      ESC[id].writeMicroseconds(value);
    }
    Serial.write(msg, 3);
  }  
}
