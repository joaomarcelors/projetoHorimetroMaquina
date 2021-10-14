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

  //Botao START/STOP
  botaoStartStop.estado_atual = !digitalRead(botaoStartStop.pin_botao);
  
  if(botaoStartStop.estado_atual != botaoStartStop.ultimo_estado){  
    botaoStartStop.ultimo_estado = botaoStartStop.estado_atual;
    if(botaoStartStop.estado_atual == HIGH){
      if(!botaoStartStop.parado){
        Serial.println("=========================================");
        Serial.println("MAQUINA PARADA!");
        Serial.println("TIPO: START/STOP");
        botaoStartStop.parado = true;
        digitalWrite(botaoStartStop.pin_led, HIGH);
       /* data_inicio_parada = getData();
        hora_inicio_parada = getHora();
        Serial.println("Data: " + data_inicio_parada);
        Serial.println("Hora: " + hora_inicio_parada);*/
        Serial.println("=========================================");       
      }else{
        Serial.println("=========================================");
        Serial.println("MAQUINA RETORMADA!");
        //Serial.println("RELATORIO:");
        botaoStartStop.parado = false;
        digitalWrite(botaoStartStop.pin_led, LOW);
       /* data_fim_parada = getData();
        hora_fim_parada = getHora();
        Serial.println("Data inicio: " + data_inicio_parada);
        Serial.println("Hora inicio: " + hora_inicio_parada);      
        Serial.println("Data fim:   " + data_fim_parada);
        Serial.println("Hora fim:   " + hora_fim_parada);*/
        Serial.println("=========================================");
      }
    }
    delay(75);
  }    

  //Botao SETUP
  botaoSetup.estado_atual = !digitalRead(botaoSetup.pin_botao);
  
  if(botaoSetup.estado_atual != botaoSetup.ultimo_estado){  
    botaoSetup.ultimo_estado = botaoSetup.estado_atual;
    if(botaoSetup.estado_atual == HIGH){
      if(!botaoSetup.parado){
        Serial.println("=========================================");
        Serial.println("MAQUINA PARADA!");
        Serial.println("TIPO: SETUP");
        botaoSetup.parado = true;
        digitalWrite(botaoSetup.pin_led, HIGH);
       /* data_inicio_parada = getData();
        hora_inicio_parada = getHora();
        Serial.println("Data: " + data_inicio_parada);
        Serial.println("Hora: " + hora_inicio_parada);*/
        Serial.println("=========================================");       
      }else{
        Serial.println("=========================================");
        Serial.println("MAQUINA RETORMADA!");
        //Serial.println("RELATORIO:");
        botaoSetup.parado = false;
        digitalWrite(botaoSetup.pin_led, LOW);
       /* data_fim_parada = getData();
        hora_fim_parada = getHora();
        Serial.println("Data inicio: " + data_inicio_parada);
        Serial.println("Hora inicio: " + hora_inicio_parada);      
        Serial.println("Data fim:   " + data_fim_parada);
        Serial.println("Hora fim:   " + hora_fim_parada);*/
        Serial.println("=========================================");
      }
    }
    delay(75);
  }  

  //Botao MANUTENÇÂO
  botaoManutecao.estado_atual = !digitalRead(botaoManutecao.pin_botao);
  
  if(botaoManutecao.estado_atual != botaoManutecao.ultimo_estado){  
    botaoManutecao.ultimo_estado = botaoManutecao.estado_atual;
    if(botaoManutecao.estado_atual == HIGH){
      if(!botaoManutecao.parado){
        Serial.println("=========================================");
        Serial.println("MAQUINA PARADA!");
        Serial.println("TIPO: MANUTENÇÂO");
        botaoManutecao.parado = true;
        digitalWrite(botaoManutecao.pin_led, HIGH);
       /* data_inicio_parada = getData();
        hora_inicio_parada = getHora();
        Serial.println("Data: " + data_inicio_parada);
        Serial.println("Hora: " + hora_inicio_parada);*/
        Serial.println("=========================================");       
      }else{
        Serial.println("=========================================");
        Serial.println("MAQUINA RETORMADA!");
        //Serial.println("RELATORIO:");
        botaoManutecao.parado = false;
        digitalWrite(botaoManutecao.pin_led, LOW);
       /* data_fim_parada = getData();
        hora_fim_parada = getHora();
        Serial.println("Data inicio: " + data_inicio_parada);
        Serial.println("Hora inicio: " + hora_inicio_parada);      
        Serial.println("Data fim:   " + data_fim_parada);
        Serial.println("Hora fim:   " + hora_fim_parada);*/
        Serial.println("=========================================");
      }
    }
    delay(75);
  }    

}