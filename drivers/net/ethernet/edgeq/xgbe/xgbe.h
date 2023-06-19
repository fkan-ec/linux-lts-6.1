/* SPDX-License-Identifier: GPL-2.0-only */
/*******************************************************************************
  XGBE Header File

*******************************************************************************/

#ifndef __XGBE_H__
#define __XGBE_H__

#define XGMAC_RESOURCE_NAME	"xgmaceth"
#define DRV_MODULE_VERSION	"Jun_2021"

#include <linux/clk.h>
#include <linux/if_vlan.h>
#include <linux/phylink.h>
#include <linux/pci.h>
#include "common.h"
#include <linux/ptp_clock_kernel.h>
#include <linux/net_tstamp.h>
#include <linux/reset.h>
#include <net/page_pool.h>
#include <linux/uio_driver.h>

#define CLOCK_SRC_PROP		"soc-clk-src" /** SoC clock source property name */

/* @brief SoC clock source enum copied from u-boot
 * @note Please be sure to update this enum should
 *		 there be any changes in u-boot
 */
enum soc_clk_src {
	SoC_CLK_DEFAULT,
	SoC_CLK_VCTCXO = 0,
	SoC_CLK_DCTCXO,
	SoC_CLK_DPLL,
	SoC_CLK_LOCAL_CLK,
	SoC_CLK_SRC_NUM,
};

struct xgmac_resources {
	void __iomem *xgmac_base;
	void __iomem *xpcs_base;
	void __iomem *ecpri_base;
	void __iomem *bss_base;
	void __iomem *misc_base;
	void __iomem *e56_base;
	dma_addr_t ecpri_phy_addr;
	const char *mac;
	int wol_irq;
	int lpi_irq;
	int irq;
};

struct xgmac_tx_info {
	dma_addr_t buf;
	bool map_as_page;
	unsigned len;
	bool last_segment;
	bool is_jumbo;
};

#define XGMAC_TBS_AVAIL		BIT(0)
#define XGMAC_TBS_EN		BIT(1)

/* Frequently used values are kept adjacent for cache effect */
struct xgmac_tx_queue {
	u32 tx_count_frames;
	int tbs;
	struct timer_list txtimer;
	u32 queue_index;
	struct xgmac_priv *priv_data;
	struct dma_extended_desc *dma_etx ____cacheline_aligned_in_smp;
	struct dma_edesc *dma_entx;
	struct dma_desc *dma_tx;
	struct sk_buff **tx_skbuff;
	struct xgmac_tx_info *tx_skbuff_dma;
	unsigned int cur_tx;
	unsigned int dirty_tx;
	dma_addr_t dma_tx_phy;
	u32 tx_tail_addr;
	u32 mss;
};

struct xgmac_rx_buffer {
	struct page *page;
	struct page *sec_page;
	dma_addr_t addr;
	dma_addr_t sec_addr;
};

struct xgmac_rx_queue {
	u32 rx_count_frames;
	u32 queue_index;
	struct page_pool *page_pool;
	struct xgmac_rx_buffer *buf_pool;
	struct xgmac_priv *priv_data;
	struct dma_extended_desc *dma_erx;
	struct dma_desc *dma_rx ____cacheline_aligned_in_smp;
	unsigned int cur_rx;
	unsigned int dirty_rx;
	u32 rx_zeroc_thresh;
	dma_addr_t dma_rx_phy;
	u32 rx_tail_addr;
	unsigned int state_saved;
	struct {
		struct sk_buff *skb;
		unsigned int len;
		unsigned int error;
	} state;
};

struct xgmac_channel {
	struct napi_struct rx_napi ____cacheline_aligned_in_smp;
	struct napi_struct tx_napi ____cacheline_aligned_in_smp;
	struct xgmac_priv *priv_data;
	spinlock_t lock;
	u32 index;
	int rx_irq;
	int tx_irq;
	char rx_irq_name[IFNAMSIZ + 32];
	char tx_irq_name[IFNAMSIZ + 32];
	int rx_irq_cpu;
	int tx_irq_cpu;
};

