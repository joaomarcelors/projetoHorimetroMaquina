#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <FS_File_Record.h> // Nossa lib personalizada do SPIFFS

#define SSID "trem"
#define PASSWORD "ze127naide"
#define led 17
#define botao 16
#define host_google "www.google.com"
#define MAX_FAILED 3
#define MAX_DISCONNECTED_TIME 10000

hw_timer_t *timer = NULL; // faz o controle do controlador
// Site remoto - Coloque aqui os dados do site que vai receber a requisição GET
const int http_port = 443;
String key = "tPmAAWT5Ab3j7F9b";
String httpRequestData;
String serverName;
String serverPath;
String payload;
String data_inicio_parada;
String hora_inicio_parada;
String data_fim_parada;
String hora_fim_parada;
String param;
bool estado_atual;
bool ultimo_estado;
bool parado = false;
bool solicitouAcesso = false;
bool liberado = true;
bool temDados = false;
bool enviouTudo;
uint32_t lastTimeConnected = 0;
int httpResponseCode;
int num_fails;
const uint32_t LIMIT = 4000000000;
// 2021-09-29;17:10:58;2021-09-29;17:11:01
//  Tamanho dos registros
const int sizeOfRecord = 39;
// String que recebe as mensagens de erro
String errorMsg;
String dados;
String linha = "";
// Variável que guarda o último registro obtido

HTTPClient http;
WiFiClient client;
WiFiUDP udp;
NTPClient ntp(udp, "0.br.pool.ntp.org", -3 * 3600, 60000);
// Objeto da nossa lib que recebe o nome do arquivo e tamanho fixo de registro
FS_File_Record ObjFS("/dhif.bin", sizeOfRecord);
// IPAddress serverLocal(192, 168, 3, 107);
IPAddress serverLocal(10, 0, 0, 109);

void adicionaDadosFila(void *p);
void checkInternet(void *p);
void checkWiFiConnection(void *p);
void configureWatchDog();
void connectWiFi();
bool enviaDadosPOST(String dip, String hip, String dfp, String hfp);
String getData();
String getHora();
bool hasInternet();
bool isDisconnectedForTooLong();
bool isConnectedServer();
void initNTP();
void initSPPIFS();
void showAllFiles();
void soft_RESET();

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Iniciando...");
  serverName = "http://" + serverLocal.toString() + ":" + String(http_port) + "/maquina/insert_maquina.php";
  pinMode(led, OUTPUT);
  pinMode(botao, INPUT);

  for (int i = 0; i < 5; i++)
  {
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
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

  Serial.println("Data: " + getData() + " Hora: " + getHora());
  ultimo_estado = !digitalRead(botao);
}
// sem pressed é 1, pressed é 0 sem a negaçao
void loop()
{
  timerWrite(timer, 0);
  estado_atual = !digitalRead(botao);

  if (estado_atual != ultimo_estado)
  {
    ultimo_estado = estado_atual;
    if (estado_atual == 1)
    {
      Serial.println("tem internet? " + String(!isDisconnectedForTooLong()));
      if (!parado)
      {
        Serial.println("=========================================");
        Serial.println("MAQUINA PARADA!");
        parado = true;
        digitalWrite(led, HIGH);
        data_inicio_parada = getData();
        hora_inicio_parada = getHora();
        Serial.println("Data: " + data_inicio_parada);
        Serial.println("Hora: " + hora_inicio_parada);
        Serial.println("=========================================");
      }
      else
      {
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
        dados = data_inicio_parada + "," + hora_inicio_parada + "," + data_fim_parada + "," + hora_fim_parada;
        Serial.println("Dados a serem gravados: " + dados);
        solicitouAcesso = true;
        Serial.print("Aguardando");
        while (!liberado)
        {
          Serial.print(".");
          delay(50);
        }
        Serial.println("");
        liberado = false;
        while (!ObjFS.writeFile(dados, &errorMsg))
        {
          Serial.println(errorMsg);
          delay(50);
        }
        Serial.println("Dados gravado com sucesso!");
        solicitouAcesso = false;
        liberado = true;
        temDados = true;

        if (millis() > LIMIT)
          soft_RESET();
      }
    }
    delay(75);
  }
}

void adicionaDadosFila(void *p)
{
  TickType_t taskDelay = 2000 / portTICK_PERIOD_MS;
  while (true)
  {
    if (WiFi.status() == WL_CONNECTED && temDados)
    {
      if (isConnectedServer())
      {
        if (!solicitouAcesso)
        {
          liberado = false;
          // strcpy(dados_txt, "");
          Serial.println("task usando o SPPIFS");
          Serial.println("Enviando dados para o banco de dados");
          linha = "";
          errorMsg = "0";
          ObjFS.rewind();
          // Exibe todos os registros até o fim
          while (ObjFS.readFileNextRecord(&linha, &errorMsg) && linha != "")
          {
            Serial.println(linha);
            enviouTudo = enviaDadosPOST(linha.substring(0, 10), linha.substring(11, 19), linha.substring(20, 30), linha.substring(31, 39));
            if (!enviouTudo)
              break;
          }
          if (enviouTudo)
          {
            Serial.println("Todos os dados foram enviados!");
            if (ObjFS.destroyFile())
            {
              Serial.println("Arquivo Apagado");
            }
            else
              Serial.println("Falha ao apagar arquivo!");
            temDados = false;
          }
          else
          {
            temDados = true;
          }

          liberado = true;
        }
        else
        {
          Serial.println("Solicitaram acesso!");
        }
      }
      else
      {
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

  while (true)
  {
    if (hasInternet())
    {
      lastTimeConnected = millis();
    }

    vTaskDelay(taskDelay);
  }
}

void checkWiFiConnection(void *p)
{

  TickType_t taskDelay = 1000 / portTICK_PERIOD_MS;

  while (true)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      connectWiFi();
    }
    vTaskDelay(taskDelay);
  }
}

void configureWatchDog()
{
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &soft_RESET, true); // calback
  timerAlarmWrite(timer, 10000000, true);         // 10s
  timerAlarmEnable(timer);                        // habilita a interrupiçao
}

