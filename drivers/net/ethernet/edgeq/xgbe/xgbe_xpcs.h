/* SPDX-License-Identifier: GPL-2.0 */
/*******************************************************************************
  Copyright (C) 2020  EdgeQ Inc.
*******************************************************************************/

#ifndef __XGBE_XPCS_H__
#define __XGBE_XPCS_H__

/* ID of XPCS/XLGPCS are same, but the address to read ID are different. */
#define DW_XPCS_ID			0x7996ced0
#define DW_XLGPCS_25G_ID		0x7996ced0
#define DW_XLGPCS_50G_ID		0x7996ced0

/* XPCS registers offset. Memory remapped. */
/* PMA */
#define DW_XPCS_VR_PMA_25G_TX_WIDTH_CTRL	0x0140
#define DW_TX0_WIDTH			GENMASK(2, 0)
#define DW_TX0_WIDTH_32BIT		0x4
#define DW_XPCS_VR_PMA_12G_16G_25G_RX_GENCTRL1	0x0150
#define DW_RX_RST_0			BIT(4)
#define DW_XPCS_VR_PMA_32G_RX_EQ_CTRL4	0x0180
#define DW_RX_AD_REQ			BIT(12)
#define DW_XPCS_VR_PMA_12G_16G_25G_RX_STS	0x0190
#define DW_RX_ACK_0			BIT(0)
#define DW_XPCS_VR_PMA_12G_16G_25G_MISC_STS	0x01F0
#define DW_RX_ADPT_ACK			BIT(12)
#define DW_XPCS_VR_PMA_25G_RX_WIDTH_CTRL	0x0238
#define DW_RX0_WIDTH			GENMASK(2, 0)
#define DW_RX0_WIDTH_32BIT		0x4
/* PCS */
#define DW_XPCS_SR_PCS_CTRL1		0x0248
#define DW_CTRL1_RST			BIT(15)
#define DW_XPCS_SR_PCS_STS1		0x0250
#define DW_RX_LINK_UP			BIT(2)
#define DW_RX_STS1_FAULT		BIT(7)
#define DW_XPCS_SR_PCS_DEV_ID1		0x0258
#define DW_XPCS_SR_PCS_DEV_ID2		0x0260
#define DW_XPCS_SR_PCS_CTRL2		0x0280
#define DW_XPCS_TYPE_SEL		GENMASK(3, 0)
#define DW_XPCS_TYPE_10GBASE_R		0x0
#define DW_XPCS_SR_PCS_STS2		0x0288
#define DW_RX_STS2_FAULT		BIT(10)
#define DW_TX_STS2_FAULT		BIT(11)
#define DW_XPCS_SR_PCS_KR_STS1		0x02B8
#define DW_RPCS_BKLK			BIT(0)
#define DW_XPCS_SR_PCS_KR_STS2		0x02C0
#define DW_ERR_BLK			GENMASK(7, 0)
#define DW_XPCS_VR_PCS_DIG_CTRL1	0x0360
#define DW_USXG_EN			BIT(9)
#define DW_USRA_RST			BIT(10)
#define DW_VR_RST			BIT(15)
#define DW_XPCS_VR_PCS_DEBUG_CTRL	0x0380
#define DW_SUPRESS_LOS_DET		BIT(4)
#define DW_RX_DT_EN_CTL			BIT(6)
#define DW_XPCS_VR_PCS_KR_CTRL		0x0388
#define DW_USXG_MODE			GENMASK(12, 10);
#define DW_USXG_MODE_10G_SXGMII		(0x0 << 10)
#define DW_XPCS_VR_PCS_DIG_STS		0x0390
#define DW_RXFIFO_ERR			GENMASK(6, 5)
#define DW_XPCS_SR_MII_CTRL		0x0410
#define DW_USXGMII_FULL			BIT(8)
#define DW_USXGMII_SS			(BIT(13) | BIT(6) | BIT(5))
#define DW_USXGMII_10000		(BIT(13) | BIT(6))

