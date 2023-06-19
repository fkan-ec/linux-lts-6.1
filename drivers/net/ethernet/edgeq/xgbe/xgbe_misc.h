/* SPDX-License-Identifier: GPL-2.0 */
/*******************************************************************************
  Copyright (C) 2020  EdgeQ Inc.
*******************************************************************************/

#ifndef __XGBE_MISC_H__
#define __XGBE_MISC_H__

/* The following are three Ethernet misc registers */
/* ETHERNET_MISC_CFG register */
#define XGBE_MISC_CFG			0x0028
#define XGBE_MUX_PCS_SEL_LINK2_25G	BIT(0)
#define XGBE_MUX_MDIO_SEL_LINK1_XLGMAC	BIT(1)
#define XGBE_XLGPCS_25G_ACTIVE		BIT(4)  /* Write 1 to be active */
#define XGBE_XLGPCS_50G_ACTIVE		BIT(5)  /* Write 1 to be active */
#define XGBE_XPCS_ACTIVE		GENMASK(11, 8)  /* Write 1 to be active */
#define XGBE_XGMAC_MEM_ACTIVE		BIT(22) /* 1: out of shutdown */
#define XGBE_XLGMAC_MEM_ACTIVE		BIT(23) /* 1: out of shutdown */
/* SERDES_E56_CFG register */
#define XGBE_E56_CFG0			0x0128
#define XGBE_E56_CFG1			0x012C
#define XGBE_E56_CLK_SEL		BIT(0) /* 0: from IOSS pll0 */

#define XGBE_AUX_DAC_CFG		0x4
#define XGBE_AUX_DAC_OPM		GENMASK(2, 0)
#define XGBE_AUX_DAC_OPM_EN		0x3
#define XGBE_AUX_DAC_SELIF		BIT(3)	// 0: unsigned; 1: 2's comp
#define XGBE_AUX_DAC_VAL		GENMASK(27, 16)
#define XGBE_AUX_DAC_VAL_SHIFT		16
#define XGBE_AUX_DAC_TOGGLE		BIT(28)	// Toggle to make val effective

#define XGBE_DAC_MIN_VAL		0
#define XGBE_DAC_MID_VAL		1024
#define XGBE_DAC_MAX_VAL		2047

void xgbe_pcs_mux_config(struct xgmac_priv *priv);
void xgbe_mdio_mux_config(struct xgmac_priv *priv);
int xgbe_set_mac_mem_active(struct xgmac_priv *priv);
void xgbe_set_bss_addr(void __iomem *bss_base);
void xgbe_aux_dac_write(struct xgmac_priv *priv, u32 val);

#endif /* __XGBE_MISC_H__ */
