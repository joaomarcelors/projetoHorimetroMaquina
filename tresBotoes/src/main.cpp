#include <Arduino.h>
#include <funcoes.h>

#define led1 17
#define led2 19
#define led3 18

void setup() {
  // put your setup code here, to run once:
  pinMode(led1, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(led3, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(led1, HIGH);
  digitalWrite(19, HIGH);
  digitalWrite(led3, HIGH);
  delay(500);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  delay(500);  
}