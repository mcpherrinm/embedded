#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(BADISR_vect) {
  // Catch-all interrupt handler.
}



static void uart_init(void) {
	const uint32_t baud = 9600;
	const uint32_t ubbr = ((F_CPU/(baud<<4))-1);
	/*UBRR0H = (uint8_t)(ubbr>>8);
	UBRR0L = (uint8_t)(ubbr);*/
	UBRR0 = ubbr; /* uint16_t combination of above??? */

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
	int delay = 1000;
	while((UCSR0A&(1<<RXC0)) == 0 && delay-- > 0);
	if((UCSR0A&(1<<RXC0)) == 0) return -1;
	return UDR0;
}

static FILE uart = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int main() {
	stdin = &uart;
	stdout = &uart;
	uart_init();
	DDRD = 1<<3|1<<1;
	PORTD |= 1<<2;
	int last = 0;
	printf("Last: %p\n", &last);
	while(1) {
		/* Debug to make sure main loop is running */
		if(PIND & 1<<2) {
			PORTD &= ~(1<<3);
			if(last) {
				puts("toggle!");
				last = 0;
			}
		} else {
			PORTD |= 1<<3;
			if(!last) {
				puts("toggle!");
				last = 1;
			}
		}
		/* Echo console? okay */
		int16_t c = getchar();
		if(c != -1) putchar(c);
		if(c == '\r') putchar('\n');
	}
	return 0;
}
