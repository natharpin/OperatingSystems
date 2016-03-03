/**
 * @file kprintf.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

/*TA-BOT:MAILTO nathan.arpin@marquette.edu april.song@marquette.edu */

#include <xinu.h>

#define UNGETMAX 10             /* Can un-get at most 10 characters. */

static unsigned char ungetArray[UNGETMAX];

/**
 * Synchronously read a character from a UART.  This blocks until a character is
 * available.  The interrupt handler is not used.
 *
 * @return
 *      The character read from the UART as an <code>unsigned char</code> cast
 *      to an <code>int</code>.
 */
syscall kgetc(void)
{
    volatile struct pl011_uart_csreg *regptr;
    uchar c = 0;

    /* Pointer to the UART control and status registers.  */
    regptr = (struct pl011_uart_csreg *)0x20201000;

    int ungetIsEmpty = ungetArray[0] == 0;
    if(!ungetIsEmpty) //If the unget array is not empty, look for the next character in the array
    {
        int i;
        for(i = 9; i >= 0; i--) //Stack implementation, looks at the top of the the array first
        {
           if(ungetArray[i] != 0)
            {
                c = ungetArray[i];
                break;
            }
        }
        ungetArray[i] = 0;
        return c;
    }

    int status; //If the recieving FIFOs are not empty, retrieve a character from the UART data register
    while(((status = regptr->fr) & PL011_FR_RXFE) == PL011_FR_RXFE);

    return c = regptr->dr;
}

/**
 * kcheckc - check to see if a character is available.
 * @return true if a character is available, false otherwise.
 */
syscall kcheckc(void)
{
    volatile struct pl011_uart_csreg *regptr;
    regptr = (struct pl011_uart_csreg *)0x20201000;

    int isEmpty = 1;
    if(ungetArray[0] != 0)
    {
        isEmpty = 0;
    }

    if(!isEmpty) //If the unget array is not empty, return true
    {
        return 1; //True
    }

    int status; //If the recieving FIFOs are not empty, return true
    if(((status = regptr->fr) & PL011_FR_RXFE) != PL011_FR_RXFE) 
    {
        return 1; //True
    }
    //Return false if there are no characters ready
    return 0; //False
}

/**
 * kungetc - put a serial character "back" into a local buffer.
 * @param c character to unget.
 * @return c on success, SYSERR on failure.
 */
syscall kungetc(unsigned char c)
{
    int isFull = 1;
    int i; //Checks the unget array to see if it is full
    for(i = 0; i < UNGETMAX; i++)
    {
        if(ungetArray[i] == 0) 
        {
            isFull = 0;
            break;
        }
    }

    if(!isFull) //If the unget array is not full, put the character on top of the stack
    {
        ungetArray[i] = c;
        return c;
    }
    //Otherwise, discard the character and return SYSERR or -1
    return SYSERR;
}


/**
 * Synchronously write a character to a UART.  This blocks until the character
 * has been written to the hardware.  The interrupt handler is not used.
 *
 * @param c
 *      The character to write.
 *
 * @return
 *      The character written to the UART as an <code>unsigned char</code> cast
 *      to an <code>int</code>.
 */
syscall kputc(uchar c)
{
    volatile struct pl011_uart_csreg *regptr;

    /* Pointer to the UART control and status registers.  */
    regptr = (struct pl011_uart_csreg *)0x20201000;

    int status; //Once the transmission FIFOs are not full, send out a character
    while(((status = regptr->fr) & PL011_FR_TXFF) == PL011_FR_TXFF);
    regptr->dr = c;

    return (int) c;
}

/**
 * kernel printf: formatted, synchronous output to SERIAL0.
 *
 * @param format
 *      The format string.  Not all standard format specifiers are supported by
 *      this implementation.  See _doprnt() for a description of supported
 *      conversion specifications.
 * @param ...
 *      Arguments matching those in the format string.
 *
 * @return
 *      The number of characters written.
 */
syscall kprintf(const char *format, ...)
{
    int retval;
    va_list ap;

    va_start(ap, format);
    retval = _doprnt(format, ap, (int (*)(int, int))kputc, 0);
    va_end(ap);
    return retval;
}
