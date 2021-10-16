#include <funcoes.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Inciando...");
  serverName = "http://" + serverLocal.toString() + ":" + String(http_port)+ "/maquina/insert_maquina.php";
  configureButtons();

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

  xTaskCreatePinnedToCore(
        checkWiFiConnection,
        "checkWiFiConnection",
        10000,
        NULL,
        2,
        NULL,
        0);

  delay(500);

  xTaskCreatePinnedToCore(
        checkInternet,
        "checkInternet",
        10000,
        NULL,
        2,
        NULL,
        0);

  delay(500);

  xTaskCreatePinnedToCore(
        adicionaDadosFila,
        "adicionaDadosFila",
        10000,
        NULL,
        3,
        NULL,
        0);

  delay(500);

  disableCore0WDT();
  configureWatchDog();
  connectWiFi();
  initSPPIFS();
  initNTP();
  verificaDadosNVS(&botaoStartStop);
  verificaDadosNVS(&botaoSetup);
  verificaDadosNVS(&botaoManutecao);
  Serial.println("Nesse instante, Data: " + getData() + " Hora: " + getHora());
}

void loop() {
  timerWrite(timer, 0);

  verificaBotao(&botaoStartStop);
  verificaBotao(&botaoSetup);
  verificaBotao(&botaoManutecao);
}

void adicionaDadosFila(void *p)
{
  TickType_t taskDelay = 2000 / portTICK_PERIOD_MS;
  while (true){
    if(WiFi.status() == WL_CONNECTED && possuemDados()){
      if (isConnectedServer()){
        if (!solicitaramAcesso()){
          liberado = false;
          //strcpy(dados_txt, "");
          Serial.println("task usando o SPPIFS");
          Serial.println("Enviando dados para o banco de dados");
          linha = "";
          errorMsg = "0";
          ObjFS.rewind();
          // Exibe todos os registros até o fim
          while (ObjFS.readFileNextRecord(&linha, &errorMsg) && linha != ""){
          Serial.println(linha);
          enviou = enviaDadosPOST(linha.substring(0, 10), linha.substring(11, 19), linha.substring(20, 30), linha.substring(31, 39), linha.substring(40, 41).toInt());
          
            if(enviou){
              posicao++;
            }else{
              break;
            }
          }
          if(enviou){
            Serial.println("Todos os dados foram enviados!");
            if (ObjFS.destroyFile()){
              Serial.println("Arquivo Apagado");
            }else
              Serial.println("Falha ao apagar arquivo!");
            botaoStartStop.possuiDados = false;
            botaoSetup.possuiDados = false;
            botaoManutecao.possuiDados = false;
          }else{
            botaoStartStop.possuiDados = true;
            botaoSetup.possuiDados = true;
            botaoManutecao.possuiDados = true;
          }
          
          liberado = true;
        }
        else{
          Serial.println("Solicitaram acesso!");
        }
      }else{
        Serial.println("Dados na fila:");
        showAllFiles();
      }
    }
    vTaskDelay(taskDelay);
  }
}

void checkInternet(void *p)
{
  TickType_t taskDelay = 5000 / portTICK_PERIOD_MS;

  while (true){
    if (hasInternet()){
      lastTimeConnected = millis();
    }

    vTaskDelay(taskDelay);
  }
}

void checkWiFiConnection(void *p){

  TickType_t taskDelay = 1000 / portTICK_PERIOD_MS;

  while (true){
    if (WiFi.status() != WL_CONNECTED){
      connectWiFi();
    }
    vTaskDelay(taskDelay);
  }
}

