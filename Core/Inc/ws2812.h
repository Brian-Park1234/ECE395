#ifndef __WS2812_H
#define __WS2812_H

#include "main.h"

// Buffer allocated will be twice this
#define BUFFER_SIZE 24

/* Timer ARR / counts
 *
 * For a 64 MHz timer clock:
 *  - WS2812 bit period = 1.25 us -> 1.25e-6 * 64e6 = 80 ticks
 *  - Timer counts 0..ARR inclusive, so ARR = 80 - 1 = 79
 */
#define LED_CNT    79    // ARR value (counts from 0..79 => 80 ticks total)

/* Explicit ON/OFF counts for WS2812 at 64 MHz timer
 *
 * T0H (≈0.35 us) -> 0.35e-6 * 64e6 = 22.4 -> use 22
 * T1H (≈0.70 us) -> 0.70e-6 * 64e6 = 44.8 -> use 45
 */
#define LED_OFF 22   // High time counts for a "0" bit
#define LED_ON  45   // High time counts for a "1" bit

#define LED_RESET_CYCLES 10          // Full 24-bit cycles

#define GL 0 // Green LED
#define RL 1 // Red LED
#define BL 2 // Blue LED

/* ... rest unchanged ... */

typedef enum {
    WS2812_Ok,
    WS2812_Err,
    WS2812_Mem
} ws2812_resultTypeDef;

/* Statemachine states */
typedef enum {
    LED_RES = 0,    // Reset latch cycle
    LED_IDL = 1,    // Idle doing nothing except waiting for is_dirty
    LED_DAT = 2     // Transferring led data - one led at the time
} ws2812_stateTypeDef;

typedef struct {
    TIM_HandleTypeDef *timer;               // Timer running the PWM - MUST run at 800 kHz
    uint32_t channel;                       // Timer channel
    uint16_t dma_buffer[BUFFER_SIZE * 2];   // Fixed size DMA buffer
    uint16_t leds;                          // Number of LEDs on the string
    uint8_t *led;                           // Dynamically allocated array of LED RGB values
    ws2812_stateTypeDef led_state;          // LED Transfer state machine
    uint8_t led_cnt;                        // Counts through the leds starting from zero up to "leds"
    uint8_t res_cnt;                        // Counts reset cycles when in reset state
    uint8_t is_dirty;                       // Indicates to the call back that the led color values have been updated
    uint8_t zero_halves;                    // Counts halves send during reset
    uint32_t dma_cbs;                       // Just used for statistics
    uint32_t dat_cbs;                       // Also used for statistics
} ws2812_handleTypeDef;

ws2812_resultTypeDef ws2812_init(ws2812_handleTypeDef *ws2812, TIM_HandleTypeDef *timer, uint32_t channel, uint16_t leds);

void ws2812_update_buffer(ws2812_handleTypeDef *ws2812, uint16_t *dma_buffer_pointer);

// Set all led values to zero
ws2812_resultTypeDef zeroLedValues(ws2812_handleTypeDef *ws2812);

// Set a single led value
ws2812_resultTypeDef setLedValue(ws2812_handleTypeDef *ws2812, uint16_t led, uint8_t color, uint8_t value);

// Set values of all 3 leds
ws2812_resultTypeDef setLedValues(ws2812_handleTypeDef *ws2812, uint16_t led, uint8_t r, uint8_t g, uint8_t b);

#endif // _WS2812_H
