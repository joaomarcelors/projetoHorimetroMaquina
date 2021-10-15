#include <funcoes.h>

hw_timer_t *timer = NULL;
const int http_port = 443;
String key = "tPmAAWT5Ab3j7F9b";
String httpRequestData;
String serverName;
String serverPath;
String payload;
bool enviou;
bool liberado = true;
uint32_t lastTimeConnected = 0;
int posicao = 0; //pode ser usado la na frente
int httpResponseCode;
int num_fails;
//2021-09-29;17:10:58;2021-09-29;17:11:01;0
// Tamanho dos registros
const int sizeOfRecord = 41;
String errorMsg;
String dados;
String linha = "";

Botao botaoStartStop;
Botao botaoSetup;
Botao botaoManutecao;

HTTPClient http;
WiFiClient client;
WiFiUDP udp;
NTPClient ntp(udp, "0.br.pool.ntp.org", -3 * 3600, 60000);
FS_File_Record ObjFS("/dhif.bin", sizeOfRecord);
IPAddress serverLocal(10, 0, 0, 109);

//botoes 15, 4, 16 
//led 17, 18, 19

void configureButtons(){
    //Botao START/STOP
    botaoStartStop.pin_botao = 15;
    botaoStartStop.pin_led = 25;
    botaoStartStop.id = 1;
    pinMode(botaoStartStop.pin_botao, INPUT);
    pinMode(botaoStartStop.pin_led, OUTPUT);
    botaoStartStop.estado_atual = digitalRead(botaoStartStop.pin_botao);
    botaoStartStop.ultimo_estado = botaoStartStop.estado_atual;
    botaoStartStop.parado = false;
    botaoStartStop.solicitouAcesso = false;
    botaoStartStop.possuiDados = false;
    botaoStartStop.tipo_parada = "START/STOP";

    //Botao SETUP
    botaoSetup.pin_botao = 4;
    botaoSetup.pin_led = 33;
    botaoSetup.id = 2;
    pinMode(botaoSetup.pin_botao, INPUT);
    pinMode(botaoSetup.pin_led, OUTPUT);
    botaoSetup.estado_atual = digitalRead(botaoSetup.pin_botao);
    botaoSetup.ultimo_estado = botaoSetup.estado_atual;
    botaoSetup.parado = false;
    botaoSetup.solicitouAcesso = false;
    botaoSetup.possuiDados = false;
    botaoSetup.tipo_parada = "SETUP";

    //Botao Manuteção
    botaoManutecao.pin_botao = 16;
    botaoManutecao.pin_led = 32;
    botaoManutecao.id = 3;
    pinMode(botaoManutecao.pin_botao, INPUT);
    pinMode(botaoManutecao.pin_led, OUTPUT);
    botaoManutecao.estado_atual = digitalRead(botaoManutecao.pin_botao);
    botaoManutecao.ultimo_estado = botaoManutecao.estado_atual;
    botaoManutecao.parado = false;
    botaoManutecao.solicitouAcesso = false;
    botaoManutecao.possuiDados = false;
    botaoManutecao.tipo_parada = "MANUTENCAO";
}

void configureWatchDog(){
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &soft_RESET, true); //calback
  timerAlarmWrite(timer, 10000000, true);         //10s
  timerAlarmEnable(timer);                        //habilita a interrupiçao
}

void connectWiFi(){
  Serial.print("Connecting to ");
  Serial.print(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());
}

