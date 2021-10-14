#include <funcoes.h>

Botao botaoStartStop;
Botao botaoSetup;
Botao botaoManutecao;

//botoes 15, 4, 16 
//led 17, 18, 19

void configurar(){
    //Botao START/STOP
    botaoStartStop.pin_botao = 15;
    botaoStartStop.pin_led = 17;
    pinMode(botaoStartStop.pin_botao, INPUT);
    pinMode(botaoStartStop.pin_led, OUTPUT);
    botaoStartStop.estado_atual = digitalRead(botaoStartStop.pin_botao);
    botaoStartStop.ultimo_estado = botaoStartStop.estado_atual;
    botaoStartStop.parado = false;

    //Botao SETUP
    botaoSetup.pin_botao = 4;
    botaoSetup.pin_led = 18;
    pinMode(botaoSetup.pin_botao, INPUT);
    pinMode(botaoSetup.pin_led, OUTPUT);
    botaoSetup.estado_atual = digitalRead(botaoSetup.pin_botao);
    botaoSetup.ultimo_estado = botaoSetup.estado_atual;
    botaoStartStop.parado = false;

    //Botao Manuteção
    botaoManutecao.pin_botao = 16;
    botaoManutecao.pin_led = 19;
    pinMode(botaoManutecao.pin_botao, INPUT);
    pinMode(botaoManutecao.pin_led, OUTPUT);
    botaoManutecao.estado_atual = digitalRead(botaoManutecao.pin_botao);
    botaoManutecao.ultimo_estado = botaoManutecao.estado_atual;
    botaoStartStop.parado = false;
}

void verificaBotao(Botao *botao, String tipo){
  botao->estado_atual = !digitalRead(botao->pin_botao);
  
  if(botao->estado_atual != botao->ultimo_estado){  
    botao->ultimo_estado = botao->estado_atual;
    if(botao->estado_atual == HIGH){
      if(!botao->parado){
        Serial.println("=========================================");
        Serial.println("MAQUINA PARADA!");
        Serial.println("TIPO: " + tipo);
        botao->parado = true;
        digitalWrite(botao->pin_led, HIGH);
       /* data_inicio_parada = getData();
        hora_inicio_parada = getHora();
        Serial.println("Data: " + data_inicio_parada);
        Serial.println("Hora: " + hora_inicio_parada);*/
        Serial.println("=========================================");       
      }else{
        Serial.println("=========================================");
        Serial.println("MAQUINA RETORMADA!");
        //Serial.println("RELATORIO:");
        botao->parado = false;
        digitalWrite(botao->pin_led, LOW);
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