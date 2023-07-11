#include <Arduino.h>


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
/*
  Serial2.setTX(8);
  Serial2.setRX(9);*/
  Serial2.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
  {
    char c = Serial.read();
    Serial2.write(c);
    Serial.print(c);
  }
  if(Serial2.available())
  {
    char c2 = Serial2.read();
    Serial.write(c2);
  }

  
  /*
  if(Serial.available() > 0)
  {
    char c; 
    c = Serial.read();
    Serial2.write(c);
    Serial.print(c);
  }
  if(Serial2.available() > 0)
  {
    char c2;
    c2 = Serial2.read();
    Serial.write(c2);
  }*/
}
