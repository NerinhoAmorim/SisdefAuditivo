# Sistema de Auxílio para Pessoas com Deficiência Auditiva com RP2040

Este projeto é um sistema de auxílio para pessoas com deficiência auditiva, utilizando o microcontrolador RP2040. O sistema detecta níveis de som e fornece alertas visuais e sonoros, ajudando na identificação de sons importantes no ambiente.

## Funcionalidades

- Detecção de Som: O sistema utiliza um microfone para monitorar os níveis de som.
- Alertas Visuais: LEDs de diferentes cores (vermelho, amarelo e verde) indicam diferentes níveis de alerta.
- Alertas Sonoros: Um buzzer emite sons para alertar o usuário.
- Ajuste de Sensibilidade: O usuário pode ajustar a sensibilidade do sistema usando um joystick.
- Display OLED: Um display OLED exibe informações sobre a sensibilidade atual.

## Componentes Necessários

- Microcontrolador RP2040
- Microfone (analógico)
- Joystick
- LEDs (vermelho, amarelo, verde)
- Buzzer
- Display OLED (I2C)
- Resistores e fios de conexão

## Pinagem

| Componente          | Pino  |
|---------------------|-------|
| Microfone           | GP28  |
| Joystick (Botão)    | GP10  |
| Joystick (Eixo X)   | GP26  |
| Joystick (Eixo Y)   | GP27  |
| LED Vermelho        | GP13  |
| LED Amarelo         | GP12  |
| LED Verde           | GP11  |
| Buzzer              | GP21  |
| Botão de Alerta     | GP9   |
| I2C SDA             | GP14  |
| I2C SCL             | GP15  |

## Como Usar

1. Conexão: Conecte todos os componentes conforme a pinagem especificada.
2. Compilação: Compile o código utilizando um ambiente de desenvolvimento compatível com RP2040.
3. Upload: Faça o upload do código para o microcontrolador.
4. Operação: O sistema começará a monitorar os níveis de som. Use o joystick para ajustar a sensibilidade conforme necessário.

## Código

O código-fonte do projeto está disponível no arquivo principal. Ele inclui funções para:

- Inicializar os componentes
- Ler os valores do microfone e joystick
- Controlar os LEDs e o buzzer
- Exibir informações no display OLED