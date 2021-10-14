#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <esp_task_wdt.h>

#define SSID "trem"
#define PASSWORD "ze127naide"
#define led 17
#define botao 16

hw_timer_t *timer = NULL; 

String data_inicio_parada;
String hora_inicio_parada;
String data_fim_parada;
String hora_fim_parada;
bool estado_atual;
bool ultimo_estado;
bool parado = false;

WiFiClient client;
WiFiUDP udp;
NTPClient ntp(udp, "0.br.pool.ntp.org", -3*3600, 60000); 

void initNTP();
void connectWiFi();
void configureWatchDog();
String getData();
String getHora();
void soft_RESET();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Iniciando...");
  pinMode(led, OUTPUT);
  pinMode(botao, INPUT);

  for(int i=0; i<5; i++){
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);    
  }  
  disableCore0WDT();
  configureWatchDog();
  connectWiFi();
  initNTP();
  Serial.println("Data: " + getData() + " Hora: " + getHora());
  
  ultimo_estado = !digitalRead(botao);
}
//sem pressed é 1, pressed é 0 sem a negaçao
void loop() {
  timerWrite(timer, 0);
  estado_atual = !digitalRead(botao);

  if(estado_atual != ultimo_estado){  
    ultimo_estado = estado_atual;
    if(estado_atual == HIGH){
      if(!parado){
        Serial.println("=========================================");
        Serial.println("MAQUINA PARADA!");
        parado = true;
        digitalWrite(led, HIGH);
        data_inicio_parada = getData();
        hora_inicio_parada = getHora();
        Serial.println("Data: " + data_inicio_parada);
        Serial.println("Hora: " + hora_inicio_parada);
        Serial.println("=========================================");       
      }else{
        Serial.println("=========================================");
        Serial.println("MAQUINA RETORMADA!");
        Serial.println("RELATORIO:");
        parado = false;
        digitalWrite(led, LOW);
        data_fim_parada = getData();
        hora_fim_parada = getHora();
        Serial.println("Data inicio: " + data_inicio_parada);
        Serial.println("Hora inicio: " + hora_inicio_parada);      
        Serial.println("Data fim:   " + data_fim_parada);
        Serial.println("Hora fim:   " + hora_fim_parada);
        Serial.println("=========================================");
      }
    }
    delay(75);
  }
}

void initNTP()
{
    //Inicializa o client NTP
    ntp.begin();
    
    //Espera pelo primeiro update online
    Serial.println("Update do relogio");
    if(!ntp.update()){
        Serial.println("NTP");
        Serial.print(".");
        if(ntp.forceUpdate())
          Serial.println("Update Completo");
        else
          Serial.println("Falha ao realizar update");
    }else
      Serial.println("Update OK");
}

String getData(){
  if(!ntp.update())
    initNTP();

  return ntp.getFormattedDate();
}

String getHora(){
  if(!ntp.update())
    initNTP();

  return ntp.getFormattedTime();
}

void configureWatchDog(){
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &soft_RESET, true); //calback
  timerAlarmWrite(timer, 50000000, true); //10s
  timerAlarmEnable(timer);//habilita a interrupiçao  
}

void connectWiFi(){
  Serial.print("Connecting to ");
  Serial.print(SSID);

  WiFi.begin(SSID, PASSWORD);

  while(WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());  
} 

void IRAM_ATTR soft_RESET(){ //IRAM_ATTR faz a funçao ser chamada mais rapido. joga na RAM
  Serial.println("Reiniciano o sistema...");
  delay(1000);
  ESP.restart();
}