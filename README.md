# Controle de Display OLED com Joystick no RP2040

## Introdução
Este projeto implementa o controle de um display OLED SSD1306 utilizando um joystick analógico no microcontrolador RP2040. Além disso, ele gerencia LEDs RGB via PWM e responde a eventos de botões físicos para modificar estados dos LEDs. O objetivo principal é demonstrar a interação entre sensores analógicos, displays gráficos e sinais PWM dentro de um sistema embarcado.

## Funcionamento do Código

### Inicialização do Sistema
O código começa com a inclusão das bibliotecas necessárias, incluindo `hardware/adc.h` para leitura do joystick, `hardware/pwm.h` para controle dos LEDs e `hardware/i2c.h` para comunicação com o display OLED SSD1306. Em seguida, são definidas as constantes dos pinos utilizados para cada componente.

### Configuração do Joystick
O joystick é conectado às entradas ADC do RP2040. As funções `adc_init()` e `adc_gpio_init(VRY_PIN)` configuram os pinos analógicos necessários. A leitura dos valores do joystick é feita utilizando `adc_select_input()` seguido por `adc_read()`, que retorna um valor entre 0 e 4095. Esse valor é processado para determinar o movimento do quadrado na tela e o brilho dos LEDs RGB.

### Controle dos LEDs via PWM
Os LEDs RGB são controlados por sinais PWM, onde a intensidade dos LEDs azul e vermelho varia de acordo com os eixos Y e X do joystick, respectivamente. O código configura os pinos PWM com `gpio_set_function(gpio, GPIO_FUNC_PWM)`, define um valor de `wrap` para determinar a resolução do PWM e ajusta o brilho com `pwm_set_gpio_level(LED_BLUE_PIN, pwm_value_blue)`. 

### Configuração do Display OLED
O display SSD1306 se comunica com o RP2040 via I2C. O código inicializa a comunicação com `i2c_init(I2C_PORT, 400 * 1000)` e configura os pinos SDA e SCL para a função I2C. A função `ssd1306_init()` inicializa o display, e `ssd1306_fill()` limpa a tela antes de cada atualização. O quadrado é desenhado com `ssd1306_rect()` e a função `ssd1306_send_data()` envia os dados atualizados para o display.

### Interrupção dos Botões
Os botões são configurados como entradas com `gpio_set_dir(BUTTON_PIN, GPIO_IN)`, e utilizam `gpio_pull_up()` para manter um estado estável. As interrupções são ativadas com `gpio_set_irq_enabled_with_callback()`, que chama a função `button_callback()` quando um botão é pressionado. Essa função alterna o estado dos LEDs e desativa o controle dos LEDs RGB quando necessário.

### Loop Principal
No loop principal, o código monitora constantemente os valores do joystick, ajusta os LEDs RGB e atualiza a posição do quadrado no display OLED. Para evitar atualizações excessivas, o código inclui um `sleep_ms(100)`, garantindo uma taxa de atualização estável sem sobrecarregar o processador.

## Configuração do Hardware
A configuração do hardware inclui um Raspberry Pi Pico (RP2040) conectado ao joystick analógico e ao display OLED SSD1306 via I2C, além de LEDs RGB e botões físicos. O código é estruturado para garantir uma resposta eficiente aos eventos de entrada e saída, sendo desenvolvido em C utilizando o Pico SDK. Ele emprega interrupções para leitura de botões, timers para debounce e comunicação I2C para o controle do display OLED.

## Aplicação e Expansibilidade
O projeto serve como uma excelente aplicação para quem deseja explorar conceitos fundamentais de sistemas embarcados, incluindo leitura de sensores analógicos, controle de periféricos via PWM e manipulação de exibição gráfica. Ele demonstra de maneira prática como interagir com componentes externos e otimizar a resposta do sistema a eventos em tempo real. Além disso, sua modularidade permite futuras expansões, como a adição de novos controles ou a melhoria da interface gráfica exibida no display.

## Link do Vídeo de Explicação: