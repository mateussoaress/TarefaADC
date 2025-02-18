#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#define VRY_PIN 27  // Pino do eixo Y do joystick
#define VRX_PIN 26  // Pino do eixo X do joystick
#define LED_B_PIN 12 // Pino do LED Azul
#define LED_R_PIN 13 // Pino do LED Vermelho

uint pwm_init_gpio(uint gpio, uint wrap) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice, wrap);
    pwm_set_enabled(slice, true);
    return slice;
}

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(VRY_PIN);
    adc_gpio_init(VRX_PIN);

    uint wrap = 4095;
    pwm_init_gpio(LED_B_PIN, wrap);
    pwm_init_gpio(LED_R_PIN, wrap);

    while (true) {
        // Controle do LED Azul pelo eixo Y
        adc_select_input(1);
        uint16_t vry = adc_read();
        if (vry < 2040) {
            pwm_set_gpio_level(LED_B_PIN, vry * 2);
        } else if (vry > 2060) {
            pwm_set_gpio_level(LED_B_PIN, (vry - 2048) * 2);
        } else {
            pwm_set_gpio_level(LED_B_PIN, 0);
        }

        // Controle do LED Vermelho pelo eixo X
        adc_select_input(0);
        uint16_t vrx = adc_read();
        if (vrx < 2040) {
            pwm_set_gpio_level(LED_R_PIN, vrx * 2);
        } else if (vrx > 2060) {
            pwm_set_gpio_level(LED_R_PIN, (vrx - 2048) * 2);
        } else {
            pwm_set_gpio_level(LED_R_PIN, 0);
        }

        printf("Eixo Y: %u, Eixo X: %u\n", vry, vrx);
        sleep_ms(100);
    }
}
