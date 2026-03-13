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

// gyroscope (BMI160)
//VIN --------------- 5V 
//GND --------------- GND
//Scl --------------- A5
//Sda --------------- A4

//Adicionar Biblioteca Adafruit_TCS34725.h e DFRobot_BMI160.h ao Arduino IDE 2.0
//Adicionar via "bibliotecas" na arduino IDE 2.0
//Bibliotecas Servo.h e Wire.h é padrão no ambiente de execução. Não é necessário importar para a IDE.


#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h> 
#include <DFRobot_BMI160.h>

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

#define INTERVALO 10

#define ANGULO_CURVA 10

// Flags de Estado
bool detectouCor = false; 

bool movimentoEnable = false;

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

void parar() {
  servoEsq.write(angular(PARADO));
  servoDir.write(angular(PARADO));
}

void moverFrente() {
  servoEsq.write(angular(velFrentEsq));
  servoDir.write(angular(VelFrentDir));
}

void virarDireita() {
  servoEsq.write(angular(velFrentEsq));
  servoDir.write(angular(PARADO)); 
}

void virarEsquerda() {
  servoDir.write(angular(VelFrentDir));
  servoEsq.write(angular(PARADO));
}

void comemora() {
  servoDir.write(angular(VelFrentDir));
  servoEsq.write(angular(velTrasEsq));
}

bool verificaCor(float r, float g, float b, int alvo[]) {
  return (r > alvo[0] - INTERVALO && r < alvo[0] + INTERVALO) &&
         (g > alvo[1] - INTERVALO && g < alvo[1] + INTERVALO) &&
         (b > alvo[2] - INTERVALO && b < alvo[2] + INTERVALO);
}

// ---------- gyro

DFRobot_BMI160 bmi160;
const int8_t i2c_addr = 0x69;

// Configurações de Filtro e Amostragem
const int BUFFER_SIZE = 1; 
const int CALIBRATION_SAMPLES = 200;

// Buffers para Rotação
float bufferGyroX[BUFFER_SIZE], bufferGyroY[BUFFER_SIZE], bufferGyroZ[BUFFER_SIZE];
int bufferIndex = 0;

// Offsets e Variáveis de Estado
float gyroOffsetX = 0, gyroOffsetY = 0, gyroOffsetZ = 0;
float angleX = 0, angleY = 0, angleZ = 0;
float avgAngleX = 0, avgAngleY = 0, avgAngleZ = 0;

unsigned long lastTime;



float rotInicial = 0;
bool girando = false;
int direcaoGiro = 0; // -1 esquerda, +1 direita

void calibrateGyro() {
  int16_t data[6];
  long sx = 0, sy = 0, sz = 0;
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    if (bmi160.getAccelGyroData(data) == 0) {
      sx += data[0]; sy += data[1]; sz += data[2];
    }
    delay(1);
  }
  gyroOffsetX = sx / (float)CALIBRATION_SAMPLES;
  gyroOffsetY = sy / (float)CALIBRATION_SAMPLES;
  gyroOffsetZ = sz / (float)CALIBRATION_SAMPLES;
}

float normalize360(float angle) {
  float result = fmod(angle, 360.0);
  if (result < 0) result += 360.0;
  return result;
}

void calculateAverages() {
  float tx = 0, ty = 0, tz = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    tx += bufferGyroX[i];
    ty += bufferGyroY[i];
    tz += bufferGyroZ[i];
  }
  avgAngleX = tx / (float)BUFFER_SIZE;
  avgAngleY = ty / (float)BUFFER_SIZE;
  avgAngleZ = tz / (float)BUFFER_SIZE;
}

void setup() 
{
  Serial.begin(9600);
  Serial.println("serial Initialized");
  servoEsq.attach(PIN_SERVO_ESQ);
  servoDir.attach(PIN_SERVO_DIR);
  parar(); 
  
  pinMode(LedEnable_integrated, OUTPUT);
  pinMode(LedEnable_dedicated, OUTPUT);
  
  analogWrite(LedEnable_integrated, 200); 
  analogWrite(LedEnable_dedicated, 90);

  if (tcs.begin()) 
  {
    Serial.println("Sensor RGB encontrado");
  } 
  else 
  {
    while (1)
    {
      Serial.println("TCS34725 nao encontrado. reinicie o robô.");
    }
  }
  // ------------ gyro
  if (bmi160.softReset() != BMI160_OK || bmi160.I2cInit(i2c_addr) != BMI160_OK) while(1);

  calibrateGyro();
  lastTime = micros();
}

