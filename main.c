#include <stdio.h>
#include <string.h> // Para usar strcmp
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "include/ssd1306.h" // Biblioteca para o display OLED

// Definições dos pinos
#define MICROFONE_PIN 28    // Pino GP28 para o microfone (analógico - ADC2)
#define JOYSTICK_BUTTON 10  // Pino GP10 para o botão do joystick //Changed pin
#define JOYSTICK_X_PIN 26   // Pino GP26 para eixo X do joystick (analógico - ADC0)
#define JOYSTICK_Y_PIN 27   // Pino GP27 para eixo Y do joystick (analógico - ADC1)
#define LED_VERMELHO_PIN 13 // Pino GP13 para o LED vermelho (PWM)
#define LED_AMARELO_PIN 12  // Pino GP12 para o LED amarelo (PWM)
#define LED_VERDE_PIN 11    // Pino GP11 para o LED verde (PWM)
#define BUZZER_PIN 21       // Pino GP21 para o buzzer
#define BOTAO_PIN 9         // Pino GP9 para o botão de alerta
#define I2C_SDA_PIN 14      // Pino GP14 para o SDA do I2C
#define I2C_SCL_PIN 15      // Pino GP15 para o SCL do I2C
#define I2C_ID i2c1         // ID do I2C (i2c1)
#define I2C_ADDR 0x3C       // Endereço I2C do display OLED
#define I2C_FREQ 100000     // 100kHz   // Frequência do I2C

// Constantes para ajuste de sensibilidade
#define SENSIBILIDADE_BAIXA 10000
#define SENSIBILIDADE_MEDIA 20000
#define SENSIBILIDADE_ALTA 30000
#define AJUSTE_SENSIBILIDADE 500 // Mudança de valor para melhor ajuste

// Variáveis globais
uint16_t limite_alerta = SENSIBILIDADE_MEDIA; // Sensibilidade inicial
bool debounce_flag = false;                   // Flag para debounce

// Função para debounce (evitar leituras falsas de botões)
bool debounce(uint gpio)
{
    if (gpio_get(gpio))
    {
        if (!debounce_flag)
        {
            debounce_flag = true;
            sleep_ms(20); // Aguarda 20ms para estabilizar o sinal
            return true;
        }
    }
    else
    {
        debounce_flag = false;
    }
    return false;
}

// Função para controlar os LEDs (PWM)
void alerta_visual(const char *cor, uint8_t intensidade)
{
    uint slice_num_red = pwm_gpio_to_slice_num(LED_VERMELHO_PIN);
    uint slice_num_yellow = pwm_gpio_to_slice_num(LED_AMARELO_PIN);
    uint slice_num_green = pwm_gpio_to_slice_num(LED_VERDE_PIN);

    if (strcmp(cor, "vermelho") == 0)
    {
        pwm_set_gpio_level(LED_VERMELHO_PIN, intensidade); // Controla intensidade do LED vermelho
        pwm_set_gpio_level(LED_AMARELO_PIN, 0);            // Desliga LED amarelo
        pwm_set_gpio_level(LED_VERDE_PIN, 0);              // Desliga LED verde
        pwm_set_enabled(slice_num_red, true);
        pwm_set_enabled(slice_num_yellow, false);
        pwm_set_enabled(slice_num_green, false);
    }
    else if (strcmp(cor, "amarelo") == 0)
    {
        pwm_set_gpio_level(LED_AMARELO_PIN, intensidade); // Controla intensidade do LED amarelo
        pwm_set_gpio_level(LED_VERMELHO_PIN, 0);          // Desliga LED vermelho
        pwm_set_gpio_level(LED_VERDE_PIN, 0);             // Desliga LED verde
        pwm_set_enabled(slice_num_red, false);
        pwm_set_enabled(slice_num_yellow, true);
        pwm_set_enabled(slice_num_green, false);
    }
    else if (strcmp(cor, "verde") == 0)
    {
        pwm_set_gpio_level(LED_VERDE_PIN, intensidade); // Controla intensidade do LED verde
        pwm_set_gpio_level(LED_VERMELHO_PIN, 0);        // Desliga LED vermelho
        pwm_set_gpio_level(LED_AMARELO_PIN, 0);         // Desliga LED amarelo
        pwm_set_enabled(slice_num_red, false);
        pwm_set_enabled(slice_num_yellow, false);
        pwm_set_enabled(slice_num_green, true);
    }
}

// Função para ativar o buzzer (feedback tátil/sonoro)
void alerta_sonoro()
{
    gpio_put(BUZZER_PIN, 1); // Liga o buzzer
    sleep_ms(200);           // Mantém ligado por 200ms
    gpio_put(BUZZER_PIN, 0); // Desliga o buzzer
}

// Função para exibir o nível de sensibilidade no display
void exibir_sensibilidade(ssd1306_t *display, uint16_t sensibilidade)
{
    char mensagem[32];
    snprintf(mensagem, sizeof(mensagem), "Sensibilidade: %s",
             sensibilidade == SENSIBILIDADE_BAIXA ? "Baixa" : sensibilidade == SENSIBILIDADE_MEDIA ? "Media"
                                                                                                   : "Alta");

    ssd1306_fill(display, false);                 // Limpa o display
    ssd1306_draw_string(display, mensagem, 0, 0); // Exibe a mensagem
    ssd1306_send_data(display);                   // Atualiza o display
}

