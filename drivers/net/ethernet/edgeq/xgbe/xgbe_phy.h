// SPDX-License-Identifier: GPL-2.0-only
#ifndef _XGBE_PHY_H__
#define _XGBE_PHY_H__

int xgbe_phy_unload(struct xgmac_priv *priv);
int xgbe_phy_init(struct xgmac_priv *priv, bool load_image);
int xgbe_phy_dev_status(void);
int xgbe_phy_config(struct xgmac_priv *priv,
		    const struct phylink_link_state *state);
int xgbe_phy_get_state(struct xgmac_priv *priv,
		       struct phylink_link_state *state);
int xgbe_phy_link_up(struct xgmac_priv *priv, u16 speed);
int xgbe_phy_validate(struct xgmac_priv *priv, unsigned long *supported,
		      struct phylink_link_state *state);
void xgbe_phy_supported_features(struct xgmac_priv *priv);
int xgbe_phy_sw_reset(struct xgmac_priv *priv);
int xgbe_phy_set_intf_down(struct xgmac_priv *priv);

/* RealTek PHY MDIO_MMD_VEND2 register */
#define MIIM_RTL8221B_PHY_STATUS	0xa434
#define MIIM_RTL8221B_LINK_UP		0x4
#define MIIM_RTL8221B_FULL_DUPLEX	0x8
#define MIIM_RTL8221B_SPEED_MASK	0x630
#define MIIM_RTL8221B_SPEED_2500	0x210
#define MIIM_RTL8221B_SPEED_1000	0x020
#define MIIM_RTL8221B_SPEED_100		0x010

int xgbe_phy_rtl_get_state(struct xgmac_priv *priv,
			   struct phylink_link_state *state);

#endif // _XGBE_PHY_H__
