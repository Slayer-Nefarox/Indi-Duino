# Induino
Induino: uma alternativa para Sphero indi com Arduino Uno R3.

## Descrição
Este projeto consiste em um robô baseado em Arduino Uno R3 que utiliza um sensor de cores TCS34725 para interpretar cartões coloridos e mover-se conforme a cor detectada. O robô é equipado com uma ponte H para controle dos motores e pode acelerar, desacelerar, virar à direita ou à esquerda conforme os cartões.

## Especificações do Hardware

### Conexões:

#### Sensor de Cores (TCS34725) - Arduino Uno R3
- **VIn** → 5V
- **GND** → GND
- **SCL** → A5
- **SDA** → A4
- **LED** → 2

#### Ponte H - Arduino Uno R3
- **IN1** → 11
- **IN2** → 10
- **IN3** → 6
- **IN4** → 5

### Componentes Utilizados:
- **Arduino Uno R3**
- **Sensor de Cores Adafruit TCS34725**
- **Ponte H (L298N ou similar)**
- **Motores DC**
- **Cartões coloridos para controle do movimento**

## Dependências
Antes de carregar o código no Arduino, instale a biblioteca necessária:

```cpp
#include <Wire.h>
#include "Adafruit_TCS34725.h"
```

A biblioteca pode ser adicionada diretamente pela Arduino IDE 2.0 ou baixada do repositório oficial:
[Adafruit TCS34725 GitHub](https://github.com/adafruit/Adafruit_TCS34725)

## Funcionamento
O robô interpreta as cores detectadas pelo sensor e executa comandos específicos:
- **Verde** → Acelerar
- **Vermelho** → Desacelerar
- **Azul** → Virar à direita
- **Roxo** → Virar à esquerda

O código principal implementa essa lógica verificando os valores RGB detectados e comparando com as faixas predefinidas para cada cor.

## Código Explicado
### Definições de Hardware
```cpp
#define Mot_L_Frente 11
#define Mot_L_Tras 10
#define Mot_R_Frente 6
#define Mot_R_Tras 5
```
Define os pinos de controle dos motores.

### Parâmetros do Sistema
```cpp
#define Tempo 1500
#define intervalo 15
#define LedEnable 2
```
Controla o tempo de resposta e margem de erro para leitura de cores.

### Configuração do Sensor
```cpp
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_60X);
```
Define os parâmetros do sensor de cores.

### Função de Debug
```cpp
void debug(float red, float green, float blue) {
  Serial.print("R:	"); Serial.print(int(red)); 
  Serial.print("	G:	"); Serial.print(int(green)); 
  Serial.print("	B:	"); Serial.print(int(blue));
  Serial.print("
");
}
```
Mostra os valores RGB no monitor serial para calibração.

### Lógica de Controle
- **Aceleração:**
```cpp
if(faixa( red, verde[0] ) && faixa(green, verde[1]) && faixa( blue, verde[2]) ) { 
    speed_mult += 2;
    analogWrite(Mot_L_Frente, speed_mult * speed_L);
    analogWrite(Mot_R_Frente, speed_mult * speed_R);
}
```
- **Desaceleração:**
```cpp
if(faixa( red, vermelho[0] ) && faixa(green, vermelho[1]) && faixa( blue, vermelho[2]) ) {
    speed_mult -= 2;
    analogWrite(Mot_L_Frente, speed_mult * speed_L);
    analogWrite(Mot_R_Frente, speed_mult * speed_R);
}
```
- **Virar à Direita:**
```cpp
if(faixa( red, azul[0] ) && faixa(green, azul[1]) && faixa( blue, azul[2]) ) {
    analogWrite(Mot_L_Frente, speed_mult * speed_L);
    analogWrite(Mot_R_Frente, 0);
    delay(tempo_delay);
}
```
- **Virar à Esquerda:**
```cpp
if(faixa( red, roxo[0] ) && faixa(green, roxo[1]) && faixa( blue, roxo[2]) ) {
    analogWrite(Mot_L_Frente, 0);
    analogWrite(Mot_R_Frente, speed_mult * speed_R);
    delay(tempo_delay);
}
```

## Conclusão
Este projeto permite que o robô se movimente de forma autônoma interpretando comandos por meio de cores, sendo uma solução acessível e eficaz para aplicações educacionais.


