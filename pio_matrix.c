#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"  

#include "pio_matrix.pio.h"

#define NUM_LEDS 25 // Número de LEDs na matriz
#define DATA_PIN 7  // Pino de dados conectado à matriz


#define BUZZER_PIN 28

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

const uint8_t row_pins[KEYPAD_ROWS] = {2, 3, 4, 5};
const uint8_t column_pins[KEYPAD_COLS] = {6, 7, 8, 9};

const char keypad_map[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void initialize_gpio()
{
   
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);
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
    for (int i = 0; i < 100; i++) {
        gpio_put(BUZZER_PIN, 1);
        sleep_us(50);
        gpio_put(BUZZER_PIN, 0);
        sleep_us(50);
    }
}


int main()
{
    stdio_init_all();
    initialize_gpio();

    printf(">> Pressione uma tecla...\n");

    while (true) {
        char key = read_keypad();

        if (key) {
            switch (key) {
                case '1':
                case '4':
                case '7':
                case '0':
                case 'C':
                    break;
                case '2':
                case '5':
                case '8':
                case 'B':
                    break;
                case 'A':
                case '3':
                case '6':
                case '9':
                    break;
                case 'D':
                    break;
                case '#':
                    break;
                case '*':
                    break;
            }
        }

        sleep_ms(50); // debounce
    }

    return 0;
}
