#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"

#define NUM_LEDS 25 // Número de LEDs na matriz
#define DATA_PIN 7  // Pino de dados conectado à matriz
// Função para enviar um byte para a matriz de LEDs
void send_byte(uint8_t byte)
{
    for (int i = 0; i < 8; i++)
    {
        if (byte & (1 << (7 - i)))
        {
            gpio_put(DATA_PIN, 1); // Envia 1
            sleep_us(1);           // Ajuste de timing para 1
            gpio_put(DATA_PIN, 0); // Transição para 0
            sleep_us(1);           // Ajuste de timing para 0
        }
        else
        {
            gpio_put(DATA_PIN, 1); // Envia 0
            sleep_us(1);           // Ajuste de timing para 1
            gpio_put(DATA_PIN, 0); // Transição para 0
            sleep_us(2);           // Ajuste de timing para 0
        }
    }
} // Função para atualizar a matriz de LEDs
void update_leds(uint8_t led_data[][3])
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        send_byte(led_data[i][0]); // Envia vermelho
        send_byte(led_data[i][1]); // Envia verde
        send_byte(led_data[i][2]); // Envia azul
    } // Reset
    sleep_us(50); // Tempo de reset
}
int main()
{
    stdio_init_all();
    gpio_init(DATA_PIN);
    gpio_set_dir(DATA_PIN, GPIO_OUT);
    uint8_t led_data[NUM_LEDS][3]; // Array para armazenar cores (RGB)
    // Inicializa os LEDs com uma cor
    for (int i = 0; i < NUM_LEDS; i++)
    {
        led_data[i][0] = 255; // Vermelho
        led_data[i][1] = 0;   // Verde
        led_data[i][2] = 0;   // Azul
    }
    while (true)
    {
        update_leds(led_data); // Atualiza a matriz
        sleep_ms(1000);        // Espera 1 segundo
    }
}