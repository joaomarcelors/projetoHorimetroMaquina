#ifndef FUNCOES_H
#define FUNCOES_H
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <FS_File_Record.h>

#define SSID "trem"
#define PASSWORD "ze127naide"
#define host_google "www.google.com"
#define MAX_FAILED 3
#define MAX_DISCONNECTED_TIME 10000
#define LIMIT 4000000000

extern hw_timer_t *timer;
extern const int http_port;
extern String key;
extern String httpRequestData;
extern String serverName;
extern String serverPath;
extern String payload;
extern bool enviouTudo;
extern bool liberado;
extern uint32_t lastTimeConnected;
extern int httpResponseCode;
extern int num_fails;
//extern int sizeOfRecord;
extern String errorMsg;
extern String dados;
extern String linha;

typedef struct{
    byte pin_led;
    byte pin_botao;
    byte id;
    String data_inicio_parada;
    String hora_inicio_parada;
    String data_fim_parada;
    String hora_fim_parada;
    String tipo_parada;
    bool estado_atual;
    bool ultimo_estado;
    bool parado;
    bool solicitouAcesso;
    bool possuiDados;
}Botao;

extern Botao botaoStartStop;
extern Botao botaoSetup;
extern Botao botaoManutecao;
extern HTTPClient http;
extern WiFiClient client;
extern WiFiUDP udp;
extern NTPClient ntp;
extern FS_File_Record ObjFS;
extern IPAddress serverLocal;

void adicionaDadosFila(void *p);
void checkInternet(void *p);
void checkWiFiConnection(void *p);
void configureButtons();
void configureWatchDog();
void connectWiFi();
bool enviaDadosPOST(String dip, String hip, String dfp, String hfp, int id);
String getData();
String getHora();
bool hasInternet();
bool isDisconnectedForTooLong();
bool isConnectedServer();
void initNTP();
void initSPPIFS();
void showAllFiles();
void soft_RESET();
bool solicitaramAcesso();
bool possuemDados();
void verificaBotao(Botao *botao);

#endif 