#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

#include "pio_matrix.pio.h"

#define L (0.6) // Luminosidade dos leds
#define V (150) // Velocidade de transição animação tecla 5

#define NUM_LEDS 25 // Número de LEDs na matriz
#define OUT_PIN 7   // Pino de dados conectado à matriz

#define BUZZER_PIN 28

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

const uint8_t row_pins[KEYPAD_ROWS] = {2, 3, 4, 5};
const uint8_t column_pins[KEYPAD_COLS] = {6, 10, 8, 9};

int PHYSICAL_LEDS_MAPPER[25] = {
    24, 23, 22, 21, 20,
    15, 16, 17, 18, 19,
    14, 13, 12, 11, 10,
    5, 6, 7, 8, 9,
    4, 3, 2, 1, 0};

const char keypad_map[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

// Matriz para o botão 3
double drawing1[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0, 
                        0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0};

double drawing2[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0, 
                        0.0, 0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0};

double drawing3[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 1.0, 1.0, 1.0, 0.0, 
                        0.0, 1.0, 0.0, 1.0, 0.0,
                        0.0, 1.0, 1.0, 1.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0};

double drawing4[25] =   {1.0, 1.0, 1.0, 1.0, 1.0,
                        1.0, 0.0, 0.0, 0.0, 1.0, 
                        1.0, 0.0, 0.0, 0.0, 1.0,
                        1.0, 0.0, 0.0, 0.0, 1.0,
                        1.0, 1.0, 1.0, 1.0, 1.0};
// Fim matriz botão 3

double matrixOn[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0};

double matrixOff[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0};
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
double ddesenho1[25] = {0.0, 0.0, 0.0, 0.0, L,
                        0.0, 0.0, 0.0, 0.0, L,
                        0.0, 0.0, 0.0, 0.0, L,
                        0.0, 0.0, 0.0, 0.0, L,
                        0.0, 0.0, 0.0, 0.0, L};

double ddesenho2[25] = {0.0, 0.0, 0.0, L, L/8,
                        0.0, 0.0, 0.0, L, L/8,
                        0.0, 0.0, 0.0, L, L/8,
                        0.0, 0.0, 0.0, L, L/8,
                        0.0, 0.0, 0.0, L, L/8};

double ddesenho3[25] = {0.0, 0.0, L, L/8, L/16,
                        0.0, 0.0, L, L/8, L/16,
                        0.0, 0.0, L, L/8, L/16,
                        0.0, 0.0, L, L/8, L/16,
                        0.0, 0.0, L, L/8, L/16};

double ddesenho4[25] = {0.0, L, L/8, L/16, 0.0,
                        0.0, L, L/8, L/16, 0.0,
                        0.0, L, L/8, L/16, 0.0,
                        0.0, L, L/8, L/16, 0.0,
                        0.0, L, L/8, L/16, 0.0};

double ddesenho5[25] = {L, L/8, L/16, 0.0, 0.0,
                        L, L/8, L/16, 0.0, 0.0,
                        L, L/8, L/16, 0.0, 0.0,
                        L, L/8, L/16, 0.0, 0.0,
                        L, L/8, L/16, 0.0, 0.0};

double ddesenho6[25] = {L/8, L/16, 0.0, 0.0, L,
                        L/8, L/16, 0.0, 0.0, L,
                        L/8, L/16, 0.0, 0.0, L,
                        L/8, L/16, 0.0, 0.0, L,
                        L/8, L/16, 0.0, 0.0, L};

double ddesenho7[25] = {L/16, 0.0, 0.0, L, L/8,
                        L/16, 0.0, 0.0, L, L/8,
                        L/16, 0.0, 0.0, L, L/8,
                        L/16, 0.0, 0.0, L, L/8,
                        L/16, 0.0, 0.0, L, L/8};

double seta1[25] =   {0.0, 0.0, 0.3, 0.0, 0.0,
                        0.0, 0.3, 0.3, 0.3, 0.0, 
                        0.3, 0.0, 0.3, 0.0, 0.3,
                        0.0, 0.0, 0.3, 0.0, 0.0,
                        0.0, 0.0, 0.3, 0.0, 0.0};

double seta2[25] =   {0.0, 0.3, 0.3, 0.3, 0.0,
                        0.3, 0.0, 0.3, 0.0, 0.3, 
                        0.0, 0.0, 0.3, 0.0, 0.0,
                        0.0, 0.0, 0.3, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0};
            
double seta3[25] =   {0.3, 0.0, 0.3, 0.0, 0.3,
                        0.0, 0.0, 0.3, 0.0, 0.0, 
                        0.0, 0.0, 0.3, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.3, 0.0, 0.0};

double seta4[25] =   {0.0, 0.0, 0.3, 0.0, 0.0,
                        0.0, 0.0, 0.3, 0.0, 0.0, 
                        0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.3, 0.0, 0.0,
                        0.0, 0.3, 0.3, 0.3, 0.0};            

double seta5[25] =   {0.0, 0.0, 0.3, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0, 
                        0.0, 0.0, 0.3, 0.0, 0.0,
                        0.0, 0.3, 0.3, 0.3, 0.0,
                        0.3, 0.0, 0.3, 0.0, 0.3};

double seta6[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.3, 0.0, 0.0, 
                        0.0, 0.3, 0.3, 0.3, 0.0,
                        0.3, 0.0, 0.3, 0.0, 0.3,
                        0.0, 0.0, 0.3, 0.0, 0.0};
//Animação: Wave
double wave1[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0};

double wave2[25] = {1.0, 0.0, 0.0, 0.0, 0.0,
                    1.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0};

double wave3[25] = {1.0, 1.0, 0.0, 0.0, 0.0,
                    1.0, 1.0, 0.0, 0.0, 0.0,
                    1.0, 0.0, 0.0, 0.0, 0.0,
                    1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0};

double wave4[25] = {1.0, 1.0, 1.0, 0.0, 0.0,
                    1.0, 1.0, 1.0, 0.0, 0.0,
                    1.0, 1.0, 0.0, 0.0, 0.0,
                    1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0};

double wave5[25] = {0.0, 1.0, 1.0, 1.0, 0.0,
                    1.0, 1.0, 1.0, 1.0, 0.0,
                    1.0, 1.0, 1.0, 0.0, 0.0,
                    1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0};

double wave6[25] = {0.0, 0.0, 1.0, 1.0, 1.0,
                    0.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 0.0,
                    1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0};

double wave7[25] = {0.0, 0.0, 0.0, 1.0, 1.0,
                    0.0, 0.0, 1.0, 1.0, 1.0,
                    0.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0};

double wave8[25] = {0.0, 0.0, 0.0, 0.0, 1.0,
                    0.0, 0.0, 0.0, 1.0, 1.0,
                    0.0, 0.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0};

double wave9[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 1.0,
                    0.0, 0.0, 0.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0};

double wave10[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
                     0.0, 0.0, 0.0, 0.0, 0.0,
                     0.0, 0.0, 0.0, 0.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0};

void buzzer_beep();
void initialize_gpio();
char read_keypad();
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);
void desenho_pio_anim(double *desenho, PIO pio, uint sm, double r, double g, double b);
void clockwise_rotation_ani(PIO pio, uint sm);
void padrao1(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);
void padrao2(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);
void padrao3(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);
void padrao4(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);
void padrao5(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);

