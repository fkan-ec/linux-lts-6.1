// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 EdgeQ, Inc.
 */

#include "xgbe.h"
#include "xgbe_misc.h"

static void __iomem *bss_addr = NULL;

void xgbe_pcs_mux_config(struct xgmac_priv *priv)
{
	void __iomem *misc_addr = priv->misc_base;
	int speed = priv->plat->mac_port_sel_speed;
	u32 misc_cfg;

	if (priv->plat->phy_lane != 0)
		return;

	misc_cfg = readl(misc_addr + XGBE_MISC_CFG);
	if (speed == SPEED_25000 && priv->plat->eth_link == 2)
		misc_cfg |= XGBE_MUX_PCS_SEL_LINK2_25G;
	else
		misc_cfg &= ~XGBE_MUX_PCS_SEL_LINK2_25G;
	writel(misc_cfg, misc_addr + XGBE_MISC_CFG);
}

void xgbe_mdio_mux_config(struct xgmac_priv *priv)
{
	void __iomem *misc_addr = priv->misc_base;
	u32 misc_cfg;

	if (priv->plat->phy_lane != 0)
		return;

	misc_cfg = readl(misc_addr + XGBE_MISC_CFG);
	if (priv->plat->eth_link == 1)
		misc_cfg |= XGBE_MUX_MDIO_SEL_LINK1_XLGMAC;
	else
		misc_cfg &= ~XGBE_MUX_MDIO_SEL_LINK1_XLGMAC;
	writel(misc_cfg, misc_addr + XGBE_MISC_CFG);
}

int xgbe_set_mac_mem_active(struct xgmac_priv *priv)
{
	void __iomem *misc_addr = priv->misc_base;
	int speed = priv->plat->mac_port_sel_speed;
	u32 misc_cfg;

	if (misc_addr == NULL)
		return 0;

	misc_cfg = readl(misc_addr + XGBE_MISC_CFG);
	if (priv->plat->eth_link == 1) {
		misc_cfg |= XGBE_XLGMAC_MEM_ACTIVE;
		misc_cfg |= XGBE_XLGPCS_50G_ACTIVE;
	}
	else {
		if (speed == SPEED_25000) {
			misc_cfg |= XGBE_XLGMAC_MEM_ACTIVE;
			misc_cfg |= XGBE_XLGPCS_25G_ACTIVE;
		}
		else {
			misc_cfg |= XGBE_XGMAC_MEM_ACTIVE;
			misc_cfg |= XGBE_XPCS_ACTIVE;
		}
	}
	writel(misc_cfg, misc_addr + XGBE_MISC_CFG);

	return 0;
}

void xgbe_set_bss_addr(void __iomem *bss_base)
{
	bss_addr = bss_base;
}

void xgbe_aux_dac_write(struct xgmac_priv *priv, u32 val)
{
	u32 reg;

	if (bss_addr == NULL)
		return;

	reg = readl(bss_addr + XGBE_AUX_DAC_CFG);
	reg &= XGBE_AUX_DAC_TOGGLE;	/* Keel toggle bit unchanged */
	reg |= XGBE_AUX_DAC_OPM_EN;
	reg |= (val << XGBE_AUX_DAC_VAL_SHIFT) & XGBE_AUX_DAC_VAL;
	writel(reg, bss_addr + XGBE_AUX_DAC_CFG);
	reg ^= XGBE_AUX_DAC_TOGGLE;	/* Toggle the bit */
	writel(reg, bss_addr + XGBE_AUX_DAC_CFG);
}
