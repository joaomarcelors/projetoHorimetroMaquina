#include <funcoes.h>

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

bool enviaDadosPOST(String di, String hi, String df, String hf, int id){
  String tipo;
  if (!isConnectedServer())
    return false;

  if(id == 1)
    tipo = botaoStartStop.tipo;
  else if(id == 2)
    tipo = botaoSetup.tipo;
  else if(id == 3)
    tipo = botaoManutecao.tipo;

  Serial.println("####################################################################");
  Serial.println("Dados a ser enviado: ");
  Serial.println("Data inicio: " + di);
  Serial.println("Hora inicio: " + hi);
  Serial.println("Data fim:   " + df);
  Serial.println("Hora fim:   " + hf);
  Serial.println("TIPO: " + tipo);
  Serial.println("####################################################################");

  //Serial.println(serverName);
  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
  httpRequestData = "key=" + key + "&data_inicio=" + di
   + "&hora_inicio=" + hi +
          "&data_fim=" + df + "&hora_fim=" + hf + "&tipo=" + tipo;
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
  Serial.println(serverName);
  Serial.println(hasInternet());
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

void soft_RESET(){ //IRAM_ATTR faz a funçao ser chamada mais rapido. joga na RAM
  Serial.println("Reiniciano o sistema...");
  delay(1000);
  ESP.restart();
}