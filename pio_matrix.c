#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

#include "pio_matrix.pio.h"

#define L (0.8) //Luminosidade dos leds
#define V (200) //Velocidade de transição animação tecla 5

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

double apaga[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0};

double ddesenho1[25] = {0.0, 0.0, 0.0, 0.0, L,
                       0.0, 0.0, 0.0, 0.0, L,
                       0.0, 0.0, 0.0, 0.0, L,
                       0.0, 0.0, 0.0, 0.0, L,
                       0.0, 0.0, 0.0, 0.0, L};

double ddesenho2[25] = {0.0, 0.0, 0.0, L, 0.0,
                       0.0, 0.0, 0.0, L, 0.0,
                       0.0, 0.0, 0.0, L, 0.0,
                       0.0, 0.0, 0.0, L, 0.0,
                       0.0, 0.0, 0.0, L, 0.0};

double ddesenho3[25] = {0.0, 0.0, L, 0.0, 0.0,
                       0.0, 0.0, L, 0.0, 0.0,
                       0.0, 0.0, L, 0.0, 0.0,
                       0.0, 0.0, L, 0.0, 0.0,
                       0.0, 0.0, L, 0.0, 0.0};

double ddesenho4[25] = {0.0, L, 0.0, 0.0, 0.0,
                       0.0, L, 0.0, 0.0, 0.0,
                       0.0, L, 0.0, 0.0, 0.0,
                       0.0, L, 0.0, 0.0, 0.0,
                       0.0, L, 0.0, 0.0, 0.0};

double ddesenho5[25] = {L, 0.0, 0.0, 0.0, 0.0,
                       L, 0.0, 0.0, 0.0, 0.0,
                       L, 0.0, 0.0, 0.0, 0.0,
                       L, 0.0, 0.0, 0.0, 0.0,
                       L, 0.0, 0.0, 0.0, 0.0};

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
void padrao1(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{

    for (int16_t i = 0; i < NUM_LEDS; i++)
    {
        if (i < 5)
        {
            valor_led = matrix_rgb(desenho[24 - i], r = 0.0, g = 0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
        }else{
        if (i < 10)
        {
            valor_led = matrix_rgb(b = 0.0, desenho[24 - i], g=0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
            
        }else{
            if(i<15){
                valor_led = matrix_rgb(b = 0.0, r=0.0, desenho[24 - i]);
                pio_sm_put_blocking(pio, sm, valor_led);
            }else{
                if(i<20){
                    valor_led = matrix_rgb(b = 0.0, desenho[24 - i], desenho[24 - i]);
                    pio_sm_put_blocking(pio, sm, valor_led);
                }else{
                    valor_led = matrix_rgb(desenho[24 - i], desenho[24 - i] , g=0.0 );
                    pio_sm_put_blocking(pio, sm, valor_led);
                }
            }
        }
        }
    }
}
void padrao2(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    for (int16_t i = 0; i < NUM_LEDS; i++)
    {
            valor_led = matrix_rgb(b=0.0, r=0.0, desenho[24 - i]);
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
    double r = 0.0, b = 0.0, g = 1.0;

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
            case '4':
            case '7':
            case '0':
            case 'C':
                printf("Pressed");
                desenho_pio(desenho2, valor_led, pio, sm, r, g, b);
                sleep_ms(100);
                break;
            case '2':
            case '5':
                for(int i=0;i<3;i++)
                {
                    sleep_ms(V);
                    padrao1(ddesenho1, valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                    padrao1(ddesenho2, valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                    padrao1(ddesenho3, valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                    padrao1(ddesenho4, valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                    padrao1(ddesenho5, valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                    padrao1(ddesenho4, valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                    padrao1(ddesenho3, valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                    padrao1(ddesenho2, valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                    padrao1(ddesenho1, valor_led, pio, sm, r, g, b);
                }
                padrao1(apaga, valor_led, pio, sm, r, g, b);
                break;
            case '8':
            case 'B':
                desenho_pio(desenho1, valor_led, pio, sm, r, g, b);
                break;
            case 'A':
            case '3':
            case '6':
            case '9':
                desenho_pio(desenho1, valor_led, pio, sm, r, g, b);
                break;
            case 'D':
                padrao2(matrixOn, valor_led, pio, sm, r, g, b);
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
