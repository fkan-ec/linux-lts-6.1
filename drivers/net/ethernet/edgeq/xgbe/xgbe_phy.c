// SPDX-License-Identifier: GPL-2.0-only
/*******************************************************************************
  Provides interface to the external Marvell 88x7120 PHY

  Copyright (c) 2021  EdgeQ Inc
*******************************************************************************/

#include <linux/gpio/consumer.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/mii.h>
#include <linux/of_mdio.h>
#include <linux/phy.h>
#include <linux/property.h>
#include <linux/slab.h>

#ifdef CONFIG_MV88X7120_PHY
#include "../../../phy/mv88x7120/mydApiTypes.h"
#include "../../../phy/mv88x7120/mydAPI.h"
#include "../../../phy/mv88x7120/mydHwCntl.h"
#include "../../../phy/mv88x7120/mydUtils.h"
#include "../../../phy/mv88x7120/mydApiRegs.h"
#include "../../../phy/mv88x7120/mydIntrIOConfig.h"
#include "../../../phy/mv88x7120/mydFwDownload.h"
#include "../../../phy/mv88x7120/mydInitialization.h"
#endif

#include "dwxgmac.h"
#include "xgbe.h"
#include "xgbe_phy.h"


#define PHY_AN_10GB_KX4			0x0004
#define PHY_AN_10GB_KR			0x0008
#define PHY_AN_40GB_KR4			0x0020
#define PHY_AN_40GB_CR4			0x0040
#define PHY_AN_25GB_CR_S		0x1000
#define PHY_AN_25GB_CR			0x2000
#define PHY_AN_50GB_KR4_CR4		0x10000


static const int phy_sgmii_2g5_supported[] = {
	ETHTOOL_LINK_MODE_Pause_BIT,
	ETHTOOL_LINK_MODE_Asym_Pause_BIT,
	ETHTOOL_LINK_MODE_Autoneg_BIT,
	ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
	__ETHTOOL_LINK_MODE_MASK_NBITS,
};

static const int phy_10g_supported[] = {
        ETHTOOL_LINK_MODE_Pause_BIT,
        ETHTOOL_LINK_MODE_Asym_Pause_BIT,
        ETHTOOL_LINK_MODE_Autoneg_BIT,
        ETHTOOL_LINK_MODE_10000baseKR_Full_BIT,
	ETHTOOL_LINK_MODE_FEC_RS_BIT,
        __ETHTOOL_LINK_MODE_MASK_NBITS,
};

static const int phy_25g_supported[] = {
        ETHTOOL_LINK_MODE_Pause_BIT,
        ETHTOOL_LINK_MODE_Asym_Pause_BIT,
        ETHTOOL_LINK_MODE_Autoneg_BIT,
        ETHTOOL_LINK_MODE_25000baseCR_Full_BIT,
        ETHTOOL_LINK_MODE_25000baseKR_Full_BIT,
	ETHTOOL_LINK_MODE_FEC_RS_BIT,
        __ETHTOOL_LINK_MODE_MASK_NBITS,
};

static const int phy_50g_supported[] = {
        ETHTOOL_LINK_MODE_Pause_BIT,
        ETHTOOL_LINK_MODE_Asym_Pause_BIT,
        ETHTOOL_LINK_MODE_Autoneg_BIT,
        ETHTOOL_LINK_MODE_50000baseKR_Full_BIT,
        ETHTOOL_LINK_MODE_50000baseCR_Full_BIT,
	ETHTOOL_LINK_MODE_FEC_RS_BIT,
        __ETHTOOL_LINK_MODE_MASK_NBITS,
};

#ifdef CONFIG_MV88X7120_PHY
static MYD_DEV_PTR g_pdev = NULL;

static void xgbe_xgmac_set_cl45(struct xgmac_priv *priv)
{
	/* Set port for MDIO Clause 45 */
	writel(0, priv->ioaddr + XGMAC_MDIO_C22P);
}
#endif

