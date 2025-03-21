#include <funcoes.h>

bool gravaPosNVS(String chave, String dado){
    // Acessar a partiçao
    nvs_handle handler_particao_nvs;
    esp_err_t err;
    err = nvs_flash_init_partition("nvs");
    if (err != ESP_OK){
        Serial.println("[ERRO] Falha ao iniciar partição NVS.");           
        return false;
    }
    err = nvs_open_from_partition("nvs", "ns_nvs", NVS_READWRITE, &handler_particao_nvs);
    if (err != ESP_OK){
        Serial.println("[ERRO] Falha ao abrir NVS como escrita/leitura"); 
        return false;
    }
    // Atualiza valor do horimetro total 
    err = nvs_set_str(handler_particao_nvs, chave.c_str(), dado.c_str());
    if (err != ESP_OK){
        Serial.println("[ERRO] Erro ao gravar horimetro");                   
        nvs_close(handler_particao_nvs);
        return false;
    }else{
        Serial.println("Posição gravada com sucesso!");     
        nvs_commit(handler_particao_nvs);    
        nvs_close(handler_particao_nvs);  
        return true;    
    }
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

String lerPosNVS(String chave){
    nvs_handle handler_particao_nvs;
    esp_err_t err;
    size_t required_size;
    char dado_lido[12];
    err = nvs_flash_init_partition("nvs");
    if (err != ESP_OK){
        Serial.println("[ERRO] Falha ao iniciar partição NVS.");         
        return "";
    }
    err = nvs_open_from_partition("nvs", "ns_nvs", NVS_READWRITE, &handler_particao_nvs);
    if (err != ESP_OK){
        Serial.println("[ERRO] Falha ao abrir NVS como escrita/leitura");         
        return "";
    }
    // Faz a leitura do dado associado a chave definida em CHAVE_NVS 
    nvs_get_str(handler_particao_nvs, chave.c_str(), NULL, &required_size);//obtem o tamamnho da string
    err = nvs_get_str(handler_particao_nvs, chave.c_str(), dado_lido, &required_size);

    if (err != ESP_OK){
        Serial.println("[ERRO] Falha ao fazer leitura do dado");         
        return "";
    }else{
        Serial.println("Posição lida com sucesso!");  
        nvs_close(handler_particao_nvs);   
        return dado_lido;
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

void verificaDadosNVS(Botao *botao){
  String str;
  Serial.println("==============================");
  Serial.println("BOTAO " + botao->tipo);
  str = lerPosNVS(botao->chave_di_nvs);
  if(str.equals("")){ //senao conseguir retorna ""
    botao->data_inicio = "";
    gravaPosNVS(botao->chave_di_nvs, "");
    Serial.println("Data inicio: ----/--/--");
    botao->has_init_di = false;
  }else{
    botao->data_inicio = str;
    Serial.println("Data inicio: " + botao->data_inicio);
    botao->has_init_di = true;
  }
  str = lerPosNVS(botao->chave_hi_nvs);
  if(str.equals("")){ //senao conseguir retorna ""
    botao->hora_inicio = "";
    gravaPosNVS(botao->chave_hi_nvs, "");
    Serial.println("Hora inicio: --:--:--");
    botao->has_init_hi = false;
  }else{
    botao->hora_inicio = str;
    Serial.println("Hora inicio: " + botao->hora_inicio);
    botao->has_init_hi = true;
  }
  Serial.println("==============================");
}