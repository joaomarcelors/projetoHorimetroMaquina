#include <funcoes.h>
Botao botaoStartStop;
Botao botaoSetup;
Botao botaoManutecao;
//botoes 15, 4, 16 
//led 17, 18, 19
void configurar(){
    //Botao START/STOP
    botaoStartStop.pin_botao = 15;
    botaoStartStop.pin_led = 17;
    pinMode(botaoStartStop.pin_botao, INPUT);
    pinMode(botaoStartStop.pin_led, OUTPUT);
    botaoStartStop.estado_atual = digitalRead(botaoStartStop.pin_botao);
    botaoStartStop.ultimo_estado = botaoStartStop.estado_atual;

    //Botao SETUP
    botaoSetup.pin_botao = 4;
    botaoSetup.pin_led = 18;
    pinMode(botaoSetup.pin_botao, INPUT);
    pinMode(botaoSetup.pin_led, OUTPUT);
    botaoSetup.estado_atual = digitalRead(botaoSetup.pin_botao);
    botaoSetup.ultimo_estado = botaoSetup.estado_atual;

    //Botao Manuteção
    botaoManutecao.pin_botao = 16;
    botaoManutecao.pin_led = 19;
    pinMode(botaoManutecao.pin_botao, INPUT);
    pinMode(botaoManutecao.pin_led, OUTPUT);
    botaoManutecao.estado_atual = digitalRead(botaoManutecao.pin_botao);
    botaoManutecao.ultimo_estado = botaoManutecao.estado_atual;
}