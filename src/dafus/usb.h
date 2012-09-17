/* ======================================================================
 * Public interface of the USB driver
 *
 * Copyright 2006-2008 Dick Streefland
 *
 * This is free software, licensed under the terms of the GNU General
 * Public License as published by the Free Software Foundation.
 * ====================================================================== */

#ifndef USB_H
#define USB_H

/* define frequent used types */
typedef	unsigned char	byte_t;
typedef	unsigned int	uint_t;

/* functions implemented in 'usb.c' */
/* Initialize the low-level USB driver */
extern void	usb_init(void);
/* Poll USB driver:
* - check for incoming USB packets
* - refill an empty transmit buffer
* - check for USB bus reset */
extern void	usb_poll(void);

/* functions implemented in 'crc.S' */
extern void crc(byte_t* data, byte_t len);

/* application callback functions */
extern byte_t usb_setup(byte_t data[8]);
extern void usb_out(byte_t* data, byte_t len);
extern byte_t usb_in(byte_t* data, byte_t len);

#endif
