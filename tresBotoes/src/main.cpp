#include <Arduino.h>
#include <funcoes.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Inciando...");

  configurar();

  for(int i=0; i<5; i++){
    digitalWrite(botaoStartStop.pin_led, HIGH);
    digitalWrite(botaoSetup.pin_led, HIGH);
    digitalWrite(botaoManutecao.pin_led, HIGH);
    delay(100);
    digitalWrite(botaoStartStop.pin_led, LOW);
    digitalWrite(botaoSetup.pin_led, LOW);
    digitalWrite(botaoManutecao.pin_led, LOW);
    delay(100);
  }
}

void loop() {
 
  verificaBotao(&botaoStartStop, "START/STOP ");
  verificaBotao(&botaoSetup, "SETUP");
  verificaBotao(&botaoManutecao, "MANUTENÇÃO");
  
}