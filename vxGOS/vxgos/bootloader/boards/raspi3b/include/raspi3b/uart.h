#ifndef VXGOS_BOOTLOADER_RASPI3B_UART_H
#define VXGOS_BOOTLOADER_RASPI3B_UART_H

//---
// Public API
//---

/* uart_init() : initilize UART driver */
extern void uart_init(void);

/* uart_puts() : send string through the UART line (convert new line) */
extern void uart_puts(char const * const s);

/* uart_putf() : printf-like puts */
extern void uart_putf(char const * const format, ...);

#endif /* VXGOS_BOOTLOADER_RASPI3B_UART_H */
