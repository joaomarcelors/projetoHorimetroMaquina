#include <funcoes.h>

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