/* XLGPCS 25G registers offset. Memory remapped. */
/* PCS */
#define DW_25G_SR_PCS_CTRL1		0x0000
#define DW_XLGPCS_SS_5_2		GENMASK(5, 2)
#define DW_XLGPCS_SS_25G		(0x5 << 2)
#define DW_XLGPCS_SS_50G		(0x6 << 2)
#define DW_25G_SR_PCS_STS1		0x0008
#define DW_25G_SR_PCS_DEV_ID1		0x0010
#define DW_25G_SR_PCS_DEV_ID2		0x0018
#define DW_25G_SR_PCS_CTRL2		0x0038
#define DW_XLGPCS_TYPE_SEL		GENMASK(3, 0)
#define DW_XLGPCS_TYPE_25GBASE_R	0x7
#define DW_XLGPCS_TYPE_50GBASE_R	0x8
#define DW_25G_SR_PCS_STS2		0x0040
#define DW_25G_SR_PCS_BASER_STS1	0x0058
#define DW_25G_SR_PCS_BASER_STS2	0x0060
#define DW_25G_VR_PCS_DIG_CTRL1		0x0110
#define DW_25G_VR_PCS_DIG_CTRL3		0x0120
#define DW_CNS_EN			BIT(0)
#define DW_CNS_50G			BIT(1)
#define DW_25G_VR_PCS_DIG_STS		0x0138
#define DW_25G_VR_PCS_AM_CNT		0x0140
#define DW_VR_PCS_AM_CNT		GENMASK(13, 0)
#define DW_VR_PCS_AM_CNT_25G_50G	1024
/* PMA */
#define DW_25G_SR_PMA_CTRL2		0x0180
#define DW_XLGPCS_PMA_TYPE		GENMASK(6, 0)
#define DW_XLGPCS_PMA_TYPE_25GBASE_KR	0x39
#define DW_XLGPCS_PMA_TYPE_50GBASE_KR	0x40
#define DW_25G_SR_PMA_KR_FEC_CTRL	0x01D0
#define DW_FEC_EN			BIT(0)
#define DW_25G_SR_PMA_RS_FEC_CTRL	0x01F8
#define DW_RSFEC_EN			BIT(2)
#define DW_FLP_25G_X_2			BIT(3)	// 0 for 50G_X_1
#define DW_25G_VR_PMA_25G_16G_RX_EQ_CTRL4	0x02F8
#define DW_RX0_AD_REQ			BIT(12)
#define DW_25G_VR_PMA_32G_MISC_STS0	0x0380
#define DW_RX0_AD_ACK			BIT(4)
/* PMA */

/* XLGPCS 50G registers offset. Memory remapped. */
/* PCS */
#define DW_50G_SR_PCS_CTRL1		0x01C0
#define DW_50G_SR_PCS_STS1		0x01C8
#define DW_50G_SR_PCS_DEV_ID1		0x01D0
#define DW_50G_SR_PCS_DEV_ID2		0x01D8
#define DW_50G_SR_PCS_CTRL2		0x01F8
#define DW_50G_SR_PCS_STS2		0x0200
#define DW_50G_SR_PCS_BASER_STS1	0x0218
#define DW_50G_SR_PCS_BASER_STS2	0x0220
#define DW_50G_VR_PCS_DIG_CTRL1		0x0420
#define DW_50G_VR_PCS_DIG_STS		0x0448
//#define DW_50G_VR_PCS_AM_CNT		0x0458
/* PMA */
#define DW_50G_SR_PMA_CTRL2		0x0038
#define DW_50G_SR_PMA_RS_FEC_CTRL	0x0080

struct xpcs_ops {
	int (*probe)(void __iomem *ioaddr);
	int (*config)(void __iomem *ioaddr);
	int (*get_state)(struct device *dev, void __iomem *ioaddr, int *link);
};

struct xpcs_ops *xpcs_get_ops(struct xgmac_priv *priv);

/* E32 indirect programming register offset. */
/* For XPCS */
#define DW_XPCS_VR_PMA_12G_16G_25G_SRAM	0x0200
#define DW_INIT_DN			BIT(0)
#define DW_EXT_LD_DN			BIT(1)
#define DW_XPCS_VR_PMA_SNPS_CR_CTRL	0x0220
#define DW_START_BUSY			BIT(0)
#define DW_WR_RDN			BIT(1)
#define DW_XPCS_VR_PMA_SNPS_CR_ADDR	0x0228
#define DW_XPCS_VR_PMA_SNPS_CR_DATA	0x0230
/* For 25G XLGPCS */
#define DW_25G_VR_PMA_25G_16G_SRAM	0x0368
#define DW_INIT_DN			BIT(0)
#define DW_EXT_LD_DN			BIT(1)
#define DW_25G_VR_PMA_25G_16G_CR_CTRL	0x0390
#define DW_START_BUSY			BIT(0)
#define DW_WR_RDN			BIT(1)
#define DW_25G_VR_PMA_25G_16G_CR_ADDR	0x0398
#define DW_25G_VR_PMA_25G_16G_CR_DATA	0x03A0

