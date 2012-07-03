#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

static void uart_init(void) {
	const uint32_t baud = 9600;
	UBRR0 = ((F_CPU/(baud<<4))-1);
	UCSR0A = 0;
	/*async no parity 1 stop bit, 8-bit data*/
        UCSR0C = _BV(UCSZ01)|_BV(UCSZ00);
	/* Enable rx and tx, no interrupts */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
}

static int uart_putchar(char c, FILE *stream) {
	/* UDRE0: set when UDR0 is empty (so we can xmit) */
	while((UCSR0A&(1<<UDRE0)) == 0);
	UDR0 = c;
	return 0;
}

static int uart_getchar(FILE *stream) {
	while((UCSR0A&(1<<RXC0)) == 0);
	return UDR0;
}

static FILE uart = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int main() {
	stdin = &uart;
	stdout = &uart;
	uart_init();
	DDRD = 1<<1;
	while(1) {
		int16_t c = getchar();
		putchar(c);
		if(c == '\r') putchar('\n');
	}
	return 0;
}
