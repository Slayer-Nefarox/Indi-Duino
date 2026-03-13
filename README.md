# Indi-Duino
Indi-Duino: uma alternativa para Sphero indi com Arduino Nano.
## Versão com giroscópio.

## Descrição
Este projeto consiste em um robô baseado em Arduino nano que utiliza um sensor de cores TCS3472 para interpretar cartões coloridos e mover-se conforme a cor detectada. O robô agora usa servomotores 360 ligados diretamente e um giroscópio para movimentação e ajustes de localização.

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
- **LED2** → d6 (pwm)

#### Servos - Arduino Nano
- **Servo1** → d2
- **Servo2** → d3

  #### Giroscópio BMI160 - Arduino Nano
- **VIn** → 5V
- **GND** → GND
- **SCL** → A5
- **SDA** → A4

### Componentes Utilizados:
- **Arduino Nano**
- **Sensor de Cores Adafruit TCS3472**
- **Sensor Giroscópio BMI160**
- **Servos MG90S 360**
- **Cartões coloridos para controle do movimento**

## Dependências
Antes de carregar o código no Arduino, instale as bibliotecas necessárias:

```cpp
DFRobot_BMI160.h
Adafruit_TCS34725.h
```

As bibliotecas podem ser adicionadas diretamente pela Arduino IDE 2.0 

## Funcionamento
O robô interpreta as cores detectadas pelo sensor e executa comandos específicos:
- **Verde** → Acelerar
- **Vermelho** → Desacelerar
- **Azul** → Virar à direita
- **Roxo** → Virar à esquerda

O giroscópio permite o robo manter a rotação de 90 graus mesmo com o nivel de bateria baixo.


