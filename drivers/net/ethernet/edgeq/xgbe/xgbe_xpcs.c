// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 EdgeQ, Inc.
 *
 * Synopsys DesignWare XPCS/XLGPCS/E56 APB3 API
 *
 */

#include "xgbe.h"
#include "xgbe_xpcs.h"


#define xpcs_warn(__device, __link, __args...) \
({ \
	if (__link) \
		dev_warn(__device, ##__args); \
})

static int xpcs_read_fault_10g(struct device *dev, void __iomem *ioaddr,
			       int link)
{
	u16 value;

	//value = readw(ioaddr + DW_XPCS_SR_PCS_STS1);
	//if (value & DW_RX_STS1_FAULT) {
	//	xpcs_warn(dev, link, "Link fault condition detected!\n");
	//	return -EFAULT;
	//}

	//value = readw(ioaddr + DW_XPCS_SR_PCS_STS2);
	//if (value & DW_RX_STS2_FAULT)
	//	xpcs_warn(dev, link, "Receiver fault detected!\n");
	//if (value & DW_TX_STS2_FAULT)
	//	xpcs_warn(dev, link, "Transmitter fault detected!\n");

	value = readw(ioaddr + DW_XPCS_VR_PCS_DIG_STS);
	if (value & DW_RXFIFO_ERR) {
		xpcs_warn(dev, link, "FIFO fault condition detected!\n");
		return -EFAULT;
	}

	//value = readw(ioaddr + DW_XPCS_SR_PCS_KR_STS1);
	//if (!(value & DW_RPCS_BKLK))
	//	xpcs_warn(dev, link, "Link is not locked!\n");

	//value = readw(ioaddr + DW_XPCS_SR_PCS_KR_STS2);
	//if (value & DW_ERR_BLK) {
	//	xpcs_warn(dev, link, "Link has errors!\n");
	//	return -EFAULT;
	//}

	return 0;
}

static int xpcs_read_fault_25g(struct device *dev, void __iomem *ioaddr,
			       int link)
{
	u16 value;

	//value = readw(ioaddr + DW_25G_SR_PCS_STS1);
	//if (value & DW_RX_STS1_FAULT) {
	//	xpcs_warn(dev, link, "Link fault condition detected!\n");
	//	return -EFAULT;
	//}

	//value = readw(ioaddr + DW_25G_SR_PCS_STS2);
	//if (value & DW_RX_STS2_FAULT)
	//	xpcs_warn(dev, link, "Receiver fault detected!\n");
	//if (value & DW_TX_STS2_FAULT)
	//	xpcs_warn(dev, link, "Transmitter fault detected!\n");

	value = readw(ioaddr + DW_25G_VR_PCS_DIG_STS);
	if (value & DW_RXFIFO_ERR) {
		value = readw(ioaddr + DW_25G_VR_PCS_DIG_STS);
		if (value & DW_RXFIFO_ERR) {
			xpcs_warn(dev, link, "FIFO fault condition detected!\n");
			return -EFAULT;
		}
	}

	//value = readw(ioaddr + DW_25G_SR_PCS_BASER_STS1);
	//if (!(value & DW_RPCS_BKLK))
	//	xpcs_warn(dev, link, "Link is not locked!\n");

	//value = readw(ioaddr + DW_25G_SR_PCS_BASER_STS2);
	//if (value & DW_ERR_BLK) {
	//	xpcs_warn(dev, link, "Link has errors!\n");
	//	return -EFAULT;
	//}

	return 0;
}

static int xpcs_read_fault_50g(struct device *dev, void __iomem *ioaddr,
			       int link)
{
	u16 value;

	//value = readw(ioaddr + DW_50G_SR_PCS_STS1);
	//if (value & DW_RX_STS1_FAULT) {
	//	xpcs_warn(dev, link, "Link fault condition detected!\n");
	//	return -EFAULT;
	//}

	//value = readw(ioaddr + DW_50G_SR_PCS_STS2);
	//if (value & DW_RX_STS2_FAULT)
	//	xpcs_warn(dev, link, "Receiver fault detected!\n");
	//if (value & DW_TX_STS2_FAULT)
	//	xpcs_warn(dev, link, "Transmitter fault detected!\n");

	value = readw(ioaddr + DW_50G_VR_PCS_DIG_STS);
	if (value & DW_RXFIFO_ERR) {
		xpcs_warn(dev, link, "FIFO fault condition detected!\n");
		return -EFAULT;
	}

	//value = readw(ioaddr + DW_50G_SR_PCS_BASER_STS1);
	//if (!(value & DW_RPCS_BKLK))
	//	xpcs_warn(dev, link, "Link is not locked!\n");

	//value = readw(ioaddr + DW_50G_SR_PCS_BASER_STS2);
	//if (value & DW_ERR_BLK) {
	//	xpcs_warn(dev, link, "Link has errors!\n");
	//	return -EFAULT;
	//}

	return 0;
}

static u32 xpcs_get_id(void __iomem *ioaddr_1, void __iomem *ioaddr_2)
{
	u32 xpcs_id;

	xpcs_id = (u32)readw(ioaddr_1);
	xpcs_id = (xpcs_id << 16) + readw(ioaddr_2);

	return xpcs_id;
}

static int xpcs_poll_clear(void __iomem *ioaddr, u16 mon_bit)
{
	/* Poll until the bit clears (1ms per retry == 0.5 sec) */
	u16 retries = 500;
	u16 value;
	int ret;

	do {
		msleep(1);
		value = readw(ioaddr);
	} while ((value & mon_bit) && --retries);

	ret = (value & mon_bit) ? -ETIMEDOUT : 0;

	return ret;
}

//static int xpcs_poll_set(void __iomem *ioaddr, u16 mon_bit)
//{
//	/* Poll until the bit sets (1ms per retry == 0.1 sec) */
//	u16 retries = 100;
//	u16 value;
//	int ret;
//
//	do {
//		msleep(1);
//		value = readw(ioaddr);
//	} while (!(value & mon_bit) && --retries);
//
//	ret = !(value & mon_bit) ? -ETIMEDOUT : 0;
//
//	return ret;
//}

static int xpcs_soft_reset(void __iomem *ioaddr)
{
	u16 value;

	value = readw(ioaddr);
	value |= DW_CTRL1_RST;
	writel(value, ioaddr);

	return xpcs_poll_clear(ioaddr, DW_CTRL1_RST);
}

static int xpcs_probe_2g5(void __iomem *ioaddr)
{
	u32 xpcs_id;

	xpcs_id = xpcs_get_id(ioaddr + DW_XPCS_SR_PCS_DEV_ID1,
			      ioaddr + DW_XPCS_SR_PCS_DEV_ID2);

	if (xpcs_id != DW_XPCS_ID)
		return -ENODEV;

	return 0;
}

static int xpcs_config_2g5(void __iomem *ioaddr)
{
	u16 value;
	int ret;

	/* Configured by u-boot. Do Rx reset to get link up. */
	value = readw(ioaddr + DW_XPCS_VR_PMA_12G_16G_25G_RX_GENCTRL1);
	value |= DW_RX_RST_0;
	writel(value, ioaddr + DW_XPCS_VR_PMA_12G_16G_25G_RX_GENCTRL1);
	usleep_range(100, 200);

	value = readw(ioaddr + DW_XPCS_VR_PMA_12G_16G_25G_RX_GENCTRL1);
	value &= ~DW_RX_RST_0;
	writel(value, ioaddr + DW_XPCS_VR_PMA_12G_16G_25G_RX_GENCTRL1);
	usleep_range(100, 200);

	ret = xpcs_poll_clear(ioaddr + DW_XPCS_VR_PMA_12G_16G_25G_RX_STS,
			      DW_RX_ACK_0);
	if (ret)
		return ret;

	return 0;
}

static int xpcs_get_state_2g5(struct device *dev, void __iomem *ioaddr,
			      int *link)
{
	u16 value;
	int ret, i;

	/* Check link first */
	*link = 0;
	for (i = 0; i < 1000; i++) {
		value = readw(ioaddr + DW_XPCS_SR_PCS_STS1);
		if (value & DW_RX_LINK_UP) {
			*link = 1;
			break;
		}
		udelay(1);
	}

	/* Then check the faults */
	ret = xpcs_read_fault_10g(dev, ioaddr, *link);
	if (ret) {
		ret = xpcs_soft_reset(ioaddr + DW_XPCS_SR_PCS_CTRL1);
		if (ret)
			return ret;

		*link = 0;
	}

	return 0;
}

static int xpcs_probe_10g(void __iomem *ioaddr)
{
	u32 xpcs_id;

	xpcs_id = xpcs_get_id(ioaddr + DW_XPCS_SR_PCS_DEV_ID1,
			      ioaddr + DW_XPCS_SR_PCS_DEV_ID2);

	if (xpcs_id != DW_XPCS_ID)
		return -ENODEV;

	return 0;
}

static int xpcs_config_10g(void __iomem *ioaddr)
{
	u16 value;
	int ret;

	/* Set type to 10G BASE-R */
	value = readw(ioaddr + DW_XPCS_SR_PCS_CTRL2);
	value &= ~DW_XPCS_TYPE_SEL;
	value |= DW_XPCS_TYPE_10GBASE_R;
	writel(value, ioaddr + DW_XPCS_SR_PCS_CTRL2);

	/* Enable USXGMII */
	value = readw(ioaddr + DW_XPCS_VR_PCS_DIG_CTRL1);
	value |= DW_USXG_EN;
	writel(value, ioaddr + DW_XPCS_VR_PCS_DIG_CTRL1);
	value = readw(ioaddr + DW_XPCS_VR_PCS_KR_CTRL);
	value &= ~DW_USXG_MODE;
	value |= DW_USXG_MODE_10G_SXGMII;
	writel(value, ioaddr + DW_XPCS_VR_PCS_KR_CTRL);

	/* Configure PHY for 32-bit */
	value = readw(ioaddr + DW_XPCS_VR_PMA_25G_RX_WIDTH_CTRL);
	value &= ~DW_RX0_WIDTH;
	value |= DW_RX0_WIDTH_32BIT;
	writel(value, ioaddr + DW_XPCS_VR_PMA_25G_RX_WIDTH_CTRL);
	value = readw(ioaddr + DW_XPCS_VR_PMA_25G_TX_WIDTH_CTRL);
	value &= ~DW_TX0_WIDTH;
	value |= DW_TX0_WIDTH_32BIT;
	writel(value, ioaddr + DW_XPCS_VR_PMA_25G_TX_WIDTH_CTRL);

	/* LOS from PHY can be unreliable, supress it */
	value = readw(ioaddr + DW_XPCS_VR_PCS_DEBUG_CTRL);
	value |= (DW_SUPRESS_LOS_DET | DW_RX_DT_EN_CTL);
	writel(value, ioaddr + DW_XPCS_VR_PCS_DEBUG_CTRL);

//TODO
// DW_RX_ADPT_ACK not set. This config seems not needed.
//	/* Configure receive adaption */
//	value = readw(ioaddr + DW_XPCS_VR_PMA_32G_RX_EQ_CTRL4);
//	value |= DW_RX_AD_REQ;
//	writel(value, ioaddr + DW_XPCS_VR_PMA_32G_RX_EQ_CTRL4);
//	ret = xpcs_poll_set(ioaddr + DW_XPCS_VR_PMA_12G_16G_25G_MISC_STS,
//			    DW_RX_ADPT_ACK);
//	if (ret)
//		return ret;

	value = readw(ioaddr + DW_XPCS_VR_PMA_32G_RX_EQ_CTRL4);
	value &= ~DW_RX_AD_REQ;
	writel(value, ioaddr + DW_XPCS_VR_PMA_32G_RX_EQ_CTRL4);

	/* Configure USXGMII speed */
	value = readw(ioaddr + DW_XPCS_SR_MII_CTRL);
	value &= ~DW_USXGMII_SS;
	value |= DW_USXGMII_10000 | DW_USXGMII_FULL;
	writel(value, ioaddr + DW_XPCS_SR_MII_CTRL);
	udelay(1);	// Wait for 1us for XGMII clock to stablize

	/* Soft reset */
	value = readw(ioaddr + DW_XPCS_VR_PCS_DIG_CTRL1);
	value |= DW_USRA_RST;
	writel(value, ioaddr + DW_XPCS_VR_PCS_DIG_CTRL1);

	ret = xpcs_poll_clear(ioaddr + DW_XPCS_VR_PCS_DIG_CTRL1,
			      DW_USRA_RST);
	return ret;
}

static int xpcs_get_state_10g(struct device *dev, void __iomem *ioaddr,
			      int *link)
{
	u16 value;
	int ret, i;

	/* Check link first */
	*link = 0;
	for (i = 0; i < 1000; i++) {
		value = readw(ioaddr + DW_XPCS_SR_PCS_STS1);
		if (value & DW_RX_LINK_UP) {
			*link = 1;
			break;
		}
		udelay(1);
	}

	/* Then check the faults */
	ret = xpcs_read_fault_10g(dev, ioaddr, *link);
	if (ret) {
		ret = xpcs_soft_reset(ioaddr + DW_XPCS_SR_PCS_CTRL1);
		if (ret)
			return ret;

		*link = 0;
	}

	return 0;
}

static int xpcs_probe_25g(void __iomem *ioaddr)
{
	u32 xpcs_id;

	xpcs_id = xpcs_get_id(ioaddr + DW_25G_SR_PCS_DEV_ID1,
			      ioaddr + DW_25G_SR_PCS_DEV_ID2);

	if (xpcs_id != DW_XLGPCS_25G_ID)
		return -ENODEV;

	return 0;
}

static int xpcs_config_25g(void __iomem *ioaddr)
{
	/* XLGPCS config is done in u-boot */
	return 0;
}

static int xpcs_get_state_25g(struct device *dev, void __iomem *ioaddr,
			      int *link)
{
	u16 value;
	int ret, i;

	/* Check link first */
	*link = 0;
	for (i = 0; i < 1000; i++) {
		value = readw(ioaddr + DW_25G_SR_PCS_STS1);
		if (value & DW_RX_LINK_UP) {
			*link = 1;
			break;
		}
		udelay(1);
	}

	/* Then check the faults */
	ret = xpcs_read_fault_25g(dev, ioaddr, *link);
	if (ret) {
		ret = xpcs_soft_reset(ioaddr + DW_25G_SR_PCS_CTRL1);
		if (ret)
			return ret;

		*link = 0;
	}

	return 0;
}

static int xpcs_probe_50g(void __iomem *ioaddr)
{
	u32 xpcs_id;

	xpcs_id = xpcs_get_id(ioaddr + DW_50G_SR_PCS_DEV_ID1,
			      ioaddr + DW_50G_SR_PCS_DEV_ID2);

	if (xpcs_id != DW_XLGPCS_50G_ID)
		return -ENODEV;

	return 0;
}

static int xpcs_config_50g(void __iomem *ioaddr)
{
// Configured in u-boot
#if 0
	u16 value;
	int ret;

	/* Vendor specific soft reset */
	value = readw(ioaddr + DW_50G_VR_PCS_DIG_CTRL1);
	value |= DW_VR_RST;
	writel(value, ioaddr + DW_50G_VR_PCS_DIG_CTRL1);

	ret = xpcs_poll_clear(ioaddr + DW_50G_VR_PCS_DIG_CTRL1, DW_VR_RST);
	if (ret)
		return ret;

	/* Set speed to 50G PAM4 */
	value = readw(ioaddr + DW_50G_SR_PCS_CTRL2);
	value &= ~DW_XLGPCS_TYPE_SEL;
	value |= DW_XLGPCS_TYPE_50GBASE_R;
	writel(value, ioaddr + DW_50G_SR_PCS_CTRL2);
	value = readw(ioaddr + DW_50G_SR_PMA_CTRL2);
	value &= ~DW_XLGPCS_PMA_TYPE;
	value |= DW_XLGPCS_PMA_TYPE_50GBASE_KR;
	writel(value, ioaddr + DW_50G_SR_PMA_CTRL2);
	value = readw(ioaddr + DW_50G_SR_PCS_CTRL1);
	value &= ~DW_XLGPCS_SS_5_2;
	value |= DW_XLGPCS_SS_50G;
	writel(value, ioaddr + DW_50G_SR_PCS_CTRL1);

	/* Enable RS_FEC */
	value = readw(ioaddr + DW_50G_SR_PMA_RS_FEC_CTRL);
	value &= ~DW_FLP_25G_X_2; // Select 50G_X_1
	value |= DW_RSFEC_EN;
	writel(value, ioaddr + DW_50G_SR_PMA_RS_FEC_CTRL);
#endif

	return 0;
}

static int xpcs_get_state_50g(struct device *dev, void __iomem *ioaddr,
			      int *link)
{
	u16 value;
	int ret, i;

	/* Check link first */
	*link = 0;
	for (i = 0; i < 1000; i++) {
		value = readw(ioaddr + DW_50G_SR_PCS_STS1);
		if (value & DW_RX_LINK_UP) {
			*link = 1;
			break;
		}
		udelay(1);
	}

	/* Then check the faults */
	ret = xpcs_read_fault_50g(dev, ioaddr, *link);
	if (ret) {
		ret = xpcs_soft_reset(ioaddr + DW_50G_SR_PCS_CTRL1);
		if (ret)
			return ret;

		*link = 0;
	}

	return 0;
}

static struct xpcs_ops xpcs_ops_2g5 = {
	.config = xpcs_config_2g5,
	.get_state = xpcs_get_state_2g5,
	.probe = xpcs_probe_2g5,
};

static struct xpcs_ops xpcs_ops_10g = {
	.config = xpcs_config_10g,
	.get_state = xpcs_get_state_10g,
	.probe = xpcs_probe_10g,
};

static struct xpcs_ops xpcs_ops_25g = {
	.config = xpcs_config_25g,
	.get_state = xpcs_get_state_25g,
	.probe = xpcs_probe_25g,
};

static struct xpcs_ops xpcs_ops_50g = {
	.config = xpcs_config_50g,
	.get_state = xpcs_get_state_50g,
	.probe = xpcs_probe_50g,
};

static struct xpcs_ops xpcs_ops_null = {
	.config = NULL,
	.get_state = NULL,
	.probe = NULL,
};

struct xpcs_ops *xpcs_get_ops(struct xgmac_priv *priv)
{
	struct xpcs_ops *ops;

	switch (priv->plat->eth_link) {
	case 1:
		ops = &xpcs_ops_50g;
		break;
	case 2:
		if (priv->plat->phy_lane != 0) {
			/* B0 uses XLGMII interface */
			if (priv->plat->interface == PHY_INTERFACE_MODE_XLGMII)
				ops = &xpcs_ops_25g;
			else /* A0 uses USXGMII interface */
				ops = &xpcs_ops_10g;
		}
		else {
			if (priv->plat->mac_port_sel_speed == SPEED_25000)
				ops = &xpcs_ops_25g;
			else if (priv->plat->mac_port_sel_speed == SPEED_2500)
				ops = &xpcs_ops_2g5;
			else
				ops = &xpcs_ops_10g;
		}
		break;
	default:
		ops = &xpcs_ops_null;
		break;
	}

	return ops;
}

int xgbe_e56_serdes_probe(struct xgmac_priv *priv)
{
	void __iomem *e56_addr = priv->e56_base;
	u32 value;
	int ret;

	value = readl(e56_addr + DW_E56_PMD_CFG0);
	value |= DW_E56_PMD_SOFT_RESET;
	writel(value, e56_addr + DW_E56_PMD_CFG0);
	ret = xpcs_poll_clear(e56_addr + DW_E56_PMD_CFG0,
			      DW_E56_PMD_SOFT_RESET);
	if (ret)
		return ret;

	value = readl(e56_addr + DW_E56_PMD_CFG0);
	if ((value & DW_E56_PMD_CFG0_MASK) != DW_E56_PMD_CFG0_DEFAULT)
		return -ENODEV;
	else
		return 0;
}

int xgbe_e56_serdes_config(struct xgmac_priv *priv)
{
	void __iomem *e56_addr = priv->e56_base;
	u32 value;

	value = readl(e56_addr + DW_E56_PMD_CFG0);
	value &= ~DW_E56_PMD_MODE;	// 0 for ETHML mode
	value &= ~(DW_E56_TX_EN_CFG | DW_E56_RX_EN_CFG);
	value |= (DW_E56_TX_EN_LANE0 | DW_E56_RX_EN_LANE0);
	writel(value, e56_addr + DW_E56_PMD_CFG0);

	value = readl(e56_addr + DW_E56_PLL0_CFG0);
	value |= (DW_E56_CLK_TX_TL_EN | DW_E56_CLK_TX_TR_EN);
	writel(value, e56_addr + DW_E56_PLL0_CFG0);

	value = readl(e56_addr + DW_E56_PLL0_CFG2);
	value &= ~DW_E56_PLL0_POSTDIV;
	value |= DW_E56_PLL0_POSTDIV_4;
	writel(value, e56_addr + DW_E56_PLL0_CFG2);

	value = readl(e56_addr + DW_E56_PLL0_DIV_CFG0);
	value &= ~(DW_E56_PREDIV | DW_E56_FBDIV_INT);
	value |= (DW_E56_PREDIV_8 | DW_E56_FBDIV_INT_680);
	writel(value, e56_addr + DW_E56_PLL0_DIV_CFG0);

	value = readl(e56_addr + DW_E56_PLL1_CFG2);
	value &= ~DW_E56_PLL1_POSTDIV;
	value |= DW_E56_PLL1_POSTDIV_8;
	writel(value, e56_addr + DW_E56_PLL1_CFG2);

	value = readl(e56_addr + DW_E56_PIN_OVRDEN0);
	value |= DW_E56_OVRD_EN_PLL0_TX;
	writel(value, e56_addr + DW_E56_PIN_OVRDEN0);

	value = readl(e56_addr + DW_E56_PIN_OVRDVAL0);
	value |= DW_E56_INT_PLL0_TX;
	writel(value, e56_addr + DW_E56_PIN_OVRDVAL0);

	value = readl(e56_addr + DW_E56_ANA_OVRDEN0);
	value |= DW_E56_OVRD_EN_ANA_REFCLK;
	writel(value, e56_addr + DW_E56_ANA_OVRDEN0);

	value = readl(e56_addr + DW_E56_ANA_OVRDVAL0);
	value |= DW_E56_ANA_REFCLK_EN;
	writel(value, e56_addr + DW_E56_ANA_OVRDVAL0);

	value = readl(e56_addr + DW_E56_PMD_CFG0);
	value &= ~DW_R56_PAM4_PRECODE_NO_KRT_EN;
	value |= DW_R56_PAM4_PRECODE_PAM4_ONLY;
	writel(value, e56_addr + DW_E56_PMD_CFG0);

	value = readl(e56_addr + DW_E56_DATAPATH_CFG0);
	value &= ~(DW_E56_PLL0_TX_SYMCLK_PRD_ML | DW_E56_PLL1_TX_SYMCLK_PRD_ML |
		   DW_E56_PLL_TX_SYMCLK_PRD_SL);
	value |= (DW_E56_PLL0_TX_SYMCLK_PRD_80 | DW_E56_PLL1_TX_SYMCLK_PRD_8 |
		  DW_E56_PLL_TX_SYMCLK_PRD_80);
	writel(value, e56_addr + DW_E56_DATAPATH_CFG0);

	value = readl(e56_addr + DW_E56_DATAPATH_CFG1);
	value &= ~(DW_E56_TX_SYMDATA_WIDTH_3 | DW_E56_RX_SYMDATA_WIDTH_3);
	value |= (DW_E56_TX_SYMDATA_WIDTH_80UI | DW_E56_RX_SYMDATA_WIDTH_80UI);
	writel(value, e56_addr + DW_E56_DATAPATH_CFG1);

	value = readl(e56_addr + DW_E56_AN_CFG1);
	value &= ~DW_E56_AN0_RATE_SEL_REG;
	value |= DW_E56_AN0_RATE_SEL_50G_KR;
	writel(value, e56_addr + DW_E56_AN_CFG1);

	value = DW_E56_SPARE5_VAL;
	writel(value, e56_addr + DW_E56_SPARE5);

	value = DW_E56_SPARE52;
	writel(value, e56_addr + DW_E56_SPARE52_VAL);

	value = readl(e56_addr + DW_E56_PMD_CFG0);
	value |= DW_E56_PMD_EN;
	writel(value, e56_addr + DW_E56_PMD_CFG0);

	return 0;
}