void loop() 
{
  float r, g, b;
  tcs.getRGB(&r, &g, &b);

  // Debug (opcional, remova para performance)
  //Serial.print("R: "); Serial.print(r); Serial.print(" G: "); Serial.print(g); Serial.print(" B: "); Serial.println(b);
  // inicialização do giroscópio no loop
  int16_t raw[6];
  unsigned long currentTime = micros();
  float dt = (currentTime - lastTime) / 1000000.0;
  lastTime = currentTime;

  //---------------------- recuperação de angulos e buffers do giroscópio
  if (bmi160.getAccelGyroData(raw) == 0) {
    // Conversão para DPS (Graus por Segundo)
    float gx = (raw[0] - gyroOffsetX) / 131.0;
    float gy = (raw[1] - gyroOffsetY) / 131.0;
    float gz = (raw[2] - gyroOffsetZ) / 131.0;

    // Integração para obter ângulo
    angleX += gx * dt;
    angleY += gy * dt;
    angleZ += gz * dt;

    // Normalização 0-359 e atualização de buffer
    bufferGyroX[bufferIndex] = angleX;
    bufferGyroY[bufferIndex] = angleY;
    bufferGyroZ[bufferIndex] = angleZ;
    bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;

    calculateAverages();

    //Serial.print("X:"); Serial.print(avgAngleX); Serial.print("\t");
    //Serial.print("Y:"); Serial.print(avgAngleY); Serial.print("\t");
    Serial.print("Z:"); Serial.println(avgAngleZ);
  }

  // lógica de verificação de cores
  if (verificaCor(r, g, b, verde)) 
  {
    if (!detectouCor && !movimentoEnable) 
    {
      movimentoEnable = true;
      delay(3000);
      Serial.println("Verde: FRENTE");
      moverFrente();
      detectouCor = true;
    }
  }

  else if (verificaCor(r, g, b, azul)) 
  {
    if (!detectouCor && movimentoEnable) 
    {
      Serial.println("Azul: VIRAR DIR + FRENTE");
      rotInicial = avgAngleZ;
      girando = true;
      direcaoGiro = -1;
      detectouCor = true;
    }
  }

  else if (verificaCor(r, g, b, vermelho)) 
  {
    if (!detectouCor && movimentoEnable) 
    {
      Serial.println("Vermelho: PARAR");
      parar();
      detectouCor = true;
      movimentoEnable = false;
    }
  }

  else if (verificaCor(r, g, b, roxo)) 
  {
    if (!detectouCor && movimentoEnable) 
    {
      Serial.println("Roxo: VIRAR ESQ + FRENTE");
      rotInicial = avgAngleZ;
      girando = true;
      direcaoGiro = 1;
      detectouCor = true;
    }
  }

  else if (verificaCor(r, g, b, amarelo)) 
  {
    if (!detectouCor && movimentoEnable) 
    {
      Serial.println("Amarelo: COMEMORA");
      comemora();
      delay(5000);
      parar();
      detectouCor = true;
      movimentoEnable = false;
    }
  }

  else 
  {
    //Serial.println("Cor desconhecida / Chão");
    detectouCor = false;
  }
  //caso esteja virando
  if (girando)
  {
    float delta = avgAngleZ - rotInicial;

    if (direcaoGiro == 1) // direita
    {
      if (delta < ANGULO_CURVA)
      {
        virarEsquerda();
      }
      else
      {
        girando = false;
        moverFrente();
      }
    }

    if (direcaoGiro == -1) // esquerda
    {
      if (delta > -ANGULO_CURVA)
      {
        virarDireita();
      }
      else
      {
        girando = false;
        moverFrente();
      }
    }
  }
}
  }
}