struct xgmac_tc_entry {
	bool in_use;
	bool in_hw;
	bool is_last;
	bool is_frag;
	void *frag_ptr;
	unsigned int table_pos;
	u32 handle;
	u32 prio;
	struct {
		u32 match_data;
		u32 match_en;
		u8 af:1;
		u8 rf:1;
		u8 im:1;
		u8 nc:1;
		u8 res1:4;
		u8 frame_offset;
		u8 ok_index;
		u8 res2;
		u16 dma_ch_no;
		u16 res3;
	} __packed val;
};

#define XGMAC_PPS_MAX		4
struct xgmac_pps_cfg {
	bool available;
	struct timespec64 start;
	struct timespec64 period;
};

struct xgmac_rss {
	int enable;
	unsigned int table_size;
	u8 key[XGMAC_RSS_HASH_KEY_SIZE];
	u32 table[XGMAC_RSS_MAX_TABLE_SIZE];
};

#define XGMAC_FLOW_ACTION_DROP		BIT(0)
struct xgmac_flow_entry {
	unsigned long cookie;
	unsigned long action;
	u8 ip_proto;
	int in_use;
	int idx;
	int is_l4;
};

struct xgmac_uio_device {
	char name[IFNAMSIZ];
	struct uio_info uinfo;
};

struct xgmac_priv {
	/* Frequently used values are kept adjacent for cache effect */
	u32 tx_coal_frames;
	u32 tx_coal_timer;
	u32 rx_coal_frames;

	int tx_coalesce;
	int hwts_tx_en;
	bool tx_path_in_lpi_mode;
	bool tso;
	int sph;
	u32 sarc_type;

	unsigned int dma_buf_sz;
	unsigned int rx_copybreak;
	u32 rx_riwt;
	int hwts_rx_en;

	void __iomem *ioaddr;
	void __iomem *xpcs_base;
	void __iomem *ecpri_base;
	void __iomem *misc_base;
	void __iomem *e56_base;
	dma_addr_t ecpri_phy_addr;
	struct ecpri_host_buf_hdr *h_buf_hdr;
	struct dma_desc *h_desc;
	struct ecpri_host_buf *h_pkt_buf;
	bool ecpri_over_eth;
	struct net_device *dev;
	struct device *device;
	struct mac_device_info *hw;
	int (*hwif_quirks)(struct xgmac_priv *priv);
	struct mutex lock;

	/* RX Queue */
	struct xgmac_rx_queue rx_queue[MTL_MAX_RX_QUEUES];

	/* TX Queue */
	struct xgmac_tx_queue tx_queue[MTL_MAX_TX_QUEUES];

	/* Generic channel for NAPI */
	struct xgmac_channel channel[XGMAC_CH_MAX];

	int speed;
	unsigned int flow_ctrl;
	unsigned int pause;
	struct mii_bus *mii;
	int mii_irq[PHY_MAX_ADDR];

	struct phylink_config phylink_config;
	struct phylink *phylink;
	void* phy_dev;

	struct xgmac_extra_stats xstats ____cacheline_aligned_in_smp;
	struct xgmac_safety_stats sstats;
	struct plat_xgmacenet_data *plat;
	struct dma_features dma_cap;
	struct xgmac_counters mmc;
	int hw_cap_support;
	int synopsys_id;
	int dev_id;
	u32 msg_enable;
	int wolopts;
	int wol_irq;
	int clk_csr;
	struct timer_list eee_ctrl_timer;
	int lpi_irq;
	int eee_enabled;
	int eee_active;
	int tx_lpi_timer;
	unsigned int mode;
	unsigned int chain_mode;
	int extend_desc;
	struct hwtstamp_config tstamp_config;
	struct ptp_clock *ptp_clock;
	struct ptp_clock_info ptp_clock_ops;
	unsigned int default_addend;
	u32 sub_second_inc;
	u32 systime_flags;
	int use_riwt;
	int irq_wake;
	spinlock_t ptp_lock;
	void __iomem *mmcaddr;
	void __iomem *ptpaddr;
	unsigned long active_vlans[BITS_TO_LONGS(VLAN_N_VID)];

#ifdef CONFIG_DEBUG_FS
	struct dentry *dbgfs_dir;
#endif

	unsigned long state;
	struct workqueue_struct *wq;
	struct work_struct service_task;

