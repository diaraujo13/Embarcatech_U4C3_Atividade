#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

#include "pio_matrix.pio.h"

#define NUM_LEDS 25 // Número de LEDs na matriz
#define OUT_PIN 7   // Pino de dados conectado à matriz

#define BUZZER_PIN 28

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

const uint8_t row_pins[KEYPAD_ROWS] = {2, 3, 4, 5};
const uint8_t column_pins[KEYPAD_COLS] = {6, 10, 8, 9};

const char keypad_map[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

double matrixOn[25] = {
     1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0  
};

double matrixOff[25] = {
 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0 
};
double desenho1[25] = {
     1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0};

double desenho2[25] = {
       1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};
//Animação: Wave
double wave1[25]={
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.5, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5
};
double wave2[25]={
    0.5, 0.0, 0.0, 0.0, 0.0,
    0.5, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.5, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5
};
double wave3[25]={
    0.5, 0.5, 0.0, 0.0, 0.0,
    0.5, 0.5, 0.0, 0.0, 0.0,
    0.5, 0.0, 0.0, 0.0, 0.0,
    0.5, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5
};
double wave4[25]={
    0.5, 0.5, 0.5, 0.0, 0.0,
    0.5, 0.5, 0.5, 0.0, 0.0,
    0.5, 0.5, 0.0, 0.0, 0.0,
    0.5, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5
};
double wave5[25]={
    0.0, 0.5, 0.5, 0.5, 0.0,
    0.5, 0.5, 0.5, 0.5, 0.0,
    0.5, 0.5, 0.5, 0.0, 0.0,
    0.5, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5
};
double wave6[25]={
    0.0, 0.0, 0.5, 0.5, 0.5,
    0.0, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.0,
    0.5, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5
};
double wave7[25]={
    0.0, 0.0, 0.0, 0.5, 0.5,
    0.0, 0.0, 0.5, 0.5, 0.5,
    0.0, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5
};
double wave8[25]={
    0.0, 0.0, 0.0, 0.0, 0.5,
    0.0, 0.0, 0.0, 0.5, 0.5,
    0.0, 0.0, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5
};
double wave9[25]={
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.5,
    0.0, 0.0, 0.0, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5
};
double wave10[25]={
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5
};
//LEDs vermelhos em 80%
double leds_80[25]={
    0.8, 0.8, 0.8, 0.8, 0.8,
    0.8, 0.8, 0.8, 0.8, 0.8,
    0.8, 0.8, 0.8, 0.8, 0.8,
    0.8, 0.8, 0.8, 0.8, 0.8,
    0.8, 0.8, 0.8, 0.8, 0.8
};
uint32_t matrix_rgb(float r, float g, float b)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;

    return (G << 24) | (R << 16) | (B << 8);
}

void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    for (int16_t i = 0; i < NUM_LEDS; i++)
    {
        valor_led = matrix_rgb(r, g, b);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

void initialize_gpio()
{
    for (int row = 0; row < KEYPAD_ROWS; row++)
    {
        gpio_init(row_pins[row]);
        gpio_set_dir(row_pins[row], GPIO_OUT);
        gpio_put(row_pins[row], 0);
    }

    for (int col = 0; col < KEYPAD_COLS; col++)
    {
        gpio_init(column_pins[col]);
        gpio_set_dir(column_pins[col], GPIO_IN);
        gpio_pull_down(column_pins[col]);
    }

    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);

    gpio_init(OUT_PIN);
    gpio_set_dir(OUT_PIN, GPIO_OUT);
}

char read_keypad()
{
    for (int row = 0; row < KEYPAD_ROWS; row++)
    {
        gpio_put(row_pins[row], 1);

        for (int col = 0; col < KEYPAD_COLS; col++)
        {
            if (gpio_get(column_pins[col]))
            {
                gpio_put(row_pins[row], 0);
                return keypad_map[row][col];
            }
        }
        gpio_put(row_pins[row], 0);
    }

    return 0;
}

void buzzer_beep()
{
    for (int i = 0; i < 100; i++)
    {
        gpio_put(BUZZER_PIN, 1);
        sleep_us(50);
        gpio_put(BUZZER_PIN, 0);
        sleep_us(50);
    }
}

int main()
{

    PIO pio = pio0;
    bool ok;
    uint16_t i;
    uint32_t valor_led;
    double r = 0.0, b = 0.0, g = 0.0;

    ok = set_sys_clock_khz(128000, false);

    printf("iniciando a transmissão PIO");
     if (ok) printf("clock set to %id\n", clock_get_hz(clk_sys));


    stdio_init_all();
    initialize_gpio();

    uint sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &pio_matrix_program);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    printf(">> Pressione uma tecla...\n");

    while (true)
    {
        char key = read_keypad();

        if (key)
        {
            switch (key)
            {
            case '1':
                /*printf("Pressed");
                double *frames[] = {wave1, wave2, wave3, wave4, wave5,wave6, wave7, wave8, wave9, wave10};

                desenho_pio(matrixOff, valor_led, pio, sm, 0, 0, 0);
                r = 0;
                g = 0;
                b = 1;
                while (true)
                {
                    for (i = 0; i < 10; i++)
                    {
                    desenho_pio(frames[i], valor_led, pio, sm, r, g, b);
                    sleep_ms(300);
                    }
                }*/
                r = 0;
                g = 0;
                b = 1;
                printf("Pressed");
                desenho_pio(wave1, valor_led, pio, sm, r, g, b);
                break;
            case '4':
            case '7':
            case '0':
            case 'C':
                r = 1;
                g = 0;
                b = 0;
                printf("Pressed");
                desenho_pio(leds_80, valor_led, pio, sm, r, g, b);
                break;
            case '2':
            case '5':
            case '8':
            case 'B':
                desenho_pio(desenho1, valor_led, pio, sm, r, g, b);
                break;
            case 'A':
                r = 0;
                g = 0;
                b = 0;
                desenho_pio(matrixOff, valor_led, pio, sm, r, g, b);
                break;         
            case '3':
            case '6':
            case '9':
                desenho_pio(desenho1, valor_led, pio, sm, r, g, b);
                break;
            case 'D':
            printf("Pressed");
                r = 0;
                g = 0;
                b = 1;
                desenho_pio(desenho1, valor_led, pio, sm, r, g, b);
                break;
            case '#':
              buzzer_beep();
                r =1;
                g = 0;
                b = 0;
                desenho_pio(desenho1, valor_led, pio, sm, r, g, b);
                break;
            case '*':
                break;
            }
            
            sleep_ms(100); // debounce
        }

        sleep_ms(50); // debounce
    }

    return 0;
}
