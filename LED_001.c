#include <stdint.h>

// RP2040 Hardware Register Addresses
#define TIMER_BASE      0x40054000
#define SIO_BASE        0xD0000000
#define IO_BANK0_BASE   0x40014000
#define PADS_BANK0_BASE 0x4001C000

// Timer registers
#define TIMER_TIMELR    (*(volatile uint32_t *)(TIMER_BASE + 0x1C))  // Timer low 32 bits
#define TIMER_TIMEHR    (*(volatile uint32_t *)(TIMER_BASE + 0x20))  // Timer high 32 bits

// SIO GPIO registers
#define GPIO_OUT        (*(volatile uint32_t *)(SIO_BASE + 0x10))
#define GPIO_OUT_SET    (*(volatile uint32_t *)(SIO_BASE + 0x14))
#define GPIO_OUT_CLR    (*(volatile uint32_t *)(SIO_BASE + 0x18))
#define GPIO_OE         (*(volatile uint32_t *)(SIO_BASE + 0x20))
#define GPIO_OE_SET     (*(volatile uint32_t *)(SIO_BASE + 0x24))

// GPIO25 control registers
#define GPIO25_CTRL     (*(volatile uint32_t *)(IO_BANK0_BASE + 0x0CC))
#define GPIO25_PAD      (*(volatile uint32_t *)(PADS_BANK0_BASE + 0x0C8))

// Function to get current timer value in microseconds
uint64_t get_timer_us() {
    uint32_t hi1, lo, hi2;
    
    // Read timer safely (handle rollover)
    do {
        hi1 = TIMER_TIMEHR;
        lo = TIMER_TIMELR;
        hi2 = TIMER_TIMEHR;
    } while (hi1 != hi2);
    
    return ((uint64_t)hi1 << 32) | lo;
}

// Timer-based delay in microseconds
void timer_delay_us(uint32_t us) {
    uint64_t start = get_timer_us();
    uint64_t current;
    
    do {
        current = get_timer_us();
    } while ((current - start) < us);
}

// Timer-based delay in milliseconds
void timer_delay_ms(uint32_t ms) {
    timer_delay_us(ms * 1000);
}

int main(){
    // Initialize GPIO25 (built-in LED)
    GPIO25_PAD = 0;           // No pull, normal drive
    GPIO25_CTRL = 5;          // Function 5 = SIO output
    GPIO_OE_SET = (1 << 25);  // Enable output for GPIO25
    GPIO_OUT_CLR = (1 << 25); // Start with LED OFF
    
    while(1){
        // Turn LED ON
        GPIO_OUT_SET = (1 << 25);
        timer_delay_ms(500);  // Delay using hardware timer

        // Turn LED OFF
        GPIO_OUT_CLR = (1 << 25);
        timer_delay_ms(500);  // Delay using hardware timer
    }
}