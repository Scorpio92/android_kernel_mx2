/*
 * arch/arm/mach/include/mx2_cs8803g_spi.h
 *
 * Copyright (C) 2012 Meizu Technology Co.Ltd, Zhuhai, China
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __M041_MODEM_H
#define __M041_MODEM_H __FILE__

#define  SPT_MODEM_CONNECT_FLAG     0x0001
#define  SPT_MODEM_RESET_FLAG       0x0002
#define  SPT_MODEM_CRASH_FLAG       0x0004
#define  SPT_MODEM_DUMP_FLAG        0x0008
#define  SPT_MODEM_DISCONNECT_FLAG  0x0010
#define  SPT_MODEM_SIM_DETECT_FLAG  0x0020
#define  SPT_MODEM_INIT_ON_FLAG     0x0040

#define  SPT_MODEM_EVENT_MASK       0x007E

enum SPT_MODEM_EVENT_TYPE {
	SPT_MODEM_EVENT_POWEROFF,
	SPT_MODEM_EVENT_RESET,
	SPT_MODEM_EVENT_CRASH,
	SPT_MODEM_EVENT_DUMP,
	SPT_MODEM_EVENT_CONN,
	SPT_MODEM_EVENT_DISCONN,
	SPT_MODEM_EVENT_SIM,
	SPT_MODEM_EVENT_BOOT_INIT,
};

#define SPT_MODEM_SC8803G	1

struct spt_modem_platform_data {
	unsigned short pwr_on;		/* power on */

	unsigned short srdy;		/* MDM_RDY */
	unsigned short mrdy;		/* AP_RDY */
	unsigned short srts;		/* MDM_RTS */
	unsigned short mrts;		/* AP_RTS */
	unsigned short srsd;		/* MDM_RESEND: modem resend request */
	unsigned short mrsd;		/* AP_RESEND: ap resend reguest */
	unsigned short salive;		/* MDM_ALIVE: modem alive signal */

	unsigned short s2m1;		/* MDM_TO_AP1 */
	unsigned short s2m2;		/* MDM_TO_AP2 */
	unsigned short m2s1;		/* AP_TO_MODEM1 */
	unsigned short m2s2;		/* AP_TO_MODEM2 */
	
	unsigned char modem_type;	/* Modem type */
	unsigned char bit_per_word;	/* spi bit word length*/
	unsigned	char	mode;			/* spi transfer mode*/
	unsigned long max_hz;		/* max SPI frequency */
	unsigned short use_dma:1;	/* spi protocol driver supplies
					   dma-able addrs */
};

extern int register_spt_modem_crash_notifier(struct notifier_block *nb);
extern int unregister_spt_modem_crash_notifier(struct notifier_block *nb);
extern void spt_modem_notify_event(int type);
#endif

