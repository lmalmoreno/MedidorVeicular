/* 
################# ENGENHARIA ELÉTRICA UNIP BAURU #################         #### ESTE PROGRAMA PERTENCE AO GRUPO ####
#PROJETO MEDIDOR DE VELOCIDADE VEICULAR - APS 7°/8° SEMESTRE 2013#         #####  OS INTEGRANTES DO GRUPO SÃO  #####
#***************** VERSÃO (DATA - MODIFICAÇÃO) ******************#         #---------------------------------------#
#0.01A (14/09/13) - Criação do código de medição do tempo de     #         #         Lucas Maldonado Moreno        #
#      leitura das portas                                        #         #          Adriano de Oliveira          #
#0.02A (17/09/13) - Adição do cabeçalho e dos comentários        #         #     Fabio Alexandre Lucas Mariano     #
#0.03A (19/09/13) - Adição do processamento da velocidade        #         #      Lucas Trevizi Martins Vieira     #
#0.04A (19/10/13) - Adição dos displays de 7 segmentos           #         #         Marcelo Augusto Firmino       #
#0.05A (23/10/13) - Correção do cálculo  da velocidade           #         #           Renan Amador Batista        #
#0.06A (25/10/13) - Correção da exibição dos displays            #         #          Roberson Antônio Manoel      #
#0.07A (25/10/13) - Várias correções gerais                      #         #            Rodrigo Pavan Lima         #                 
#1.00B (26/10/13) - Programa passa para fase Beta para testes e  #         #        Sidnei Veiga de Oliveira       #
#                   melhorias                                    #         #---------------------------------------#
#1.01B (12/11/13) - Adição do modo de testes e correção de erros #         # PROGRAMA LICENCIADO CREATIVE COMMONS  #
#1.02B (12/11/13) - Adição do flash                              #         #  Atribuição-NãoComercial-Compartilha  #
#1.03B (03/01/14) - Adição do watchdog                           #         #        Igual 4.0 Internacional        #
#1.04B (04/01/14) - Correção nos tempos de watchdog              #         #           (CC BY-NC-SA 4.0)           #
#1.0F  (05/01/14) - Programa passa para fase final e está pronto #         #     PARA MAIS INFORMAÇÕES VISITE:     #
#                   para uso, qualquer melhoria será atualização #         #          http://goo.gl/pwda2A         #
#                                                                #         #########################################
# Para facilitar a resolução de erros e ficar de uma forma mais  #
# fácil de entender, foram criados grupos de linhas de comandos  #
# que estão no comentario na frente de cada linha conforme tabe  #
# la abaixo:                                                     #
#  [Z] - Presente somente na fase Alfa/Beta                      #
#  [A] - Medição do tempo de acionamento dos sensores            #
#  [B] - Obtenção da velocidade                                  #
#  [C] - Programação do display de 7 segmentos                   #
#  [D] - Inicialização e testes do sistema                       #
#  [E] - Análise da velocidade para acionamento do flash         #
#  [F] - Adição de watchdog para correção de travamentos         #
#                                                                #
##################################################################
*/

#include <Arduino.h>
#include <avr/wdt.h> //[F] Inclusão da biblioteca do watchdog
#include <LED8SegDisplay595.h> //[C] Inclusão da biblioteca que trata os displays
#define latchPin 8  //[C] Pino conectado a  saída 8 (Latch)
#define clockPin 12 //[C] Pino conectado a  saída 12 (Clock)
#define dataPin 11  //[C] Pino conectado a  saída 11 (Data)
#define animacao  reverseNumbers() //[C] Indica que a rotina de animação escreve reversamente nos displays


LED8SegDisplay595 ls(2,clockPin,latchPin,dataPin); //[C] Define a quantidade de displays e os pinos de comunicação com os 74HC595


