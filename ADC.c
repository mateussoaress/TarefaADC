#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

// Definições de pinos para os componentes
#define VRY_PIN 27
#define VRX_PIN 26
#define LED_BLUE_PIN 13
#define LED_RED_PIN 12
#define LED_GREEN_PIN 11
#define BUTTON_PIN 22
#define BUTTON_A_PIN 5

// Definições para o display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SQUARE_SIZE 8
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define DISPLAY_ADDRESS 0x3C

// Variáveis globais para estados dos LEDs e debounce de botões
volatile bool led_green_state = false;
volatile bool leds_active = true;
volatile uint32_t last_interrupt_time_green = 0;
volatile uint32_t last_interrupt_time_leds = 0;

// Inicialização do display OLED e posição inicial do quadrado
ssd1306_t disp;
int square_x = (SCREEN_WIDTH - SQUARE_SIZE) / 2;
int square_y = (SCREEN_HEIGHT - SQUARE_SIZE) / 2;

// Callback para tratamento das interrupções dos botões
void button_callback(uint gpio, uint32_t events) {
    uint32_t current_time = time_us_32();
    if (gpio == BUTTON_PIN && (current_time - last_interrupt_time_green > 200000)) {
        led_green_state = !led_green_state;
        gpio_put(LED_GREEN_PIN, led_green_state);
        last_interrupt_time_green = current_time;
    } else if (gpio == BUTTON_A_PIN && (current_time - last_interrupt_time_leds > 200000)) {
        leds_active = !leds_active;
        if (!leds_active) {
            pwm_set_gpio_level(LED_BLUE_PIN, 0);
            pwm_set_gpio_level(LED_RED_PIN, 0);
        }
        last_interrupt_time_leds = current_time;
    }
}

// Função para inicializar o PWM em um pino específico
uint pwm_init_gpio(uint gpio, uint wrap) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);
    return slice_num;
}

// Atualiza o display com a nova posição do quadrado
void update_display() {
    ssd1306_fill(&disp, false);
    ssd1306_rect(&disp, square_x, square_y, SQUARE_SIZE, SQUARE_SIZE, true, false);
    ssd1306_send_data(&disp);
}

int main() {
    // Inicialização do sistema
    stdio_init_all();
    adc_init();
    adc_gpio_init(VRY_PIN);
    adc_gpio_init(VRX_PIN);
    
    // Configuração dos LEDs e botões
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_put(LED_GREEN_PIN, false);
    
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    
    // Configuração das interrupções dos botões
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    
    // Configuração do PWM para controle dos LEDs RGB
    uint pwm_wrap = 4095;
    pwm_init_gpio(LED_BLUE_PIN, pwm_wrap);
    pwm_init_gpio(LED_RED_PIN, pwm_wrap);
    
    // Configuração do barramento I2C e do display OLED
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    ssd1306_init(&disp, SCREEN_WIDTH, SCREEN_HEIGHT, false, DISPLAY_ADDRESS, I2C_PORT);
    ssd1306_config(&disp);
    update_display();
    
    while (true) {
        if (leds_active) {
            // Leitura do eixo Y e atualização do PWM do LED Azul
            adc_select_input(1);
            uint16_t vry_value = adc_read();  
            uint16_t pwm_value_blue = (vry_value > 2048) ? (vry_value - 2048) * 2 : (2048 - vry_value) * 2;
            pwm_set_gpio_level(LED_BLUE_PIN, pwm_value_blue);
            
            // Leitura do eixo X e atualização do PWM do LED Vermelho
            adc_select_input(0);
            uint16_t vrx_value = adc_read();
            uint16_t pwm_value_red = (vrx_value > 2048) ? (vrx_value - 2048) * 2 : (2048 - vrx_value) * 2;
            pwm_set_gpio_level(LED_RED_PIN, pwm_value_red);
            
            // Atualização da posição do quadrado no display
            square_x = ((4095 - vrx_value) * (SCREEN_WIDTH - SQUARE_SIZE)) / 4095;
            square_y = ((4095 - vry_value) * (SCREEN_HEIGHT - SQUARE_SIZE)) / 4095;
            
            update_display();
            
            // Impressão dos valores lidos e estados dos LEDs
            printf("VRY: %u, PWM_BLUE: %u | VRX: %u, PWM_RED: %u | LED_GREEN: %d | LEDs Active: %d\n", 
                   vry_value, pwm_value_blue, vrx_value, pwm_value_red, led_green_state, leds_active);
        }
        sleep_ms(100);
    }
    return 0;
}