static int xgbe_xgmac_mdio_read(struct xgmac_priv *priv, u16 mdio_port,
				u16 mmd, u16 reg)
{
	unsigned int mii_address = priv->hw->mii.addr;
	unsigned int mii_data = priv->hw->mii.data;
	u32 tmp, addr;
	u32 value = XGMAC_BUSY;

	addr = ((mmd << XGMAC_DA_SHIFT) & XGMAC_DA_MASK) |
	       ((mdio_port << XGMAC_PA_SHIFT) & XGMAC_PA_MASK) | reg;

	value |= (priv->clk_csr << priv->hw->mii.clk_csr_shift)
		 & priv->hw->mii.clk_csr_mask;
	value |= ((XGMAC_CMD_READ << XGMAC_CMD_SHIFT) & XGMAC_CMD_MASK);

	/* Wait until any existing MII operation is complete */
	if (readl_poll_timeout(priv->ioaddr + mii_data, tmp,
			       !(tmp & XGMAC_BUSY), 100, 10000))
		return -EBUSY;

	/* Set the MII address register to read */
	writel(addr, priv->ioaddr + mii_address);
	writel(value, priv->ioaddr + mii_data);

	/* Wait until any existing MII operation is complete */
	if (readl_poll_timeout(priv->ioaddr + mii_data, tmp,
			       !(tmp & XGMAC_BUSY), 100, 10000))
		return -EBUSY;

	/* Read the data from the MII data register */
	return readl(priv->ioaddr + mii_data) & GENMASK(15, 0);
}

#if 0
static MYD_STATUS xgbe_xgmac_mdio_read(MYD_DEV_PTR pdev, u16 mdio_port,
				       u16 mmd, u16 reg, u16 *data)
{
	struct xgmac_priv *priv = (struct xgmac_priv *)pdev->hostContext;
	unsigned int mii_address = priv->hw->mii.addr;
	unsigned int mii_data = priv->hw->mii.data;
	u32 tmp, addr;
        u32 value = XGMAC_BUSY;

	addr = ((mmd << XGMAC_DA_SHIFT) & XGMAC_DA_MASK) |
	       ((mdio_port << XGMAC_PA_SHIFT) & XGMAC_PA_MASK) | reg;

	value |= (priv->clk_csr << priv->hw->mii.clk_csr_shift)
		 & priv->hw->mii.clk_csr_mask;
	value |= ((XGMAC_CMD_READ << XGMAC_CMD_SHIFT) & XGMAC_CMD_MASK);

	/* Wait until any existing MII operation is complete */
	if (readl_poll_timeout(priv->ioaddr + mii_data, tmp,
			       !(tmp & XGMAC_BUSY), 100, 10000))
		return MYD_FAIL;

	/* Set the MII address register to read */
	writel(addr, priv->ioaddr + mii_address);
	writel(value, priv->ioaddr + mii_data);

	/* Wait until any existing MII operation is complete */
	if (readl_poll_timeout(priv->ioaddr + mii_data, tmp,
			       !(tmp & XGMAC_BUSY), 100, 10000))
		return MYD_FAIL;

	/* Read the data from the MII data register */
	*data = (u16)readl(priv->ioaddr + mii_data);

	return MYD_OK;
}

static MYD_STATUS xgbe_xgmac_mdio_write(MYD_DEV_PTR pdev, u16 mdio_port,
					u16 mmd, u16 reg, u16 data)
{
	struct xgmac_priv *priv = (struct xgmac_priv *)pdev->hostContext;
	unsigned int mii_address = priv->hw->mii.addr;
	unsigned int mii_data = priv->hw->mii.data;
	u32 addr, tmp;
	u32 value = XGMAC_BUSY;

	addr = ((mmd << XGMAC_DA_SHIFT) & XGMAC_DA_MASK) |
	       ((mdio_port << XGMAC_PA_SHIFT) & XGMAC_PA_MASK) | reg;

	value |= (priv->clk_csr << priv->hw->mii.clk_csr_shift)
		& priv->hw->mii.clk_csr_mask;
	value |= ((XGMAC_CMD_WRITE << XGMAC_CMD_SHIFT) & XGMAC_CMD_MASK);
	value |= data;

	/* Wait until any existing MII operation is complete */
	if (readl_poll_timeout(priv->ioaddr + mii_data, tmp,
			       !(tmp & XGMAC_BUSY), 100, 10000))
		return MYD_FAIL;

	/* Set the MII address register to write */
	writel(addr, priv->ioaddr + mii_address);
	writel(value, priv->ioaddr + mii_data);

	/* Wait until any existing MII operation is complete */
	if (readl_poll_timeout(priv->ioaddr + mii_data, tmp,
			       !(tmp & XGMAC_BUSY), 100, 10000))
		return MYD_FAIL;

	return MYD_OK;
}
#endif