bool enviaDadosPOST(String dip, String hip, String dfp, String hfp, int id){
  String tipo;
  if (!isConnectedServer())
    return false;

  if(id == 1)
    tipo = botaoStartStop.tipo_parada;
  else if(id == 2)
    tipo = botaoSetup.tipo_parada;
  else if(id == 3)
    tipo = botaoManutecao.tipo_parada;

  Serial.println("####################################################################");
  Serial.println("Dados a ser enviado: ");
  Serial.println("Data inicio: " + dip);
  Serial.println("Hora inicio: " + hip);
  Serial.println("Data fim:   " + dfp);
  Serial.println("Hora fim:   " + hfp);
  Serial.println("TIPO: " + tipo);
  Serial.println("####################################################################");

  //Serial.println(serverName);
  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
  httpRequestData = "key=" + key + "&data_inicio_parada=" + dip + "&hora_inicio_parada=" + hip +
          "&data_fim_parada=" + dfp + "&hora_fim_parada=" + hfp + "&tipo=" + tipo;
  httpResponseCode = http.POST(httpRequestData);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  if(httpResponseCode == 200){
    payload = http.getString();
    Serial.println("Payload: " + payload);
    if(payload.equals("OK")){
      Serial.println("DADOS ENVIADO COM SUCESSO AO BANCO DE DADOS!");
      http.end();
      return true;
    }
  }else if(httpResponseCode == 404){
    Serial.println("PAGINA NAO ENCONTRADA!");
  }else{
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  return false;
}

String getData(){
  if (!isDisconnectedForTooLong()){
    if (!ntp.update())
      initNTP();
  }
  return ntp.getFormattedDate();
}

String getHora(){
  if (!isDisconnectedForTooLong()){
    if (!ntp.update())
      initNTP();
  }
  return ntp.getFormattedTime();
}

bool hasInternet(){
  WiFiClient client;
  //Endreço IP do Google 172.217.3.110
  IPAddress adr;
  //Tempo limite para conexão
  WiFi.hostByName(host_google, adr);
  client.setTimeout(5);
  Serial.print("IP server google: ");
  Serial.println(adr);
  //Tenta conectar
  bool connected = client.connect(adr, http_port);
  //Fecha a conexão
  client.stop();
  //Retorna true se está conectado ou false se está desconectado
  return connected;
}

bool isDisconnectedForTooLong(){
  //Retorna true se o tempo desde a última conexão for maior que o definido
  return millis() - lastTimeConnected > MAX_DISCONNECTED_TIME;
}

bool isConnectedServer(){
  bool conectado;
  conectado = client.connect(serverLocal, http_port, 3000);

  if (conectado)
    Serial.println("Conectado com o servidor");
  else
    Serial.println("Falha na conexao com o servidor");

  client.stop();
  return conectado;
}

void initNTP(){
  //Inicializa o client NTP
  ntp.begin();

  //Espera pelo primeiro update online
  Serial.println("Update do relogio");
  if (!ntp.update()){
    Serial.println("NTP");
    Serial.print(".");
    if (ntp.forceUpdate())
      Serial.println("Update Completo");
    else
      Serial.println("Falha ao realizar update");
  }
  else
    Serial.println("Update OK");
}

void initSPPIFS(){
  num_fails = 1;
  while (!ObjFS.init()){
    Serial.println("Falha ao iniciar SPPIFS..." + String(num_fails) + "a tentativa...");
    delay(100);
    if (num_fails == MAX_FAILED){
      Serial.println("Falha ao iniciar SPPIFS");
      soft_RESET();
    }
    num_fails++;
  }
  Serial.println("Inicialização da SPPIFS concluída!");
  // Se o arquivo não existe, criamos o arquivo
  if (!ObjFS.fileExists()){
    Serial.println("Criando novo aquivo...");
    ObjFS.newFile(); // Cria o arquivo
  }else{
    Serial.println("Abrindo aquivo existente...");
  }
}

void showAllFiles(){
  int count = 0;
  String linha = "";

  // Exibe na serial o início do arquivo
  Serial.println("# Begin of file #");
  errorMsg = "";

  // Posiciona o ponteiro do arquivo no início
  ObjFS.rewind();

  // Exibe todos os registros até o fim
  while (ObjFS.readFileNextRecord(&linha, &errorMsg) && linha != ""){
    Serial.print(String(count) + " - ");
    Serial.println(linha);
    count++;
  }

  // Se existir mensagem de erro exibe na serial e no display
  if (errorMsg != ""){
    Serial.println(errorMsg);
  }

  // Exibe na serial e no display o fim do arquivo
  Serial.println("# End of file #");
}

void soft_RESET(){ //IRAM_ATTR faz a funçao ser chamada mais rapido. joga na RAM
  Serial.println("Reiniciano o sistema...");
  delay(1000);
  ESP.restart();
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
      if(!botao->parado){
        Serial.println("=========================================");
        if(botao->id == 1) //operador da maquina
          Serial.println("OPERADOR COMEÇOU A USAR A MÁQUINA!");
        else
          Serial.println("MAQUINA PARADA!");
        botao->parado = true;
        digitalWrite(botao->pin_led, HIGH);
        botao->data_inicio_parada = getData();
        botao->hora_inicio_parada = getHora();
        Serial.println("TIPO: " + botao->tipo_parada);
        Serial.println("Data: " + botao->data_inicio_parada);
        Serial.println("Hora: " + botao->hora_inicio_parada);
        Serial.println("=========================================");       
      }else{
        Serial.println("=========================================");
        if(botao->id == 1)
          Serial.println("OPERADOR TERMINOU DE USAR A MÁQUINA!");
        else
          Serial.println("MAQUINA RETORMADA!");
        botao->parado = false;
        digitalWrite(botao->pin_led, LOW);
        botao->data_fim_parada = getData();
        botao->hora_fim_parada = getHora();
        Serial.println("TIPO: " + botao->tipo_parada);
        Serial.println("RELATORIO:");
        Serial.println("Data inicio: " + botao->data_inicio_parada);
        Serial.println("Hora inicio: " + botao->hora_inicio_parada);      
        Serial.println("Data fim:   " + botao->data_fim_parada);
        Serial.println("Hora fim:   " + botao->hora_fim_parada);
        Serial.println("=========================================");
        dados = botao->data_inicio_parada + "," + botao->hora_inicio_parada + "," + botao->data_fim_parada + "," + botao->hora_fim_parada + "," + String(botao->id);
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