// Função principal
int main()
{
    stdio_init_all();

    // Inicializa o ADC para o microfone e joystick
    adc_init();
    adc_gpio_init(MICROFONE_PIN);  // GP28 (ADC2)
    adc_gpio_init(JOYSTICK_X_PIN); // GP26 (ADC0)
    adc_gpio_init(JOYSTICK_Y_PIN); // GP27 (ADC1)
    gpio_pull_down(MICROFONE_PIN);

    // Inicializa os LEDs como PWM
    gpio_set_function(LED_VERMELHO_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED_AMARELO_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED_VERDE_PIN, GPIO_FUNC_PWM);
    uint slice_num_red = pwm_gpio_to_slice_num(LED_VERMELHO_PIN);
    uint slice_num_yellow = pwm_gpio_to_slice_num(LED_AMARELO_PIN);
    uint slice_num_green = pwm_gpio_to_slice_num(LED_VERDE_PIN);
    pwm_set_wrap(slice_num_red, 255);
    pwm_set_wrap(slice_num_yellow, 255);
    pwm_set_wrap(slice_num_green, 255);
    pwm_set_enabled(slice_num_red, true);
    pwm_set_enabled(slice_num_yellow, true);
    pwm_set_enabled(slice_num_green, true);

    // Inicializa o buzzer
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0); // Ensure buzzer is off initially

    // Inicializa o botão
    gpio_init(BOTAO_PIN);
    gpio_set_dir(BOTAO_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_PIN); // Habilita pull-up interno

    // Inicializa o botão do joystick
    gpio_init(JOYSTICK_BUTTON);
    gpio_set_dir(JOYSTICK_BUTTON, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON); // Habilita pull-up interno

    // Inicializa o display I2C
    i2c_init(I2C_ID, I2C_FREQ);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    ssd1306_t display;
    ssd1306_init(&display, 128, 64, false, I2C_ADDR, I2C_ID);
    ssd1306_config(&display); // Configura o display
    exibir_sensibilidade(&display, limite_alerta);
    sleep_ms(1000);

    // Loop principal
    while (true)
    {
        // Verifica se o botão do joystick foi pressionado para ajustar a sensibilidade
        if (debounce(JOYSTICK_BUTTON))
        {
            if (limite_alerta == SENSIBILIDADE_BAIXA)
                limite_alerta = SENSIBILIDADE_MEDIA;
            else if (limite_alerta == SENSIBILIDADE_MEDIA)
                limite_alerta = SENSIBILIDADE_ALTA;
            else
                limite_alerta = SENSIBILIDADE_BAIXA;

            exibir_sensibilidade(&display, limite_alerta);
        }

        // Lê o valor do microfone e do joystick
        adc_select_input(2); // Seleciona o canal do microfone (GP28 - ADC2)
        uint16_t valor_microfone = adc_read();
        adc_select_input(0); // Seleciona o canal do eixo X do joystick (GP26 - ADC0)
        uint16_t valor_joystick_x = adc_read();

        // Ajusta a sensibilidade com base no joystick
        limite_alerta += (int)((valor_joystick_x - 2048) / AJUSTE_SENSIBILIDADE); // Center is 2048 (roughly)
        if (limite_alerta < SENSIBILIDADE_BAIXA)
            limite_alerta = SENSIBILIDADE_BAIXA;
        if (limite_alerta > SENSIBILIDADE_ALTA)
            limite_alerta = SENSIBILIDADE_ALTA;
        
        printf("Microfone: %d, Limite: %d\n", valor_microfone, limite_alerta); //add test print
        // Verifica o nível de som e aciona os alertas
        if (valor_microfone > limite_alerta)
        {
            alerta_visual("vermelho", 255); // Aciona LED vermelho com intensidade máxima
            alerta_sonoro();
            ssd1306_fill(&display, false);                          // Limpa o display
            ssd1306_draw_string(&display, "Alerta: Sirene!", 0, 0); // Exibe mensagem
            ssd1306_send_data(&display);                            // Atualiza o display
            sleep_ms(500);
        }
        else if (valor_microfone > limite_alerta / 2)
        {
            alerta_visual("amarelo", 128); // Aciona LED amarelo com intensidade média
            ssd1306_fill(&display, false);
            ssd1306_draw_string(&display, "Alerta: Campainha!", 0, 0);
            ssd1306_send_data(&display);
        }
        else
        {
            alerta_visual("verde", 64); // Aciona LED verde com intensidade baixa
            ssd1306_fill(&display, false);
            ssd1306_draw_string(&display, "Ambiente seguro", 0, 0);
            ssd1306_send_data(&display);
        }

        // Verifica se o botão foi pressionado
        if (debounce(BOTAO_PIN))
        {
            // Aqui você pode adicionar funcionalidades extras, como resetar o sistema
            printf("Botão pressionado!\n");
            exibir_sensibilidade(&display, limite_alerta);
        }

        sleep_ms(100); // Aguarda 100ms antes da próxima leitura
    }
}