#ifdef CONFIG_MV88X7120_PHY
/******************************************************************************
 Unload PHY driver
*******************************************************************************/
int xgbe_phy_unload(struct xgmac_priv *priv)
{
	/* Only XGMAC-0 or XLGMAC can access PHY */
	if (priv->plat->phy_lane != 0)
		return 0;

	if (g_pdev) {
//TODO		if (g_pdev->devEnabled == MYD_TRUE)
//			mydUnloadDriver(g_pdev);
		kfree(g_pdev);
		g_pdev = NULL;
	}

	return 0;
}

/******************************************************************************
 Initialize PHY driver
*******************************************************************************/
int xgbe_phy_init(struct xgmac_priv *priv, bool load_image)
{
//TODO	MYD_STATUS status;

	/* Only XGMAC-0 or XLGMAC can access PHY */
	if (priv->plat->phy_lane != 0) {
		if (g_pdev == NULL) {
			printk("PHY dev not created (lane %d)\n",
			       priv->plat->phy_lane);
			return -EFAULT;
		}
		return 0;
	}

	xgbe_xgmac_set_cl45(priv);

	priv->phy_dev = kzalloc(sizeof(MYD_DEV), GFP_KERNEL);
	if (!priv->phy_dev)
		return -ENOMEM;
	g_pdev = (MYD_DEV_PTR)priv->phy_dev;

	/* No EEPROM */
	g_pdev->devEEPROM = MYD_FALSE;

//TODO
#if 0
	/* All fw images are loaded by u-boot */
	status = mydInitDriver(xgbe_xgmac_mdio_read,
			       xgbe_xgmac_mdio_write,
			       (priv->plat->phy_addr & 0x1C),
			       NULL, 0, NULL, 0, NULL, 0,
			       priv, g_pdev);
	if (status != MYD_OK)
		return -EFAULT;

	status = mydLanePowerdown(g_pdev, priv->plat->phy_addr, MYD_LINE_SIDE,
				  MYD_ALL_LANES);
	if (status != MYD_OK)
		return -EFAULT;
#endif

	return 0;
}

int xgbe_phy_dev_status(void)
{
	return (g_pdev == NULL) ? 0 : 1;
}