int psens = 2; //[A] Número inteiro (porta)
int ssens = 3; //[A] Número inteiro (porta)
int lpsens = 9; //[D] Número inteiro (porta)
int lssens = 10; //[D] Número inteiro (porta)
int flash = 2; //[E] Número inteiro (porta)
unsigned long tmp1, tmp2; //[A] Variáveis de tamanho ampliado (tempos positivos)
float tmp, dt;// [A] Variável de número fracionário (tempo processado e delta t)
int val1; //[A] Numero inteiro (leitura do sensor psens)
int val2; //[A] Numero inteiro (leitura do sensor ssens)
int vel = 0; //[B] Variável de números inteiros para armazenar a velocidade
float dist = 0.5; //[B] Variável de número fracionário para distancia entre os sensores (em metros)  ***************************
int tex = 1500; //[C] Define o tempo de duração da exibição da velocidade nos displays (em ms)       **** VALORES QUE DEVEM ****       
int vflash = 15; //[E] Velocidade em Km/h em que o flash passa a ser acionado                        ****  SER CONFIGURADOS ****
int tflash = 100; //[E] Tempo em ms em que o flash fica aceso                                        ***************************

// !!!!!!!!!!  ATENÇÂO: Ao mudar os valores tex, vflash ou tflash verifique o tempo de watchdog !!!!!!!!!!

void setup()

{
  wdt_enable(WDTO_8S); //[F] Habilita watchdog para 8 segundos
  wdt_reset(); //[F] Reseta watchdog informando que o programa está ok
  // Serial.begin(9600); //[Z] Abre comunicação serial
  pinMode (psens, INPUT); //[A] Define o tipo do pino como entrada
  pinMode (ssens, INPUT); //[A] Define o tipo do pino como entrada
  pinMode (lpsens, OUTPUT); //[D] Define o tipo do pino como saída
  pinMode (lssens, OUTPUT); //[D] Define o tipo do pino como saída
  pinMode (flash, OUTPUT); //[E] Define o tipo do pino como saída
  
  // INICIO DOS TESTES DO CIRCUITO 
  ls.setDisplayMask(0);   //
  ls.setDisplayMask(1);   //
  ls.write(88);           // [D] Limpa os displays
  ls.clearDisplayMask(0); //
  ls.clearDisplayMask(1); //

  ls.write(88); //[D] Acende todos os segmentos dos displays por 4 segundos
  digitalWrite(lpsens, HIGH); //[D] Acende o LED de status do sensor por 4 segundos
  digitalWrite(lssens, HIGH); //[D] Acende o LED de status do sensor por 4 segundos
  delay(4000);
  digitalWrite(lpsens, LOW); //[D] Apaga o LED de status do sensor
  digitalWrite(lssens, LOW); //[D] Apaga o LED de status do sensor
  ls.setDisplayMask(0);   //
  ls.setDisplayMask(1);   //
  ls.write(88);           // [D] Apaga os segmentos dos displays
  ls.clearDisplayMask(0); //
  ls.clearDisplayMask(1); //
  wdt_reset(); //[F] Reseta watchdog informando que o programa está ok
  // FIM DOS TESTES DO CIRCUITO
  
  // INICIO DA APRESENTAÇÃO DO SISTEMA
  ls.setDisplayMask(0); //[D] Muda o tipo do primeiro digito  
  ls.setDisplayMask(1); //[D] Muda o tipo do segundo digito 
  ls.write(18); //[D] Escreve nos displays
  delay(1000);  //[D] Aguarda o tempo definido (em ms)
  ls.write(11); //[D] Escreve nos displays                                   
  delay(1000);  //[D] Aguarda o tempo definido (em ms)                 **** ¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡ ****                              
  ls.write(81); //[D] Escreve nos displays                           **** ANIMAÇÃO "EE" (ENGENHARIA ELÉTRICA) ****                       
  delay(1000);  //[D] Aguarda o tempo definido (em ms)                 **** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****
  ls.write(88); //[D] Escreve nos displays
  delay(1000);  //[D] Aguarda o tempo definido (em ms)
  ls.write(11); //[D] Escreve nos displays
  delay(1500);  //[D] Aguarda o tempo definido (em ms)
  ls.write(88); //[D] Escreve nos displays
  ls.clearDisplayMask(0);  //[D] Volta o tipo do primeiro digito
  ls.clearDisplayMask(1);  //[D] Volta o tipo do segundo digito 
  wdt_reset(); //[F] Reseta watchdog informando que o programa está ok
  // FIM DA APRESENTAÇÃO DO SISTEMA
  
}