void connectWiFi()
{
  Serial.print("Connecting to ");
  Serial.print(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());
}

bool enviaDadosPOST(String dip, String hip, String dfp, String hfp)
{
  if (!isConnectedServer())
    return false;

  Serial.println("####################################################################");
  Serial.println("Dados a ser enviado: ");
  Serial.println("Data inicio: " + dip);
  Serial.println("Hora inicio: " + hip);
  Serial.println("Data fim:   " + dfp);
  Serial.println("Hora fim:   " + hfp);
  Serial.println("####################################################################");

  Serial.println(serverName);
  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  httpRequestData = "key=" + key + "&data_inicio_parada=" + dip + "&hora_inicio_parada=" + hip +
                    "&data_fim_parada=" + dfp + "&hora_fim_parada=" + hfp; // Parâmetros com as leituras
  httpResponseCode = http.POST(httpRequestData);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode == 200)
  {
    payload = http.getString();
    Serial.println("Payload: " + payload);
    if (payload.equals("OK"))
    {
      Serial.println("DADOS ENVIADO COM SUCESSO AO BANCO DE DADOS!");
      http.end();
      return true;
    }
  }
  else if (httpResponseCode == 404)
  {
    Serial.println("PAGINA NAO ENCONTRADA!");
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  return false;
}

String getData()
{
  if (!isDisconnectedForTooLong())
  {
    if (!ntp.update())
      initNTP();
  }
  return ntp.getFormattedDate();
}

String getHora()
{
  if (!isDisconnectedForTooLong())
  {
    if (!ntp.update())
      initNTP();
  }
  return ntp.getFormattedTime();
}

bool hasInternet()
{
  WiFiClient client;
  // Endreço IP do Google 172.217.3.110
  IPAddress adr;
  // Tempo limite para conexão
  WiFi.hostByName(host_google, adr);
  client.setTimeout(5);
  Serial.print("IP server google: ");
  Serial.println(adr);
  // Tenta conectar
  bool connected = client.connect(adr, http_port);
  // Fecha a conexão
  client.stop();
  // Retorna true se está conectado ou false se está desconectado
  return connected;
}

bool isDisconnectedForTooLong()
{
  // Retorna true se o tempo desde a última conexão for maior que o definido
  return millis() - lastTimeConnected > MAX_DISCONNECTED_TIME;
}

bool isConnectedServer()
{
  bool conectado;
  conectado = client.connect(serverLocal, http_port, 3000);

  if (conectado)
    Serial.println("Conectado com o servidor");
  else
    Serial.println("Falha na conexao com o servidor");

  client.stop();
  return conectado;
}

void initNTP()
{
  // Inicializa o client NTP
  ntp.begin();

  // Espera pelo primeiro update online
  Serial.println("Update do relogio");
  if (!ntp.update())
  {
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

void initSPPIFS()
{
  num_fails = 1;
  while (!ObjFS.init())
  {
    Serial.println("Falha ao iniciar SPPIFS..." + String(num_fails) + "a tentativa...");
    delay(100);
    if (num_fails == MAX_FAILED)
    {
      Serial.println("Falha ao iniciar SPPIFS");
      soft_RESET();
    }
    num_fails++;
  }
  Serial.println("Inicialização da SPPIFS concluída!");
  // Se o arquivo não existe, criamos o arquivo
  if (!ObjFS.fileExists())
  {
    Serial.println("Criando novo aquivo...");
    ObjFS.newFile(); // Cria o arquivo
  }
  else
  {
    Serial.println("Abrindo aquivo existente...");
  }
}

void showAllFiles()
{
  int count = 0;
  String linha = "";

  // Exibe na serial o início do arquivo
  Serial.println("# Begin of file #");
  errorMsg = "";

  // Posiciona o ponteiro do arquivo no início
  ObjFS.rewind();

  // Exibe todos os registros até o fim
  while (ObjFS.readFileNextRecord(&linha, &errorMsg) && linha != "")
  {
    Serial.print(String(count) + " - ");
    Serial.println(linha);
    count++;
  }

  // Se existir mensagem de erro exibe na serial e no display
  if (errorMsg != "")
  {
    Serial.println(errorMsg);
  }

  // Exibe na serial e no display o fim do arquivo
  Serial.println("# End of file #");
}

void soft_RESET()
{ // IRAM_ATTR faz a funçao ser chamada mais rapido. joga na RAM
  Serial.println("Reiniciano o sistema...");
  delay(1000);
  ESP.restart();
}