/******************************************************************************
 Configure CLK_OUT_SE2 for recovered clock from line side, lane 0 of the port.

 Note: The datasheet says CLK_OUT_SE1 and CLK_OUT_SE2 can output recovered
 clock from any ports. But MYD API says CLK_OUT_SE1 can recover clock only
 from Port2 and Port3.
*******************************************************************************/
static int config_recovered_clk(struct xgmac_priv *priv)
{
	MYD_RCLK_MUX rclk_mux;
	MYD_STATUS status;
	u16 mdio_port, port_idx, mux_sel;

//TODO
return 0;
	/* Recovered clock is from lane 0 */
	if (priv->plat->phy_lane != 0)
		return 0;

	/* Initialized the structure to be ALL 0 */
	memset(&rclk_mux, 0, sizeof(rclk_mux));

	/* Recommend only call mydSetPinMode() with same Pin once in the whole
	 * program. The Pin mode MUST match schematic.
	 */
	status = mydSetPinMode(g_pdev, MYD_PIN_CLK_OUT_SE2, MYD_PIN_MODE_RCLK);
	if (status != MYD_OK)
		return -EFAULT;

	mdio_port = priv->plat->phy_addr;
	port_idx = MYD_GET_PORT_IDX(g_pdev, mdio_port);

	/* Configure Mux 1 on the line side. Use src clock 10T/20T/40T */
	rclk_mux.x7120.mux1[port_idx][MYD_LINE_MODE_IDX].srcClockSelect = 0;
	rclk_mux.x7120.mux1[port_idx][MYD_LINE_MODE_IDX].laneSelect = 0;
	rclk_mux.x7120.mux1[port_idx][MYD_LINE_MODE_IDX].divideRatio = 0x08;
	/* Enable the divider in any case */
	rclk_mux.x7120.mux1[port_idx][MYD_LINE_MODE_IDX].dividerConfig = 0x2;

	status = mydConfigRClkMux(g_pdev, &rclk_mux);
	if (status != MYD_OK)
		return -EFAULT;

	/* Select line side of current port, and and use mux 1 */
	mux_sel = 1;
	status = mydConfigRClkPin(g_pdev, MYD_PIN_CLK_OUT_SE2, mdio_port,
				  MYD_LINE_SIDE, mux_sel);
	if (status != MYD_OK)
		return -EFAULT;

	return 0;
}
/******************************************************************************
 Select operation mode (PCS or Repeater), speed, and some parameters.
*******************************************************************************/
int xgbe_phy_config(struct xgmac_priv *priv,
		    const struct phylink_link_state *state)
{
	MYD_OP_MODE host_mode;
	MYD_OP_MODE line_mode;
	MYD_STATUS status;
	u32 mode_option;
	u16 result;
	int ret;

//TODO
return 0;
	/* Optional MYD_MODE_ICAL_EFFORT_0 mode_option to speed up linkup time.
	 * When MYD_MODE_FORCE_RECONFIG is set, it re-configures the lane(s) on
	 * a port to the new mode.
	 */
	mode_option = MYD_MODE_FORCE_RECONFIG | MYD_MODE_ICAL_EFFORT_0;

	if (priv->plat->max_speed == SPEED_50000 ||
	    priv->plat->max_speed == SPEED_25000) {
		if (priv->plat->phy_lane != 0)
			return 0;

		if (priv->plat->max_speed == SPEED_50000) {
			host_mode = MYD_P50UP; // No auto-neg on host side
			line_mode = MYD_P50QP;
		}
		else {
			host_mode = MYD_P25LR; // No auto-neg on host side
			line_mode = MYD_P25CR;
		}
		status = mydSetModeSelection(g_pdev, priv->plat->phy_addr, 0,
				host_mode, line_mode, mode_option,
				&result);
		if (status != MYD_OK)
			return -EFAULT;

		if (state->an_enabled) {
			status = mydAutoNegControl(g_pdev, priv->plat->phy_addr,
					MYD_LINE_SIDE, 0, MYD_ENABLE, MYD_TRUE);
			if (status != MYD_OK)
				return -EFAULT;
		}
	}
	else {
		host_mode = MYD_P10LN; // No auto-neg on host side
		line_mode = MYD_P10LN;

		status = mydSetModeSelection(g_pdev, priv->plat->phy_addr,
					priv->plat->phy_lane, host_mode,
					line_mode, mode_option, &result);
		if (status != MYD_OK)
			return -EFAULT;

		status = mydLanePowerup(g_pdev, priv->plat->phy_addr, MYD_BOTH_SIDE,
					priv->plat->phy_lane);
		if (status != MYD_OK)
			return -EFAULT;

#if 0 // Not support autoneg
		if (state->an_enabled) {
			status = mydAutoNegControl(g_pdev,
					priv->plat->phy_addr,
					MYD_LINE_SIDE, priv->plat->phy_lane,
					MYD_ENABLE, MYD_TRUE);
			if (status != MYD_OK)
				return -EFAULT;
		}
#endif
	}

	ret = config_recovered_clk(priv);
	return ret;
}