void animacao //[C] Rotina da animação de espera
{
  wdt_enable(WDTO_8S); //[F] Habilita watchdog para 8 segundos
  wdt_reset(); //[F] Reseta watchdog informando que o programa está ok
  ls.setDisplayMask(0); //[C] Muda o tipo do primeiro digito  
  ls.setDisplayMask(1); //[C] Muda o tipo do segundo digito 
  ls.write(18); //[C] Escreve nos displays
  delay(1000);  //[C] Aguarda o tempo definido (em ms)
  ls.write(11); //[C] Escreve nos displays
  delay(1000);  //[C] Aguarda o tempo definido (em ms)         **** APESAR DE NÃO FUNCIONAR O CÓDIGO DA ANIMAÇÃO ESTÁ PRESENTE ****           
  ls.write(81); //[C] Escreve nos displays                                    **** PODE SER FUTURA IMPLEMENTAÇÃO ****
  delay(1000);  //[C] Aguarda o tempo definido (em ms)
  ls.write(88); //[C] Escreve nos displays
  delay(1000);  //[C] Aguarda o tempo definido (em ms)
  ls.clearDisplayMask(0);  //[C] Volta o tipo do primeiro digito
  ls.clearDisplayMask(1);  //[C] Volta o tipo do segundo digito 
  wdt_reset(); //[F] Reseta watchdog informando que o programa está ok
}




void loop()
{
  wdt_disable(); //[F] Desabilita watchdog para uso do while
  // Serial.println("Aguardando..."); //[Z] Escreve "Aguardando..." na porta serial
 
  val1 = analogRead (psens); //[A] Atribui a variável val1 o valor do psens
  val2 = analogRead (ssens); //[A] Atribui a variável val2 o valor do ssens
  
  while (val2 >= 100){ //[A] Não deixa o sistema acionar se os sensores forem ativados inversamente
   
  while (val1 < 100) //[A] Analisa val1 e enquanto ela for menor que 100
  {                           
    val1 = analogRead(psens); //[A] Armazena o psens em val1
  }
  
  while (val1 >= 100) //[A] Analisa val1 e quando ela for maior ou igual a 100
  {
    tmp1 = micros (); //[A] Armazena o valor de micros em tmp1
    val1 = analogRead(psens); //[A] Armazena o valor do psens em val1
  }
  
  digitalWrite(lpsens, HIGH); //[D] Acende o LED de status

  while (val2 < 100) //[A] Analisa val2 e enquanto ela for menor que 100
  {
    val2 = analogRead(ssens); //[A] Armazena o ssens em val2
  }
  
  while (val2 >=100) //[A] Analisa val2 e quando ela for maior ou igual a 100
  {
    tmp2 = micros(); //[A] Armazena o valor de micros em tmp2
    val2 = analogRead(ssens); //[A] Armazena o valor do ssens em val2
  }
  
  digitalWrite(lssens, HIGH); //[D] Acende o LED de status
  
  }
  
  wdt_enable(WDTO_4S); //[F] Habilita watchdog para 4 segundos
  wdt_reset(); //[F] Reseta watchdog informando que o programa está ok
  
  dt = tmp2 - tmp1;  //[A] Faz a subtração para obter a diferença de tempo entre o acionamento dos dois sensores 
  
  tmp = dt/1000000 ; //[A] Converte de micro segundos para segundos
  
  vel = (dist/tmp)*3.6; //[B] Cálculo da velocidade (em Km/h)
  
  // Serial.println(vel); //[Z] Escreve a velocidade obtida na porta serial
  
 
  if (vel <= 150) //[C] Prevenção de erros, não deixa o sistema exibir nada se vel for maior que 150
  {
  ls.write (vel); //[C] Apresenta no display o valor da velocidade
  }
  
  if (vel <= 150) // [E] Verifica se a velocidade obtida está dentro da faixa de trabalho
  {
    if (vel >= vflash) // [E] Verifica se a velocidade obtida é maior ou igual a velocidade de acionamento do flash
    {
      digitalWrite(flash, HIGH); 
      delay (tflash);
      digitalWrite(flash, LOW); 
    }
  }
  
  delay (tex-tflash); //[C] Tempo de exibição da velocidade (em ms)
  
  ls.setDisplayMask(0);   //
  ls.setDisplayMask(1);   //
  ls.write(88);           // [C] Apaga os displays
  ls.clearDisplayMask(0); //
  ls.clearDisplayMask(1); //  
  
  digitalWrite(lpsens, LOW); //[D] Apaga o LED de status do sensor
  digitalWrite(lssens, LOW); //[D] Apaga o LED de status do sensor
  
}
wdt_reset(); //[F] Reseta watchdog informando que o programa está ok
    
