//ligar os seguintes pinos:
//TCS3472 ----------- Arduino Nano
//VIn --------------- 5v
//Gnd --------------- Gnd
//Scl --------------- A5
//Sda --------------- A4
//Led --------------- d5 (Pwm)

//Ponte H ----------- Arduino Nano
//in1 --------------- d2
//in2 --------------- d3

//Adicionar Biblioteca "Adafruit_TCS34725.h" ao Arduino IDE 2.0
//Possivel encontrar em https://github.com/adafruit/Adafruit_TCS34725
//adicionar via "bibliotecas" na arduino IDE 2.0 também é possivel, e BEM mais viável.


#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h> 

// --- Configuração ---
#define PIN_SERVO_ESQ 2
#define PIN_SERVO_DIR 3

#define LedEnable_integrated 5
#define LedEnable_dedicated 6

// --- Calibração Servos ---
#define PARADO 90 
#define MAX_FRENTE_ESQ 180 
#define MAX_TRAS_ESQ 0     
#define MAX_FRENTE_DIR 20 
#define MAX_TRAS_DIR 180   

#define INTERVALO 10


// Flags de Estado
bool detectouCor = false; 

bool velocidade = false;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_60X);
Servo servoEsq;
Servo servoDir;

// Cores alvo (R, G, B)
int verde[3] = {55, 122, 75};
int vermelho[3] = {179, 48, 42};
int azul[3] = {23, 90, 143};
int roxo[3] = {58, 75, 128};
int amarelo[3] = {111, 104, 59};

void parar() {
  servoEsq.write(PARADO );
  servoDir.write(PARADO );
}

void moverFrente() {
  servoEsq.write(MAX_FRENTE_ESQ);
  servoDir.write(MAX_FRENTE_DIR);
}

// CORREÇÃO: Removemos moverFrente() daqui para permitir controle temporal no loop
void virarDireita() {
  servoEsq.write(MAX_FRENTE_ESQ);
  servoDir.write(PARADO); 
}

void virarEsquerda() {
  servoDir.write(MAX_FRENTE_DIR);
  servoEsq.write(PARADO);
}

void comemora() {
  servoDir.write(MAX_FRENTE_DIR);
  servoEsq.write(MAX_TRAS_ESQ);
}

bool verificaCor(float r, float g, float b, int alvo[]) {
  return (r > alvo[0] - INTERVALO && r < alvo[0] + INTERVALO) &&
         (g > alvo[1] - INTERVALO && g < alvo[1] + INTERVALO) &&
         (b > alvo[2] - INTERVALO && b < alvo[2] + INTERVALO);
}

void setup() {
  Serial.begin(9600);
  servoEsq.attach(PIN_SERVO_ESQ);
  servoDir.attach(PIN_SERVO_DIR);
  parar(); 
  
  pinMode(LedEnable_integrated, OUTPUT);
  analogWrite(LedEnable_integrated, 200); 
  analogWrite(LedEnable_dedicated, 90);
  if (tcs.begin()) 
  {
    Serial.println("Sensor encontrado");
  } else 
  {
    Serial.println("TCS34725 nao encontrado. reinicie o robô.");
    while (1);
  }
}

void loop() 
{
  float r, g, b;
  tcs.getRGB(&r, &g, &b);

  // Debug (opcional, remova para performance)
  Serial.print("R: "); Serial.print(r); Serial.print(" G: "); Serial.print(g); Serial.print(" B: "); Serial.println(b);

  if (verificaCor(r, g, b, verde)) 
  {
    if (!detectouCor) 
    {
      velocidade = true;
      delay(3000);
      Serial.println("Verde: FRENTE");
      moverFrente();
      detectouCor = true;
      
    }
  }
  else if (verificaCor(r, g, b, azul)) 
  {
    if (!detectouCor && velocidade) 
    {
      Serial.println("Azul: VIRAR DIR + FRENTE");
      virarDireita();
      delay(800); // CORREÇÃO: Tempo para realizar a curva
      moverFrente();
      detectouCor = true;
    }
  }
  else if (verificaCor(r, g, b, vermelho)) 
  {
    if (!detectouCor && velocidade) 
    {
      Serial.println("Vermelho: PARAR 3s");
      parar();
      delay(5000);
      detectouCor = true;
      velocidade = false;//fim de percurso
    }
  }
  else if (verificaCor(r, g, b, roxo)) 
  {
    if (!detectouCor && velocidade) 
    {
      Serial.println("Roxo: VIRAR ESQ + FRENTE");
      virarEsquerda();
      delay(800); // CORREÇÃO: Tempo para realizar a curva
      moverFrente();
      detectouCor = true;
    }
  }
  else if (verificaCor(r, g, b, amarelo)) 
  {
    if (!detectouCor && velocidade) 
    {
      Serial.println("Amarelo: COMEMORA");
      comemora();
      delay(5000); // CORREÇÃO: Tempo para executar o giro
      parar();
      detectouCor = true;
      velocidade = false;//fim de percurso
    }
  }
  else 
  {
    Serial.println("Cor desconhecida / Chão");
    detectouCor = false; 
    
  }
}
