#ifndef FUNCOES_H
#define FUNCOES_H
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <FS_File_Record.h>
#include "nvs_flash.h" 

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
extern bool enviou;
extern bool liberado;
extern uint32_t lastTimeConnected;
extern int httpResponseCode;
extern int num_fails;
extern int posicao;
//extern int sizeOfRecord;
extern String errorMsg;
extern String dados;
extern String linha;

typedef struct{
    byte pin_led;
    byte pin_botao;
    byte id;
    String data_inicio;
    String hora_inicio;
    String data_fim;
    String hora_fim;
    String tipo;
    String chave_di_nvs;
    String chave_hi_nvs;
    bool estado_atual;
    bool ultimo_estado;
    bool ativado;
    bool solicitouAcesso;
    bool possuiDados;
    bool has_init_di;
    bool has_init_hi;
}Botao;

extern Botao botaoStartStop;
extern Botao botaoSetup;
extern Botao botaoManutecao;
extern HTTPClient http;
extern WiFiClientSecure client;
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
bool enviaDadosPOST(String di, String hi, String df, String hf, int id);
String getData();
String getHora();
bool gravaPosNVS(String chave, String dado);
bool hasInternet();
bool isDisconnectedForTooLong();
bool isConnectedServer();
void initNTP();
void initSPPIFS();
String lerPosNVS(String chave);
void showAllFiles();
void soft_RESET();
bool solicitaramAcesso();
bool possuemDados();
void verificaBotao(Botao *botao);
void verificaDadosNVS(Botao *botao);

#endif 