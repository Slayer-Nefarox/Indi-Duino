//ligar os seguintes pinos:
//TCS3472 ----------- Arduino Nano
//VIn --------------- 5v
//Gnd --------------- Gnd
//Scl --------------- A5
//Sda --------------- A4
//Led --------------- d5 (Pwm)

//Servos tower 360 ----------- Arduino Nano
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
#define VELOCIDADE_L 50 // -------->Caso motores desiguais, calibrar aqui
#define VELOCIDADE_R 50
const int PARADO =0;
const int velFrentEsq = VELOCIDADE_L;
const int velTrasEsq = -VELOCIDADE_L;    
const int VelFrentDir = -VELOCIDADE_R;
const int velTrasDir = VELOCIDADE_R;  

#define INTERVALO 18


// Flags de Estado
bool detectouCor = false; 

bool velocidade = false;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_60X);
Servo servoEsq;
Servo servoDir;

int angular(int velocidade)
{
  return map(velocidade, -100, 100,0,180 );
}

// Cores alvo (R, G, B)
int verde[3] = {67, 115, 71};
int vermelho[3] = {178, 44, 44};
int azul[3] = {31, 86, 147};
int roxo[3] = {67, 66, 126};
int amarelo[3] = {150, 117, 57};

void parar() 
{
  servoEsq.write(angular(PARADO));
  servoDir.write(angular(PARADO));
}

void moverFrente() 
{
  servoEsq.write(angular(velFrentEsq));
  servoDir.write(angular(VelFrentDir));
}

void virarDireita() 
{
  servoEsq.write(angular(velFrentEsq));
  servoDir.write(angular(PARADO)); 
}

void virarEsquerda() 
{
  servoDir.write(angular(VelFrentDir));
  servoEsq.write(angular(PARADO));
}

void comemora() 
{
  servoDir.write(angular(VelFrentDir));
  servoEsq.write(angular(velTrasEsq));
}

bool verificaCor(float r, float g, float b, int alvo[]) 
{
  return (r > alvo[0] - INTERVALO && r < alvo[0] + INTERVALO) &&
         (g > alvo[1] - INTERVALO && g < alvo[1] + INTERVALO) &&
         (b > alvo[2] - INTERVALO && b < alvo[2] + INTERVALO);
}

void setup() 
{
  Serial.begin(9600);
  Serial.println("Comunicação Serial Inicializada");
  servoEsq.attach(PIN_SERVO_ESQ);
  servoDir.attach(PIN_SERVO_DIR);
  parar(); 
  
  pinMode(LedEnable_integrated, OUTPUT);
  pinMode(LedEnable_dedicated, OUTPUT);
  
  analogWrite(LedEnable_integrated, 200); 
  analogWrite(LedEnable_dedicated, 90);

  if (tcs.begin()) 
  {
    Serial.println("Sensor RGB Inicializado");
  } 
  else 
  {
    while (1);
    {
      Serial.println("TCS34725 nao encontrado. reinicie o robô.");
    }
  }
}

void loop() 
{
  float r, g, b;
  tcs.getRGB(&r, &g, &b);

  // Debug (opcional, remova para performance)
  //Serial.print("R: "); Serial.print(r); Serial.print(" G: "); Serial.print(g); Serial.print(" B: "); Serial.println(b);

  if (verificaCor(r, g, b, verde)) 
  {
    if (!detectouCor && !velocidade) 
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
      delay(1000); 
      moverFrente();
      detectouCor = true;
    }
  }
  else if (verificaCor(r, g, b, vermelho)) 
  {
    if (!detectouCor && velocidade) 
    {
      Serial.println("Vermelho: PARAR");
      parar();
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
      delay(1000); 
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
      delay(5000);
      parar();
      detectouCor = true;
      velocidade = false;//fim de percurso
    }
  }
  else 
  {
    //Serial.println("Cor desconhecida / Chão");
    detectouCor = false; 
    
  }
}
