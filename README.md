# Indi-Duino
Indi-Duino: uma alternativa para Sphero indi com Arduino Nano.

## Descrição
Este projeto consiste em um robô baseado em Arduino Uno R3 que utiliza um sensor de cores TCS3472 para interpretar cartões coloridos e mover-se conforme a cor detectada. O robô é equipado com uma ponte H para controle dos motores e pode acelerar, desacelerar, virar à direita ou à esquerda conforme os cartões.

## Trabalho Descritivo do Processo de Criação (TCC - UFMS)
https://repositorio.ufms.br/handle/123456789/12544

## Especificações do Hardware

### Conexões:

#### Sensor de Cores (TCS3472) - Arduino Nano
- **VIn** → 5V
- **GND** → GND
- **SCL** → A5
- **SDA** → A4
- **LED** → d5 (pwm)

#### Servos - Arduino Nano
- **Servo1** → d2
- **Servo2** → d3

### Componentes Utilizados:
- **Arduino Nano**
- **Sensor de Cores Adafruit TCS3472**
- **Servos MG90S 360**
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


