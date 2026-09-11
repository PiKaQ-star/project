#ifndef __UART_H
#define __UART_H

void uart_init(void);

void send_char(char ch);

void send_str(char*str);

void get_data(void);

#endif
