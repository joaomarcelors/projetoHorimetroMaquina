#include <funcoes.h>

void configureButtons(){
    //Botao START/STOP
    botaoStartStop.pin_botao = 15;
    botaoStartStop.pin_led = 25;
    botaoStartStop.id = 1;
    pinMode(botaoStartStop.pin_botao, INPUT);
    pinMode(botaoStartStop.pin_led, OUTPUT);
    botaoStartStop.estado_atual = digitalRead(botaoStartStop.pin_botao);
    botaoStartStop.ultimo_estado = botaoStartStop.estado_atual;
    botaoStartStop.ativado = false;
    botaoStartStop.solicitouAcesso = false;
    botaoStartStop.possuiDados = false;
    botaoStartStop.tipo = "START/STOP";
    botaoStartStop.chave_di_nvs = "bss_di";
    botaoStartStop.chave_hi_nvs = "bss_hi";

    //Botao SETUP
    botaoSetup.pin_botao = 4;
    botaoSetup.pin_led = 33;
    botaoSetup.id = 2;
    pinMode(botaoSetup.pin_botao, INPUT);
    pinMode(botaoSetup.pin_led, OUTPUT);
    botaoSetup.estado_atual = digitalRead(botaoSetup.pin_botao);
    botaoSetup.ultimo_estado = botaoSetup.estado_atual;
    botaoSetup.ativado = false;
    botaoSetup.solicitouAcesso = false;
    botaoSetup.possuiDados = false;
    botaoSetup.tipo = "SETUP";
    botaoSetup.chave_di_nvs = "bsetup_di";
    botaoSetup.chave_hi_nvs = "bsetup_hi";

    //Botao Manuteção
    botaoManutecao.pin_botao = 16;
    botaoManutecao.pin_led = 32;
    botaoManutecao.id = 3;
    pinMode(botaoManutecao.pin_botao, INPUT);
    pinMode(botaoManutecao.pin_led, OUTPUT);
    botaoManutecao.estado_atual = digitalRead(botaoManutecao.pin_botao);
    botaoManutecao.ultimo_estado = botaoManutecao.estado_atual;
    botaoManutecao.ativado = false;
    botaoManutecao.solicitouAcesso = false;
    botaoManutecao.possuiDados = false;
    botaoManutecao.tipo = "MANUTENCAO";
    botaoManutecao.chave_di_nvs = "bmntc_di";
    botaoManutecao.chave_hi_nvs = "bmntc_hi";
}

bool solicitaramAcesso(){
  return botaoStartStop.solicitouAcesso || botaoSetup.solicitouAcesso || botaoManutecao.solicitouAcesso;
}

bool possuemDados(){
    return botaoStartStop.possuiDados || botaoSetup.possuiDados || botaoManutecao.possuiDados;
}

void verificaBotao(Botao *botao){
  botao->estado_atual = !digitalRead(botao->pin_botao);
  
  if(botao->estado_atual != botao->ultimo_estado){  
    botao->ultimo_estado = botao->estado_atual;
    if(botao->estado_atual == HIGH){
      if(!botao->ativado){
        Serial.println("=========================================");
        if(botao->id == 1) //operador da maquina
          Serial.println("OPERADOR COMEÇOU A USAR A MÁQUINA!");
        else
          Serial.println("MAQUINA PARADA!");
        botao->ativado = true;
        digitalWrite(botao->pin_led, HIGH);
        botao->data_inicio = getData();
        botao->hora_inicio = getHora();
        Serial.println("TIPO: " + botao->tipo);
        Serial.println("Data: " + botao->data_inicio);
        Serial.println("Hora: " + botao->hora_inicio);
        Serial.println("=========================================");       
      }else{
        Serial.println("=========================================");
        if(botao->id == 1)
          Serial.println("OPERADOR TERMINOU DE USAR A MÁQUINA!");
        else
          Serial.println("MAQUINA RETORMADA!");
        botao->ativado = false;
        digitalWrite(botao->pin_led, LOW);
        botao->data_fim = getData();
        botao->hora_fim = getHora();
        Serial.println("TIPO: " + botao->tipo);
        Serial.println("RELATORIO:");
        Serial.println("Data inicio: " + botao->data_inicio);
        Serial.println("Hora inicio: " + botao->hora_inicio);      
        Serial.println("Data fim:   " + botao->data_fim);
        Serial.println("Hora fim:   " + botao->hora_fim);
        Serial.println("=========================================");
        dados = botao->data_inicio + "," + botao->hora_inicio + "," + botao->data_fim + "," + botao->hora_fim + "," + String(botao->id);
        Serial.println("Dados a serem gravados: " + dados);
        botao->solicitouAcesso = true;
        Serial.print("Aguardando");
        while (!liberado){
          Serial.print(".");
          delay(50);
        }
        Serial.println("");
        liberado = false;
        while (!ObjFS.writeFile(dados, &errorMsg)){
          Serial.println(errorMsg);
          delay(50);
        }
        Serial.println("Dados gravado com sucesso!");
        botao->solicitouAcesso = false;
        liberado = true;
        botao->possuiDados = true;

        if (millis() > LIMIT)
          soft_RESET();        
      }
    }
    delay(75);
  }     
}