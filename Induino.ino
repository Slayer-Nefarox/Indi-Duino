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

/* gyroscope (BMI160)
VIN → 5V 
GND → GND
SCX → D13 (SCK)
SDX → D12 (MISO)
SDA → D11 (MOSI)
CS → D10 (pino digital configurado como Chip Select)
*/

//Adicionar Biblioteca "Adafruit_TCS34725.h" ao Arduino IDE 2.0
//Possivel encontrar em https://github.com/adafruit/Adafruit_TCS34725
//Adicionar via "bibliotecas" na arduino IDE 2.0 também é possivel, e BEM mais viável.
//Bibliotecas Servo.h e SPI.h são padrão no ambiente de execução. Não é necessário adicionar.


#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h> 
#include <SPI.h>


// --- Configuração ---
#define PIN_SERVO_ESQ 2
#define PIN_SERVO_DIR 3

#define LedEnable1 5
#define LedEnable2 6

#define PIN_CS 10

// --- Calibração Servos ---
#define PARADO 90 
#define MAX_FRENTE_ESQ 180 
#define MAX_TRAS_ESQ 0     
#define MAX_FRENTE_DIR 20 
#define MAX_TRAS_DIR 180   

#define INTERVALO 30

// =====================
// Registradores
// =====================
const uint8_t REG_CMD        = 0x7E;
const uint8_t REG_GYRO_X_L   = 0x0C;
const uint8_t REG_GYRO_RANGE = 0x43;
const uint8_t REG_GYRO_CONF  = 0x42;

// =====================
// Escala
// =====================
const float GYRO_SCALE_250DPS = 131.2; // LSB/(°/s) para ±250 dps

// =====================
// Variáveis
// =====================
int16_t rawX, rawY, rawZ;
float gyroX, gyroY, gyroZ;


// Flags de Estado
bool detectou_Cor_Antes = false; 

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_60X);
Servo servoEsq;
Servo servoDir;

// Cores alvo (R, G, B)
int verde[3] = {55, 122, 75};
int vermelho[3] = {179, 48, 42};
int azul[3] = {23, 90, 143};
int roxo[3] = {58, 75, 128};
int amarelo[3] = {111, 104, 59};


void writeRegister(uint8_t reg, uint8_t data) {
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(reg & 0x7F);  // bit7 = 0 → write
  SPI.transfer(data);
  digitalWrite(PIN_CS, HIGH);
}

void readRegisters(uint8_t reg, uint8_t count, uint8_t* buffer) {
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(reg | 0x80);  // bit7 = 1 → read
  for (uint8_t i = 0; i < count; i++) {
    buffer[i] = SPI.transfer(0x00);
  }
  digitalWrite(PIN_CS, HIGH);
}

void parar() {
  servoEsq.write(PARADO);
  servoDir.write(PARADO);
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

void read_position(uint8_t vetorbuffer[])
{
   readRegisters(REG_GYRO_X_L, 6, vetorbuffer);

  rawX = vetorbuffer[1] << 8 | vetorbuffer[0];
  rawY = vetorbuffer[3] << 8 | vetorbuffer[2];
  rawZ = vetorbuffer[5] << 8 | vetorbuffer[4];

  gyroX = rawX / GYRO_SCALE_250DPS;
  gyroY = rawY / GYRO_SCALE_250DPS;
  gyroZ = rawZ / GYRO_SCALE_250DPS;

  Serial.print("Gx: ");
  Serial.print(gyroX);
  Serial.print(" | Gy: ");
  Serial.print(gyroY);
  Serial.print(" | Gz: ");
  Serial.println(gyroZ);
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
  servoEsq.attach(PIN_SERVO_ESQ);
  servoDir.attach(PIN_SERVO_DIR);
  parar(); 

  //gyro-----------------------
  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_CS, HIGH);  // Mantém desativado

  SPI.begin();  // Inicializa SPI

  // Configura SPI (modo 0, até 1 MHz seguro)
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  delay(100);

  // Coloca giroscópio em modo normal
  writeRegister(REG_CMD, 0x15);  // 0x15 = gyro normal mode
  delay(100);

  // Configura faixa ±250 dps
  writeRegister(REG_GYRO_RANGE, 0x00);

  // Configura ODR padrão (100 Hz)
  writeRegister(REG_GYRO_CONF, 0x28);

  //------------------------------
  //TCS---------------------------
  
  pinMode(LedEnable1, OUTPUT);
  analogWrite(LedEnable1, 125); // Liga LED para calibração inicial
  pinMode(LedEnable2, OUTPUT);
  analogWrite(LedEnable2, 80); // Liga LED para calibração inicial
  
  
  if (tcs.begin()) 
  {
    Serial.println("Sensor encontrado");
  } 
  else 
  {
    Serial.println("TCS34725 nao encontrado. Travando.");
    while (1);
  }
}

void loop() 
{

  float r, g, b;
  tcs.getRGB(&r, &g, &b);

  // Debug (opcional, remova para performance)
  //Serial.print("R: "); Serial.print(r); Serial.print(" G: "); Serial.print(g); Serial.print(" B: "); Serial.println(b);

  uint8_t buffer[6];

  read_position(buffer);

  if (verificaCor(r, g, b, verde)) 
  {
    if (!detectou_Cor_Antes) 
    {
      delay(3000);
      Serial.println("Verde: FRENTE");
      moverFrente();
      detectou_Cor_Antes = true;
    }
  }
  else if (verificaCor(r, g, b, azul)) 
  {
    if (!detectou_Cor_Antes) 
    {
      Serial.println("Azul: VIRAR DIR + FRENTE");
      virarDireita();
      delay(800); // CORREÇÃO: Tempo para realizar a curva
      moverFrente();
      detectou_Cor_Antes = true;
    }
  }
  else if (verificaCor(r, g, b, vermelho)) 
  {
    if (!detectou_Cor_Antes) 
    {
      Serial.println("Vermelho: PARAR");
      parar();
      delay(5000);
      detectou_Cor_Antes = true;
    }
  }
  else if (verificaCor(r, g, b, roxo)) 
  {
    if (!detectou_Cor_Antes) 
    {
      Serial.println("Roxo: VIRAR ESQ + FRENTE");
      virarEsquerda();
      delay(800); // CORREÇÃO: Tempo para realizar a curva
      moverFrente();
      detectou_Cor_Antes = true;
    }
  }
  else if (verificaCor(r, g, b, amarelo)) 
  {
    if (!detectou_Cor_Antes) 
    {
      Serial.println("Amarelo: COMEMORA");
      comemora();
      delay(5000); // CORREÇÃO: Tempo para executar o giro
      parar();
      detectou_Cor_Antes = true;
    }
  }
  else 
  {
    //Serial.println("Cor desconhecida / Chão");
    detectou_Cor_Antes = false; 
    
  }
}
