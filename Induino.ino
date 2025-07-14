//ligar os seguintes pinos:
//TCS3472 ----------- Arduino Uno R3
//VIn --------------- 5v
//Gnd --------------- Gnd
//Scl --------------- A5
//Sda --------------- A4
//Led --------------- 2

//Ponte H ----------- Arduino Uno R3
//in1 --------------- 11
//in2 --------------- 10
//in3 --------------- 6
//in4 --------------- 5

//Adicionar Biblioteca "Adafruit_TCS34725.h" ao Arduino IDE 2.0
//Possivel encontrar em https://github.com/adafruit/Adafruit_TCS34725
//adicionar via "bibliotecas" na arduino IDE 2.0 também é possivel, e BEM mais viável.


#include <Wire.h>
#include "Adafruit_TCS34725.h"

#define Mot_L_Frente 11
#define Mot_L_Tras 10
#define Mot_R_Frente 6
#define Mot_R_Tras 5

#define Tempo 1500 // tempo (em milissegundos) entre comandos

#define intervalo 15 // intervalo de tolerancia da faixa de cor

#define LedEnable 2 //pino que liga/desliga o led (caso use iluminação propria da placa)

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_60X);// ganho possivel: X01, X04, X16, X60.

//variaveis globais
  //Velocidades individuais para cada motor para balancear imparidade de hardware. Minimo 1, máximo até 63.
int speed_L = 59;
int speed_R = 63;

int speed_mult = 0;
int ultima_Cor = 0;

//indice de cores
int verde[3] = {60,110,80};
int vermelho[3] = {150, 55,55};
int azul[3] = {28,86,131};
int roxo[3] = {103,51,94};
// ...

void debug(float red, float green, float blue)
{
  //debug valor do RGB
  Serial.print("R:\t"); Serial.print(int(red)); 
  Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));
  Serial.print("\n");
}

bool faixa(int var, int valor)
{
  if( var > valor-intervalo && var <= valor+intervalo )// se 'var' estiver dentro do intervalo levando em conta a margem de tolerância
  {
    return true;
  }
  else
  {
    return false;
  }
}

void setup() 
{
  Serial.begin(9600);
  pinMode(Mot_L_Frente, OUTPUT);
  pinMode(Mot_L_Tras, OUTPUT);
  pinMode(Mot_R_Frente, OUTPUT);
  pinMode(Mot_R_Tras, OUTPUT);

  analogWrite(Mot_L_Frente, 0);
  analogWrite(Mot_L_Tras, 0);

  analogWrite(Mot_R_Frente, 0);
  analogWrite(Mot_R_Tras, 0);
  
  if (tcs.begin()) 
  {
    Serial.println("Found sensor");
  } 
  else 
  {
    while (1) // ERRO
    {
      Serial.println("No TCS34725 found ... check your connections and reboot");
    }
  }

  pinMode(LedEnable, OUTPUT);
}

void loop() 
{
  digitalWrite(LedEnable, HIGH);// controle do LED
  float tempo_delay = Tempo;//estando mais rápido, precisa de menos tempo pra sair do cartão
  float red, green, blue;
  tcs.getRGB(&red, &green, &blue);

  if(faixa( red, verde[0] ) && faixa(green, verde[1]) && faixa( blue, verde[2]) ) //1: verde
  { 
    if(ultima_Cor != 1)
    {
      Serial.print("Frente ou acelera.");
      if(speed_mult<4)
      {
        speed_mult +=2;
        tempo_delay = Tempo / ((2/3)*speed_mult);//toda vez que atualizar a velocidade, atulizar o delay de virada
      }
      ultima_Cor = 1;
      delay(500);
      analogWrite(Mot_L_Frente, speed_mult*speed_L);
      analogWrite(Mot_L_Tras, 0); 
      analogWrite(Mot_R_Frente, speed_mult*speed_R);
      analogWrite(Mot_R_Tras, 0);
    }
    else
    {
      Serial.print("Frente ou acelera, Negado.");
    }
    
  }
  else if(faixa( red, azul[0] ) && faixa(green, azul[1]) && faixa( blue, azul[2]) ) //2: Azul
  { 
    if(ultima_Cor != 2)
    {
      Serial.print("Virar a direita.");
      ultima_Cor = 2;
      delay(500);
      analogWrite(Mot_L_Frente, speed_mult*speed_L);
      analogWrite(Mot_L_Tras, 0); 
      analogWrite(Mot_R_Frente, 0);
      analogWrite(Mot_R_Tras, 0); 
      delay(tempo_delay);//ajustar tempo_delay para virar 90 graus 
      analogWrite(Mot_L_Frente, speed_mult*speed_L);
      analogWrite(Mot_L_Tras, 0); 
      analogWrite(Mot_R_Frente, speed_mult*speed_R);
      analogWrite(Mot_R_Tras, 0);
    }
    else
    {
      Serial.print("Virar a direita, Negado.");
    }
    
  }
  else if(faixa( red, vermelho[0] ) && faixa(green, vermelho[1]) && faixa( blue, vermelho[2]) ) //3: vermelho
  { 
    if(ultima_Cor != 3)
    {
      Serial.print("Desacelera ou para.");
      if(speed_mult>0)
      {
        speed_mult -=2;
        tempo_delay = Tempo / ((2/3)*speed_mult);//toda vez que atualizar a velocidade, atulizar o delay de virada
      }
      ultima_Cor = 3;
      delay(500);
      analogWrite(Mot_L_Frente, speed_mult*speed_L);
      analogWrite(Mot_L_Tras, 0); 
      analogWrite(Mot_R_Frente, speed_mult*speed_R);
      analogWrite(Mot_R_Tras, 0);
    }
    else
    {
      Serial.print("Desacelera ou para, Negado.");
    }
    
  }
  else if(faixa( red, roxo[0] ) && faixa(green, roxo[1]) && faixa( blue, roxo[2]) ) //4: roxo
  { 
    if(ultima_Cor != 4)
    {
      Serial.print("Virar a esquerda.");
      delay(500);
      ultima_Cor = 4;
      analogWrite(Mot_L_Frente, 0);
      analogWrite(Mot_L_Tras, 0);
      analogWrite(Mot_R_Frente, speed_mult*speed_R);
      analogWrite(Mot_R_Tras, 0);
      delay(tempo_delay);//ajustar tempo_delay para virar 90 graus 
      analogWrite(Mot_L_Frente, speed_mult*speed_L);
      analogWrite(Mot_L_Tras, 0);
      analogWrite(Mot_R_Frente, speed_mult*speed_R);
      analogWrite(Mot_R_Tras, 0);
    }
    else
    {
      Serial.print("Virar a esquerda, Negado.");
    }
  }
  else //nenhuma cor cadastrada
  {
    Serial.print("Nenhuma cor detectada entre as cadastradas.");
    ultima_Cor = 0;
  }
  //modelo de estrutura para adicionar cores
  /*
  else if(faixa( red, cor[0] ) && faixa(green, cor[1]) && faixa( blue, cor[2]) ) // cor
  { 
    Serial.print("frente");
    analogWrite(Mot_L_Frente, 200);
    analogWrite(Mot_L_Tras, 0); 
    analogWrite(Mot_R_Frente, 200);
    analogWrite(Mot_R_Tras, 0); 
    delay(tempo_delay);
  }
  */
  debug(red, green, blue);
}

