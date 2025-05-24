#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define BUFFER_SIZE 64

void uart_init(void) {
    uint16_t ubrr = 103;
    UBRR0H = (ubrr >> 8);
    UBRR0L = ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send_char(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_send_string(const char* str) {
    while (*str) {
        uart_send_char(*str++);
    }
}

char uart_receive_char(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void uart_receive_line(char* buffer, uint8_t maxlen) {
    uint8_t i = 0;
    char c;
    while (i < maxlen - 1) {
        c = uart_receive_char();
        if (c == '\r') continue;
        if (c == '\n') break;
        buffer[i++] = c;
    }
    buffer[i] = '\0';
}

int main(void) {
    char buffer[BUFFER_SIZE];
    uart_init();
    _delay_ms(1000);

    while (1) {
        uart_receive_line(buffer, BUFFER_SIZE);
        uart_send_string("");
        uart_send_string(buffer);
        uart_send_string("\r\n");
    }
}