	/* TC Handling */
	unsigned int tc_entries_max;
	unsigned int tc_off_max;
	struct xgmac_tc_entry *tc_entries;
	unsigned int flow_entries_max;
	struct xgmac_flow_entry *flow_entries;

	/* Pulse Per Second output */
	struct xgmac_pps_cfg pps[XGMAC_PPS_MAX];

	/* Receive Side Scaling */
	struct xgmac_rss rss;

	/* UIO device */
	struct xgmac_uio_device uio_dev;
};

enum xgmac_state {
	XGMAC_DOWN,
	XGMAC_RESET_REQUESTED,
	XGMAC_RESETING,
	XGMAC_SERVICE_SCHED,
};

struct xgmac_hw_ts_config {
	u32 ptp_over_ipv4_udp;
	u32 ptp_over_ipv6_udp;
	u32 ptp_over_ethernet;
	u32 snap_type_sel;
	u32 ts_master_en;
	u32 ts_event_en;
	u32 tstamp_all;
};

int xgmac_mdio_unregister(struct net_device *ndev);
int xgmac_mdio_register(struct net_device *ndev);
int xgmac_mdio_reset(struct mii_bus *mii);
void xgmac_set_ethtool_ops(struct net_device *netdev);

void xgmac_ptp_register(struct xgmac_priv *priv);
void xgmac_ptp_unregister(struct xgmac_priv *priv);
int xgmac_resume(struct device *dev);
int xgmac_suspend(struct device *dev);
int xgmac_dvr_remove(struct device *dev);
int xgmac_dvr_probe(struct device *device,
		     struct plat_xgmacenet_data *plat_dat,
		     struct xgmac_resources *res);
void xgmac_disable_eee_mode(struct xgmac_priv *priv);
bool xgmac_eee_init(struct xgmac_priv *priv);
int xgbe_driver_init(void);
void xgbe_driver_exit(void);
void xgmac_get_rx_hwtstamp(struct xgmac_priv *priv, struct dma_desc *p,
                            struct dma_desc *np, struct sk_buff *skb);
void xgmac_rx_vlan(struct net_device *dev, struct sk_buff *skb);

#if IS_ENABLED(CONFIG_XGMAC_SELFTESTS)
void xgmac_selftest_run(struct net_device *dev,
			 struct ethtool_test *etest, u64 *buf);
void xgmac_selftest_get_strings(struct xgmac_priv *priv, u8 *data);
int xgmac_selftest_get_count(struct xgmac_priv *priv);
#else
static inline void xgmac_selftest_run(struct net_device *dev,
				       struct ethtool_test *etest, u64 *buf)
{
	/* Not enabled */
}
static inline void xgmac_selftest_get_strings(struct xgmac_priv *priv,
					       u8 *data)
{
	/* Not enabled */
}
static inline int xgmac_selftest_get_count(struct xgmac_priv *priv)
{
	return -EOPNOTSUPP;
}
#endif /* CONFIG_XGMAC_SELFTESTS */

#define SIOCGPTPSEC			(SIOCDEVPRIVATE+0)
#define SIOCSECPRIUDP			(SIOCDEVPRIVATE+1)
#define SIOCGECPRIUDP			(SIOCDEVPRIVATE+2)
#define SIOCSECPRIVLAN			(SIOCDEVPRIVATE+3)
#define SIOCGECPRIVLAN			(SIOCDEVPRIVATE+4)
#define SIOCSECPRIDESTIP		(SIOCDEVPRIVATE+5)
#define SIOCGECPRIDESTIP		(SIOCDEVPRIVATE+6)
#define SIOCSECPRIDESTMAC		(SIOCDEVPRIVATE+7)
#define SIOCGECPRIDESTMAC		(SIOCDEVPRIVATE+8)
#define SIOCSPMDSUPPORT			(SIOCDEVPRIVATE+9)
#define SIOCGPMDSUPPORT			(SIOCDEVPRIVATE+10)
#define SIOCSAUXSNAPEN			(SIOCDEVPRIVATE+11)
#define SIOCGAUXSNAPEN			(SIOCDEVPRIVATE+12)
#define SIOCGAUXSNAPTS			(SIOCDEVPRIVATE+13)

extern int pmd_enabled;

#endif /* __XGBE_H__ */
