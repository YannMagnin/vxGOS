#include <stdarg.h>
#include <stdio.h>

#include "raspi3b/uart.h"
#include "raspi3b/auxiliary.h"
#include "raspi3b/gpio.h"
#include "raspi3b/mailbox.h"

//---
// Internals
//---

/* bcm2837_uart1_init() : initialize the MiniUART1 module */
static void bcm2837_uart1_init(void)
{
    /* initialize UART
     *
     * @note
     * <> disalbe UART interruption
     * <> enable UART module
     * <> clear Tx and Rx FIFOs
     * <> setup to 115200 baud
     *      baudrate = system_clk_freq / (8 * (baud_reg + 1))
     *      baud_reg = ((system_clk_freq / baudrate) / 8) - 1 */
    BCM2837_AUXILIARY.AUX_ENABLE.MiniUART       = 1;

    /* disable as soon as possible Tx and Rx */
    BCM2837_AUXILIARY.AUX_MU_CNTL.RE            = 0;
    BCM2837_AUXILIARY.AUX_MU_CNTL.TE            = 0;

    /* disable Tx or Rx interrupts */
    BCM2837_AUXILIARY.AUX_MU_IER.TIE            = 0;
    BCM2837_AUXILIARY.AUX_MU_IER.RIE            = 0;

    /* clear FIFOs (FIFOs are always enabled) */
    //FIXME : be sure to not force enable FIFO channels
    BCM2837_AUXILIARY.AUX_MU_IIR.TXFLUSH        = 1;
    BCM2837_AUXILIARY.AUX_MU_IIR.RXFLUSH        = 1;

    /* Configure data format */
    BCM2837_AUXILIARY.AUX_MU_LCR.DLAB           = 0;
    BCM2837_AUXILIARY.AUX_MU_LCR.BREAK          = 0;
    BCM2837_AUXILIARY.AUX_MU_LCR.CHR            = 0b11;
    BCM2837_AUXILIARY.AUX_MU_MCR.RTS            = 0;

    /* Configure extra features */
    BCM2837_AUXILIARY.AUX_MU_CNTL.ERAF          = 0;
    BCM2837_AUXILIARY.AUX_MU_CNTL.ETAF          = 0;
    BCM2837_AUXILIARY.AUX_MU_CNTL.RTS_AUTO      = 0b00;
    BCM2837_AUXILIARY.AUX_MU_CNTL.RTS_ASSERT    = 0;
    BCM2837_AUXILIARY.AUX_MU_CNTL.CTS_ASSERT    = 0;

    /* Setup baudrate */
    //FIXME : find module freq, for now assum that is 250MHz
    BCM2837_AUXILIARY.AUX_MU_LCR.DLAB           = 1;
    BCM2837_AUXILIARY.AUX_MU_BAUD.BAUD          = 270;
    BCM2837_AUXILIARY.AUX_MU_LCR.DLAB           = 0;

    /* map UART1 to GPOI pins
     *
     * @note
     * <> map GPIO15 to alternate function 5 (RXD1 UART1 Transmit Data)
     * <> map GPIO14 to alternate function 5 (TXD1 UART1 Receive Data)
     *    <> alternante function 5 is the only way to map the UART1
     * */
    BCM2837_GPIO.GPFSEL1.FSEL15 = 0b011;
    BCM2837_GPIO.GPFSEL1.FSEL14 = 0b011;

    /* link GPIO15 and GPIO14 pins to the UART1
     *
     * Now, we want the GPIO15 and GPIO14 to be completly controlled by the
     * UART1 module. So, we shall "disable" the current pins behaviour to
     * avoid conflict with the module.
     *
     * @note
     * <> we first need to disable pull-up/down for all GPIO pins
     * <> we MUST wait at least 150 cycles (CPPUDCLKn, page 101)
     * <> specify that only GPIO14 and GPIO15 must be modified
     * <> we MUST wait at least 150 cycles (CPPUDCLKn, page 101)
     * <> reset PUD (in our case, we already in reset mode)
     * <> reset CLK to "flush" the "new" GPIO configuration
     * */
    BCM2837_GPIO.GPPUD.PUD = 0b00;
    UTIL_SPINWAIT_CYCLES(150);
    BCM2837_GPIO.GPPUDCLK0.PIN14 = 1;
    BCM2837_GPIO.GPPUDCLK0.PIN15 = 1;
    UTIL_SPINWAIT_CYCLES(150);
    BCM2837_GPIO.GPPUDCLK0.lword = 0x00000000;
    UTIL_SPINWAIT_CYCLES(150);

    /* enable Tx and disable Rx */
    BCM2837_AUXILIARY.AUX_MU_CNTL.RE = 0;
    BCM2837_AUXILIARY.AUX_MU_CNTL.TE = 1;
}

/* bcm2837_uart1_send() : send a 8-bit information (char in our case) */
static void bcm2837_uart1_send(unsigned int c)
{
    /* wait until we can send */
    do{
        __asm__ volatile("nop");
    } while(BCM2837_AUXILIARY.AUX_MU_LSR.TDFE == 0);

    /* write the character to the buffer */
    BCM2837_AUXILIARY.AUX_MU_IO.DATA = c;
}

//---
// Public
//---

/* uart_init() : initilize UART driver */
void uart_init(void)
{
    bcm2837_uart1_init();
}

/* uart_puts() : send string through the UART line (convert new line) */
void uart_puts(char const * const s)
{
    for(int i = 0 ; s[i] != '\0' ; i++)
    {
        if(s[i] == '\n')
            bcm2837_uart1_send('\r');
        bcm2837_uart1_send(s[i]);
    }
}

/* uart_putf() : printf-like puts */
void uart_putf(char const * const format, ...)
{
    char buffer[128];
    va_list ap;

    va_start(ap, format);
    vsnprintf(buffer, 64, format, ap);
    va_end(ap);

    uart_puts(buffer);
}