#if 0 // Not support autoneg
static int phy_bits2speed(u32 speed_bits)
{
	if ((speed_bits & PHY_AN_10GB_KX4) || (speed_bits & PHY_AN_10GB_KR))
		return SPEED_10000;
	else if ((speed_bits & PHY_AN_25GB_CR_S) || (speed_bits & PHY_AN_25GB_CR))
		return SPEED_25000;
	else if ((speed_bits & PHY_AN_50GB_KR4_CR4))
		return SPEED_50000;
	else
		return SPEED_UNKNOWN;
}
#endif

/******************************************************************************
 This function gets phy link status. Only XGMAC-0 and XLGMAC can access MDIO.
 XGMAC-0 will get link status for other XGMAC. Other XGMAC will read the link
 status from link_state array.

 For PCS mode, call mydCheckPCSLinkStatus().
 For Repeater mode, call mydCheckRepeaterLinkStatus().
*******************************************************************************/
int xgbe_phy_get_state(struct xgmac_priv *priv,
		       struct phylink_link_state *state)
{
	u16 latched_status;
	u16 current_status;
	MYD_PCS_LINK_STATUS status_detail;
	MYD_STATUS status;
//	MYD_BOOL an_complete;
//	u32 result;
	int retries;
	int i;

//TODO
state->link = 1;
state->speed = priv->plat->max_speed;
state->duplex = DUPLEX_FULL;
return 0;
	/* Wait upto 0.1 second for link to come up */
	state->link = 0;
	retries = 2000;
	for (i = 0; i < retries; i++) {
		status = mydCheckPCSLinkStatus(g_pdev,
				priv->plat->phy_addr,
				priv->plat->phy_lane,
				&current_status,
				&latched_status,
				&status_detail);
		if (status == MYD_OK) {
			if (current_status == MYD_LINK_UP) {
				state->link = 1;
				break;
			}
		}
		udelay(50);
	}

	state->an_complete = 0;
	state->speed = SPEED_UNKNOWN;
	state->duplex = DUPLEX_HALF;
#if 0 // Not support autoneg
	if (state->an_enabled) {
		status = mydAutoNegCheckComplete(g_pdev,
					priv->plat->phy_addr,
					MYD_LINE_SIDE,
					priv->plat->phy_lane,
					&an_complete);
		if (status == MYD_OK && an_complete) {
			state->an_complete = 1;
			status = mydGetAutoNegResolution(g_pdev,
					priv->plat->phy_addr,
					MYD_LINE_SIDE,
					priv->plat->phy_lane,
					&result);
			if (status == MYD_OK)
				state->speed = phy_bits2speed(result);
		}
	}
#endif
	if (state->link) {
		state->pause = MLO_PAUSE_TX | MLO_PAUSE_RX;
		state->duplex = DUPLEX_FULL;
		if (state->speed == SPEED_UNKNOWN)
			state->speed = priv->plat->max_speed;
	}

	return 0;
}

/******************************************************************************
 Perform actions when link is up 
*******************************************************************************/
int xgbe_phy_link_up(struct xgmac_priv *priv, u16 speed)
{
	/* Nothing to do. Place holder for now. */
	return 0;
}

/******************************************************************************
 Validate PHY supported features
*******************************************************************************/
int xgbe_phy_validate(struct xgmac_priv *priv, unsigned long *supported,
		       struct phylink_link_state *state)
{
	struct mdio_xpcs_args *phy_args = &priv->hw->xpcs_args;

        linkmode_and(supported, supported, phy_args->supported);
        linkmode_and(state->advertising, state->advertising, phy_args->supported);

        return 0;
}

/******************************************************************************
 Perform sw reset.
*******************************************************************************/
int xgbe_phy_sw_reset(struct xgmac_priv *priv)
{
	MYD_STATUS status;

//TODO
return 0;
	status = mydLaneSoftReset(g_pdev, priv->plat->phy_addr,
				  MYD_LINE_SIDE, priv->plat->phy_lane, 1000);
	if (status == MYD_FAIL)
		return -EFAULT;

        return 0;
}

