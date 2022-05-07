/*
 * arch/arm/mach-vienna/suspend.h
 *
 * Copyright (C) 2018 VATICS Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

struct vatics_reserved_block_s {
	unsigned int *loader_addr;
	unsigned int *dram_training_addr;
	unsigned int *misc;
};

struct i2c_regs {
	volatile unsigned int ic_con;
	volatile unsigned int ic_tar;
	volatile unsigned int ic_sar;
	volatile unsigned int ic_hs_maddr;
	volatile unsigned int ic_cmd_data;
	volatile unsigned int ic_ss_scl_hcnt;
	volatile unsigned int ic_ss_scl_lcnt;
	volatile unsigned int ic_fs_scl_hcnt;
	volatile unsigned int ic_fs_scl_lcnt;
	volatile unsigned int ic_hs_scl_hcnt;
	volatile unsigned int ic_hs_scl_lcnt;
	volatile unsigned int ic_intr_stat;
	volatile unsigned int ic_intr_mask;
	volatile unsigned int ic_raw_intr_stat;
	volatile unsigned int ic_rx_tl;
	volatile unsigned int ic_tx_tl;
	volatile unsigned int ic_clr_intr;
	volatile unsigned int ic_clr_rx_under;
	volatile unsigned int ic_clr_rx_over;
	volatile unsigned int ic_clr_tx_over;
	volatile unsigned int ic_clr_rd_req;
	volatile unsigned int ic_clr_tx_abrt;
	volatile unsigned int ic_clr_rx_done;
	volatile unsigned int ic_clr_activity;
	volatile unsigned int ic_clr_stop_det;
	volatile unsigned int ic_clr_start_det;
	volatile unsigned int ic_clr_gen_call;
	volatile unsigned int ic_enable;
	volatile unsigned int ic_status;
	volatile unsigned int ic_txflr;
	volatile unsigned int ix_rxflr;
	volatile unsigned int ic_sda_hold;
	volatile unsigned int ic_tx_abrt_source;
};

#define reserved_block_addr		0x01000000
#define reserved_block_size		0x00021000
#define vatics_loader_ofs		0x00000000
#define vatics_dram_training_ofs	0x20000
#define	vatics_misc_ofs			0x20100
#define vatics_uboot_env_crc_idx	0x0
#define vatics_resume_addr		0x1
#define vatics_boot_storage		0x2

/* i2c status register  definitions */
#define IC_STATUS_SA		0x0040
#define IC_STATUS_MA		0x0020
#define IC_STATUS_RFF		0x0010
#define IC_STATUS_RFNE		0x0008
#define IC_STATUS_TFE		0x0004
#define IC_STATUS_TFNF		0x0002
#define IC_STATUS_ACT		0x0001

/* i2c enable register definitions */
#define IC_ENABLE_0B		0x0001

/* i2c interrupt status register definitions */
#define IC_GEN_CALL		0x0800
#define IC_START_DET		0x0400
#define IC_STOP_DET		0x0200
#define IC_ACTIVITY		0x0100
#define IC_RX_DONE		0x0080
#define IC_TX_ABRT		0x0040
#define IC_RD_REQ		0x0020
#define IC_TX_EMPTY		0x0010
#define IC_TX_OVER		0x0008
#define IC_RX_FULL		0x0004
#define IC_RX_OVER 		0x0002
#define IC_RX_UNDER		0x0001

/* i2c data buffer and command register definitions */
#define IC_CMD			0x0100
#define IC_STOP			0x0200

void __init vatics_pm_init(void);
