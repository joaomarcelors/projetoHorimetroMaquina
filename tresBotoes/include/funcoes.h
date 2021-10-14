#ifndef FUNCOES_H
#define FUNCOES_H
#include <Arduino.h>

typedef struct{
    byte pin_led;
    byte pin_botao;
    String data_inicio_parada;
    String hora_inicio_parada;
    String data_fim_parada;
    String hora_fim_parada;
    String tipo_parada;
    bool estado_atual;
    bool ultimo_estado;
    bool parado;
}Botao;
extern Botao botaoStartStop;
extern Botao botaoSetup;
extern Botao botaoManutecao;

void configurar();
#endif 