/******************************************************************************
 Perform sw reset.
*******************************************************************************/
int xgbe_phy_set_intf_down(struct xgmac_priv *priv)
{
	MYD_STATUS status;

//TODO
return 0;
	status = mydLanePowerdown(g_pdev, priv->plat->phy_addr, MYD_LINE_SIDE,
				  priv->plat->phy_lane);
	if (status == MYD_FAIL)
		return -EFAULT;

        return 0;
}

/******************************************************************************
 Set supported features
*******************************************************************************/
void xgbe_phy_supported_features(struct xgmac_priv *priv)
{
	struct mdio_xpcs_args *phy_args = &priv->hw->xpcs_args;
	int max_speed = priv->plat->max_speed;
	const int *supported;
	int i;

	if (max_speed == SPEED_50000)
		supported = phy_50g_supported;
	else if (max_speed == SPEED_25000)
		supported = phy_25g_supported;
	else if (max_speed == SPEED_2500 &&
		 (priv->plat->interface == PHY_INTERFACE_MODE_SGMII ||
		  priv->plat->interface == PHY_INTERFACE_MODE_XGMII))
		supported = phy_sgmii_2g5_supported;
	else
		supported = phy_10g_supported;

        for (i = 0; supported[i] != __ETHTOOL_LINK_MODE_MASK_NBITS; i++)
                set_bit(supported[i], phy_args->supported);
}
#else
int xgbe_phy_unload(struct xgmac_priv *priv)
{
	return 0;
}

int xgbe_phy_init(struct xgmac_priv *priv, bool load_image)
{
	return 0;
}

int xgbe_phy_config(struct xgmac_priv *priv,
		    const struct phylink_link_state *state)
{
	return 0;
}

int xgbe_phy_dev_status(void)
{
	return 1;
}

int xgbe_phy_get_state(struct xgmac_priv *priv,
		       struct phylink_link_state *state)
{
	state->link = 1;
	state->pause = MLO_PAUSE_TX | MLO_PAUSE_RX;
	state->duplex = DUPLEX_FULL;
	state->speed = priv->plat->max_speed;

	return 0;
}

int xgbe_phy_link_up(struct xgmac_priv *priv, u16 speed)
{
	return 0;
}

int xgbe_phy_validate(struct xgmac_priv *priv, unsigned long *supported,
		      struct phylink_link_state *state)
{
	return 0;
}

void xgbe_phy_supported_features(struct xgmac_priv *priv)
{
}

int xgbe_phy_sw_reset(struct xgmac_priv *priv)
{
	return 0;
}

int xgbe_phy_set_intf_down(struct xgmac_priv *priv)
{
	return 0;
}
#endif

int xgbe_phy_rtl_get_state(struct xgmac_priv *priv,
			   struct phylink_link_state *state)
{
	int reg;
	u16 speed;

	reg = xgbe_xgmac_mdio_read(priv, priv->plat->phy_addr,
				   MDIO_MMD_VEND2, MIIM_RTL8221B_PHY_STATUS);
	if (reg & MIIM_RTL8221B_LINK_UP) {
		state->link = 1;
		speed = (reg & MIIM_RTL8221B_SPEED_MASK);
		if (speed == MIIM_RTL8221B_SPEED_2500)
			state->speed = SPEED_2500;
		else if (speed == MIIM_RTL8221B_SPEED_1000)
			state->speed = SPEED_1000;
		else if (speed == MIIM_RTL8221B_SPEED_100)
			state->speed = SPEED_100;
		else
			state->speed = SPEED_10;

		if (reg & MIIM_RTL8221B_FULL_DUPLEX)
			state->duplex = DUPLEX_FULL;
		else
			state->duplex = DUPLEX_HALF;
	} else {
		state->link = 0;
		state->speed = SPEED_10;
		state->duplex = DUPLEX_HALF;
	}

	return 0;
}