void draw_animation(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);
void turn_off_leds(PIO pio, uint sm);
void turn_off_individual_led(PIO pio, uint sm, int led_position);


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

void turn_off_leds(PIO pio, uint sm) {
    uint32_t valor_led = 0;
    double r = 0.0, g = 0.0, b = 0.0;
    desenho_pio(matrixOff, valor_led, pio, sm, r, g, b);
}

void turn_off_individual_led(PIO pio, uint sm, int led_position) {
    printf("led %d", led_position);
    uint32_t valor_led = 0;
    double r = 0.0, g = 1.0, b = 0.0;
}

void padrao1(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{

    for (int16_t i = 0; i < NUM_LEDS; i++)
    {
        if (i < 5)
        {
            valor_led = matrix_rgb(desenho[24 - i], g = 0.0, b = 0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
        else
        {
            if (i < 10)
            {
                valor_led = matrix_rgb(r = 0.0, desenho[24 - i], b = 0.0);
                pio_sm_put_blocking(pio, sm, valor_led);
            }
            else
            {
                if (i < 15)
                {
                    valor_led = matrix_rgb(r = 0.0, g = 0.0, desenho[24 - i]);
                    pio_sm_put_blocking(pio, sm, valor_led);
                }
                else
                {
                    if (i < 20)
                    {
                        valor_led = matrix_rgb(r = 0.0, desenho[24 - i], desenho[24 - i]);
                        pio_sm_put_blocking(pio, sm, valor_led);
                    }
                    else
                    {
                        valor_led = matrix_rgb(desenho[24 - i], desenho[24 - i], b = 0.0);
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
        valor_led = matrix_rgb(r = 0.0, desenho[24 - i], b = 0.0);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}
void padrao3(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    for (int16_t i = 0; i < NUM_LEDS; i++)
    {
        int led_matrix_location = PHYSICAL_LEDS_MAPPER[i];
        valor_led = matrix_rgb(0.0, 0.0, desenho[led_matrix_location]);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

void padrao4(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    for (int16_t i = 0; i < NUM_LEDS; i++)
    {
        valor_led = matrix_rgb(desenho[24 - i], desenho[24 - i], desenho[24 - i]);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

void padrao5(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    for (int16_t i = 0; i < NUM_LEDS; i++)
    {
        valor_led = matrix_rgb(r=0.0,g=0.0,desenho[24 - i]);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

void desenho_pio_anim(double *desenho, PIO pio, uint sm, double r, double g, double b)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int led_matrix_location = PHYSICAL_LEDS_MAPPER[i];
        uint32_t valor_led = matrix_rgb(desenho[led_matrix_location] * r, desenho[led_matrix_location] * g, desenho[led_matrix_location] * b);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

void clockwise_rotation_ani(PIO pio, uint sm)
{
    double arrow_right[25] = {0};
    arrow_right[2] = arrow_right[8] = arrow_right[10] = arrow_right[18] = arrow_right[22] = 1.0;
    arrow_right[11] = arrow_right[12] = arrow_right[13] = arrow_right[14] = 1.0;

    double arrow_down[25] = {0};
    arrow_down[2] = arrow_down[7] = arrow_down[12] = arrow_down[17] = arrow_down[22] = 1.0;
    arrow_down[16] = arrow_down[14] = arrow_down[10] = arrow_down[18] = 1.0;

    double arrow_left[25] = {0};
    arrow_left[2] = arrow_left[6] = arrow_left[10] = arrow_left[16] = arrow_left[22] = 1.0;
    arrow_left[11] = arrow_left[12] = arrow_left[13] = arrow_left[14] = 1.0;

    double arrow_up[25] = {0};
    arrow_up[2] = arrow_up[6] = arrow_up[12] = arrow_up[17] = arrow_up[22] = 1.0;
    arrow_up[14] = arrow_up[8] = arrow_up[6] = arrow_up[10] = arrow_up[7] = 1.0;

    double arrow_cross[25] = {0};
    arrow_cross[2] = arrow_cross[7] =  arrow_cross[17] = arrow_cross[22] = 0.6;
    arrow_cross[14] = arrow_cross[13] = arrow_cross[12] = arrow_cross[10] = arrow_cross[11] = 1.0;


    double *arrows[5] = {arrow_right, arrow_down, arrow_left, arrow_up, arrow_cross};

    for (int spin = 0; spin < 2; spin++)
    {
        for (int frame = 0; frame < 5; frame++)
        {
            buzzer_beep();
            sleep_ms(50);
            for (int j = 0; j <= 10; j++)
            {
                desenho_pio_anim(arrows[frame], pio, sm, 1, 0, 0);
                sleep_ms(50);
            }
            desenho_pio_anim(arrows[frame], pio, sm, 0.0, 0.0, 0.0);
            sleep_ms(100);
        }
    }

    for (int i = 0; i < 25; i++)
    {
        arrow_right[i] = 0.0;
    }
    desenho_pio_anim(arrow_right, pio, sm, 0.0, 0.0, 0.0);
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

// Função para fazer animação do botão 3
void draw_animation(double *drawing, uint32_t led_value, PIO pio, uint sm, double r, double g, double b){

    for (int16_t i = 0; i < NUM_LEDS; i++)
    {
        if (i % 2==0)
        {
            led_value = matrix_rgb(drawing[24 - i], r = 0.0, g = 0.0);
            pio_sm_put_blocking(pio, sm, led_value);

        }
        else
        {
            led_value = matrix_rgb(b = 0.0, drawing[24 - i], g = 0.0);
            pio_sm_put_blocking(pio, sm, led_value);
        }
    }
} //Fim draw_animation

int main()
{

    PIO pio = pio0;
    bool ok;
    uint16_t i;
    uint32_t valor_led;
    double r = 0.0, b = 0.0, g = 0.0;

    ok = set_sys_clock_khz(128000, false);

    printf("iniciando a transmissão PIO");
    if (ok)
        printf("clock set to %id\n", clock_get_hz(clk_sys));

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
            case '0':
                sm = 0;
                r = 1.0, b = 0.0, g = 0.0;
                clockwise_rotation_ani(pio, sm);
                break;
            case '1':
                printf("Pressed");
                double *frames[] = {wave1, wave2, wave3, wave4, wave5,wave6, wave7, wave8, wave9, wave10, wave1};
                    for (i = 0; i < 11; i++)
                    {
                    padrao3(frames[i], valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                    }
                //Linha 2 com animação invertida
                break;
            case '4':
                r = 0;
                g = 0;
                b = 0;
                for(int i=0;i<3;i++){    
                    padrao5(seta1, valor_led, pio, sm, r, g, b);
                    sleep_ms(200);
                    padrao5(seta2, valor_led, pio, sm, r, g, b);
                    sleep_ms(200);
                    padrao5(seta3, valor_led, pio, sm, r, g, b);
                    sleep_ms(200);
                    padrao5(seta4, valor_led, pio, sm, r, g, b);
                    sleep_ms(200);
                    padrao5(seta5, valor_led, pio, sm, r, g, b);
                    sleep_ms(200);
                    padrao5(seta6, valor_led, pio, sm, r, g, b);
                    sleep_ms(200);
                }
            break;
            case '7':
            case 'C':
                r = 0.8;
                g = 0;
                b = 0;
                printf("Pressed");
                desenho_pio(matrixOn, valor_led, pio, sm, r, g, b);
                break;
            case '2':
                break;
            case '5':
                padrao1(ddesenho1, valor_led, pio, sm, r, g, b);
                sleep_ms(V);
                padrao1(ddesenho2, valor_led, pio, sm, r, g, b);
            for (int i = 0; i < 8; i++)
                {                    
                    
                    padrao1(ddesenho3, valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                    padrao1(ddesenho4, valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                    padrao1(ddesenho5, valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                    padrao1(ddesenho6, valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                    padrao1(ddesenho7, valor_led, pio, sm, r, g, b);
                    sleep_ms(V);
                }
                padrao1(matrixOff, valor_led, pio, sm, r, g, b);
                break;
            case '8':
                turn_off_leds(pio, sm);
            case 'B':
                r = 0.0, b = 1.0, g = 0.0;
                desenho_pio(desenho1, valor_led, pio, sm, r, g, b);
                break;
            case 'A':
                r = 0;
                g = 0;
                b = 0;
                desenho_pio(matrixOff, valor_led, pio, sm, r, g, b);
                break;         
            case '3':
                // Animação para o botão 3
                int count = 0;

                while(count < 10)
                {
                    draw_animation(drawing1, valor_led, pio, sm, r, g, b);
                    sleep_ms(200);
                    draw_animation(drawing2, valor_led, pio, sm, r, g, b);
                    sleep_ms(200);
                    draw_animation(drawing3, valor_led, pio, sm, r, g, b);
                    sleep_ms(200);
                    draw_animation(drawing4, valor_led, pio, sm, r, g, b);
                    sleep_ms(200);

                    count++;
                }

                draw_animation(drawing1, valor_led, pio, sm, r, g, b);

                break;
            case '6':
                sm = 0;
                r = 0.0;
                g = 1;
                b = 0.0;
                desenho_pio(matrixOn, valor_led, pio, sm, r, g, b);
            break;
            case '9':
                desenho_pio(desenho1, valor_led, pio, sm, r, g, b);
                break;
            case 'D':
                padrao2(matrixOn, valor_led, pio, sm, r, g, b);
                break;
            case '#':
                r=0;
                g=0;
                b=0;
                padrao4(matrixOn, valor_led, pio, sm, r, g, b);
                break;
            case '*':
                reset_usb_boot(0,0);
                break;
            }

            sleep_ms(100); // debounce
        }

        sleep_ms(50); // debounce
    }

    return 0;
}
