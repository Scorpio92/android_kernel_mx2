/* arch/arm/plat-samsung/include/plat/uncompress.h
 *
 * Copyright 2003, 2007 Simtec Electronics
 *	http://armlinux.simtec.co.uk/
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * S3C - uncompress code
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __ASM_PLAT_UNCOMPRESS_H
#define __ASM_PLAT_UNCOMPRESS_H

typedef unsigned int upf_t;	/* cannot include linux/serial_core.h */

/* uart setup */

unsigned int fifo_mask;
unsigned int fifo_max;

/* forward declerations */

static void arch_detect_cpu(void);

/* defines for UART registers */

#include <plat/regs-serial.h>
#include <plat/regs-watchdog.h>

/* working in physical space... */
#undef S3C2410_WDOGREG
#define S3C2410_WDOGREG(x) ((S3C24XX_PA_WATCHDOG + (x)))

/* how many bytes we allow into the FIFO at a time in FIFO mode */
#define FIFO_MAX	 (14)

#define uart_base S3C_PA_UART + (S3C_UART_OFFSET * CONFIG_S3C_LOWLEVEL_UART_PORT)

static __inline__ void
uart_wr(unsigned int reg, unsigned int val)
{
	volatile unsigned int *ptr;

	ptr = (volatile unsigned int *)(reg + uart_base);
	*ptr = val;
}

static __inline__ unsigned int
uart_rd(unsigned int reg)
{
	volatile unsigned int *ptr;

	ptr = (volatile unsigned int *)(reg + uart_base);
	return *ptr;
}

/* we can deal with the case the UARTs are being run
 * in FIFO mode, so that we don't hold up our execution
 * waiting for tx to happen...
*/

static void putc(int ch)
{
	if (uart_rd(S3C2410_UFCON) & S3C2410_UFCON_FIFOMODE) {
		int level;

		while (1) {
			level = uart_rd(S3C2410_UFSTAT);
			level &= fifo_mask;

			if (level < fifo_max)
				break;
		}

	} else {
		/* not using fifos */

		while ((uart_rd(S3C2410_UTRSTAT) & S3C2410_UTRSTAT_TXE) != S3C2410_UTRSTAT_TXE)
			barrier();
	}

	/* write byte to transmission register */
	uart_wr(S3C2410_UTXH, ch);
}

static inline void flush(void)
{
}

#define __raw_writel(d, ad)			\
	do {							\
		*((volatile unsigned int __force *)(ad)) = (d); \
	} while (0)

#define __raw_readl(ad)			\
	 *((volatile unsigned int __force *)(ad))

/* CONFIG_S3C_BOOT_WATCHDOG
 *
 * Simple boot-time watchdog setup, to reboot the system if there is
 * any problem with the boot process
*/

#ifdef CONFIG_S3C_BOOT_WATCHDOG

#define WDOG_COUNT (0xff00)

static inline void arch_decomp_wdog(void)
{
	__raw_writel(WDOG_COUNT, S3C2410_WTCNT);
}

static void arch_decomp_wdog_start(void)
{
	__raw_writel(WDOG_COUNT, S3C2410_WTDAT);
	__raw_writel(WDOG_COUNT, S3C2410_WTCNT);
	__raw_writel(S3C2410_WTCON_ENABLE | S3C2410_WTCON_DIV128 | S3C2410_WTCON_RSTEN | S3C2410_WTCON_PRESCALE(0x80), S3C2410_WTCON);
}

#else
#define arch_decomp_wdog_start()
#define arch_decomp_wdog()
#endif

#ifdef CONFIG_S3C_BOOT_ERROR_RESET

static void arch_decomp_error(const char *x)
{
	putstr("\n\n");
	putstr(x);
	putstr("\n\n -- System resetting\n");

	__raw_writel(0x4000, S3C2410_WTDAT);
	__raw_writel(0x4000, S3C2410_WTCNT);
	__raw_writel(S3C2410_WTCON_ENABLE | S3C2410_WTCON_DIV128 | S3C2410_WTCON_RSTEN | S3C2410_WTCON_PRESCALE(0x40), S3C2410_WTCON);

	while(1);
}

#define arch_error arch_decomp_error
#endif

#ifdef CONFIG_S3C_BOOT_UART_FORCE_FIFO
static inline void arch_enable_uart_fifo(void)
{
	u32 fifocon = uart_rd(S3C2410_UFCON);

	if (!(fifocon & S3C2410_UFCON_FIFOMODE)) {
		fifocon |= S3C2410_UFCON_RESETBOTH;
		uart_wr(S3C2410_UFCON, fifocon);

		/* wait for fifo reset to complete */
		while (1) {
			fifocon = uart_rd(S3C2410_UFCON);
			if (!(fifocon & S3C2410_UFCON_RESETBOTH))
				break;
		}
	}
}
#else
#define arch_enable_uart_fifo() do { } while(0)
#endif
#if defined(CONFIG_MX_SERIAL_TYPE) || defined(CONFIG_MX2_SERIAL_TYPE)
#undef S5P_PA_INFORM2
#undef S5P_PA_CHIPID
#undef EXYNOS4210_CHIPID
#define S5P_PA_INFORM2			0x10020808
#define S5P_PA_CHIPID			0x10000000
#define EXYNOS4210_CHIPID		0x43210000

static int arch_is_exynos4210(void)
{
	int chipid=0;
	
	chipid = __raw_readl(S5P_PA_CHIPID) >> 17;
	
	if(chipid == (EXYNOS4210_CHIPID>>17))
		return 1;
	return 0;
}
#endif
static void
arch_decomp_setup(void)
{
	/* we may need to setup the uart(s) here if we are not running
	 * on an BAST... the BAST will have left the uarts configured
	 * after calling linux.
	 */

	arch_detect_cpu();
	arch_decomp_wdog_start();

	/* Enable the UART FIFOs if they where not enabled and our
	 * configuration says we should turn them on.
	 */

	arch_enable_uart_fifo();

#if defined(CONFIG_MX_SERIAL_TYPE) || defined(CONFIG_MX2_SERIAL_TYPE)
	if(arch_is_exynos4210())
		__raw_writel(0, S5P_PA_INFORM2);//set zore to inform2 register
#endif
}


#endif /* __ASM_PLAT_UNCOMPRESS_H */