/* E56 serdes register offset. */
#define DW_E56_PLL0_CFG0		0x0C10
#define DW_E56_CLK_TX_TL_EN		BIT(16)
#define DW_E56_CLK_TX_TR_EN		BIT(17)
#define DW_E56_PLL0_CFG2		0x0C18
#define DW_E56_PLL0_POSTDIV		GENMASK(12, 8)
#define DW_E56_PLL0_POSTDIV_4		(0x4 << 8)
#define DW_E56_PLL0_DIV_CFG0		0x0C1C
#define DW_E56_PREDIV			GENMASK(4, 0)
#define DW_E56_PREDIV_8			0x8
#define DW_E56_FBDIV_INT		GENMASK(18, 8)
#define DW_E56_FBDIV_INT_680		(0x2A8 << 8)
#define DW_E56_PLL1_CFG2		0x0C50
#define DW_E56_PLL1_POSTDIV		GENMASK(12, 8)
#define DW_E56_PLL1_POSTDIV_8		(0x8 << 8)
#define DW_E56_PIN_OVRDEN0		0x0C8C
#define DW_E56_OVRD_EN_PLL0_TX		BIT(12)
#define DW_E56_PIN_OVRDVAL0		0x0C94
#define DW_E56_INT_PLL0_TX		BIT(10)
#define DW_E56_ANA_OVRDEN0		0x0CA4
#define DW_E56_OVRD_EN_ANA_REFCLK	BIT(0)
#define DW_E56_ANA_OVRDVAL0		0x0CB0
#define DW_E56_ANA_REFCLK_EN		BIT(0)
#define DW_E56_PMD_CFG0			0x1400
#define DW_E56_PMD_CFG0_MASK		0x0F3FF133
#define DW_E56_PMD_CFG0_DEFAULT		0x0F3FF100
#define DW_E56_PMD_SOFT_RESET		BIT(0)
#define DW_E56_PMD_EN			BIT(1)
#define DW_E56_PMD_MODE			BIT(8)	// 0 for ETHML
#define DW_E56_TX_EN_CFG		GENMASK(15, 12)
#define DW_E56_TX_EN_LANE0		(0x1 << 12)
#define DW_E56_RX_EN_CFG		GENMASK(19, 16)
#define DW_E56_RX_EN_LANE0		(0x1 << 16)
#define DW_R56_PAM4_PRECODE_NO_KRT_EN	GENMASK(27, 24)
#define DW_R56_PAM4_PRECODE_PAM4_ONLY	(0x0 << 24)
#define DW_E56_DATAPATH_CFG0		0x142C
#define DW_E56_PLL0_TX_SYMCLK_PRD_ML	GENMASK(10, 8)
#define DW_E56_PLL0_TX_SYMCLK_PRD_80	(0x7 << 8)
#define DW_E56_PLL1_TX_SYMCLK_PRD_ML	GENMASK(14, 12)
#define DW_E56_PLL1_TX_SYMCLK_PRD_8	(0x0 << 12)
#define DW_E56_PLL_TX_SYMCLK_PRD_SL	GENMASK(18, 16)
#define DW_E56_PLL_TX_SYMCLK_PRD_80	(0x7 << 16)
#define DW_E56_DATAPATH_CFG1		0x1430
#define DW_E56_TX_SYMDATA_WIDTH_3	GENMASK(14, 12)
#define DW_E56_TX_SYMDATA_WIDTH_80UI	(0x7 << 12)
#define DW_E56_RX_SYMDATA_WIDTH_3	GENMASK(30, 28)
#define DW_E56_RX_SYMDATA_WIDTH_80UI	(0x7 << 28)
#define DW_E56_AN_CFG1			0x1438
#define DW_E56_AN0_RATE_SEL_REG		GENMASK(4, 0)
#define DW_E56_AN0_RATE_SEL_50G_KR	0xD
#define DW_E56_SPARE5			0x1608
#define DW_E56_SPARE5_VAL		0x3F
#define DW_E56_SPARE52			0x16FC
#define DW_E56_SPARE52_VAL		0xF0	// For adjust rx_div66 clock


int xgbe_e56_serdes_probe(struct xgmac_priv *priv);
int xgbe_e56_serdes_config(struct xgmac_priv *priv);

#endif /* __XGBE_XPCS_H__ */
