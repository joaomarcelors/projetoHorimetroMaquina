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
WiFiClient client; //o pacote de dados vai criptografado
WiFiUDP udp;
NTPClient ntp(udp, "0.br.pool.ntp.org", -3 * 3600, 60000);
FS_File_Record ObjFS("/dhif.bin", sizeOfRecord);
IPAddress serverLocal(192, 168, 3, 4);