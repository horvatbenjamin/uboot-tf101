/*
 * Copyright (c) 2011 The Chromium OS Authors.
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


/*
 * This file contains convenience functions for decoding useful and
 * enlightening information from FDTs. It is intended to be used by device
 * drivers and board-specific code within U-Boot. It aims to reduce the
 * amount of FDT munging required within U-Boot itself, so that driver code
 * changes to support FDT are minimized.
 */

/* A typedef for a physical address. We should move it to a generic place */
#ifdef CONFIG_PHYS_64BIT
typedef u64 addr_t;
#define ADDR_T_NONE (-1ULL)
#define addr_to_cpu(reg) be64_to_cpu(reg)
#else
typedef u32 addr_t;
#define ADDR_T_NONE (-1U)
#define addr_to_cpu(reg) be32_to_cpu(reg)
#endif

/* Information obtained about a UART from the FDT */
struct fdt_uart {
	addr_t reg;	/* address of registers in physical memory */
	int id;		/* id or port number (numbered from 0, default -1) */
	int reg_shift;	/* each register is (1 << reg_shift) apart */
	int baudrate;	/* baud rate, will be gd->baudrate if not defined */
	int clock_freq;	/* clock frequency, -1 if not provided */
	int multiplier;	/* divisor multiplier, default 16 */
	int divisor;	/* baud rate divisor, default calculated */
	int enabled;	/* 1 to enable, 0 to disable */
	int interrupt;	/* interrupt line */
	int silent;	/* 1 for silent UART (supresses output by default) */
};

/**
 * Return information from the FDT about the console UART. This looks for
 * an alias node called 'console' which must point to a UART. It then reads
 * out the following attributes:
 *
 *	reg
 *	port
 *	clock-frequency
 *	reg-shift (default 2)
 *	baudrate (default 115200)
 *	multiplier (default 16)
 *	divisor (calculated by default)
 *	enabled (default 1)
 *	interrupts (returns first in interrupt list, -1 if none)
 *	silent (default 0)
 *
 * The divisor calculation is performed by calling
 * fdt_decode_uart_calc_divisor() automatically once the information is
 * available. Therefore callers should be able to simply use the ->divisor
 * field as their baud rate divisor.
 *
 * @param blob	FDT blob to use
 * @param uart	struct to use to return information
 * @param default_baudrate  Baud rate to use if none defined in FDT
 * @return 0 on success, -ve on error, in which case uart is unchanged
 */
int fdt_decode_uart_console(const void *blob, struct fdt_uart *uart,
		int default_baudrate);

/**
 * Calculate the divisor to use for the uart. This is done based on the
 * formula (uart clock / (baudrate * multiplier). The divisor is updated
 * in the uart structure ready for use.
 *
 * @param uart	uart structure to examine and update
 */
void fdt_decode_uart_calc_divisor(struct fdt_uart *uart);