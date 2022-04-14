
#ifndef SRSGNB_SCHED_RESULT_H
#define SRSGNB_SCHED_RESULT_H

#include "ofdm_symbol_range.h"
#include "prb_grant.h"
#include "sched_consts.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include <cstddef>

namespace srsgnb {

/// Maximum grants per slot. Implementation-specific.
const size_t MAX_GRANTS = 16;
/// Maximum Logical channels per TB. Implementation-specific.
const size_t MAX_LC_GRANTS = 4;
/// Maximum SSB opportunity per slot. This can be derived from the candidate ODFM symbols indices within the ranges
/// 0-13, 14-27, 28-41, 42-55, etc.. from TS 38.213, Section 4.1
const size_t MAX_SSB_PER_SLOT = 2;

/// Representation of resources associated to a BWP.
struct bwp_configuration {
  subcarrier_spacing scs;
  uint8_t            cp;
  prb_interval       freq_alloc;
};

struct dl_dci_alloc {};

struct pdcch_configuration {};

struct pdsch_configuration {};

struct dl_msg_lc_info {
  /// LCID {0..32}
  lcid_t lcid;
  /// Number of scheduled bytes for this specific logical channel. {0..65535}
  unsigned sched_bytes;
};

struct dl_msg_tb_info {
  /// List of allocated logical channels
  static_vector<dl_msg_lc_info, MAX_LC_GRANTS> lc_lst;
};

/// Dedicated DL Grant for UEs
struct dl_msg_alloc {
  rnti_t                                        crnti;
  static_vector<dl_msg_tb_info, MAX_NOF_LAYERS> tbs;
};

/// Msg3 grant
struct msg3_information {
  unsigned     rapid;
  unsigned     ta;
  prb_interval prbs;
  rnti_t       temp_crnti;
};

/// See ORAN WG8, 9.2.3.3.10 - RAR information.
struct rar_information {
  du_cell_index_t                             cell_index;
  rnti_t                                      ra_rnti;
  static_vector<msg3_information, MAX_GRANTS> grants;
  pdcch_configuration                         pdcch;
};

/// Stores the information associated to an SSB.
struct ssb_information {
  unsigned          ssb_index;
  prb_interval      prbs;
  ofdm_symbol_range symbols;
};

/// Stores the information associated to an SIB1 or other SI allocation.
struct sib_information {
  enum si_indicator_type { sib1, other_si } si_indicator;
  unsigned            nof_txs;
  bwp_configuration   bwp_cfg;
  pdcch_configuration pdcch_cfg;
  pdsch_configuration pdsch_cfg;
};

/// See ORAN WG8, 9.2.3.3.12 - Downlink Broadcast Allocation.
struct dl_broadcast_allocation {
  static_vector<ssb_information, MAX_SSB_PER_SLOT> ssb_info;
  static_vector<sib_information, 1>                sibs;
};

struct dl_sched_result {
  pci_t      pci;
  slot_point slot_value;

  /// Allocation of SSB and SIBs
  dl_broadcast_allocation bc;

  /// Allocation of dedicated RARs
  static_vector<rar_information, MAX_GRANTS> rar_grants;

  /// Allocation of dedicated UE messages
  static_vector<dl_msg_alloc, MAX_GRANTS> ue_grants;
};

struct ul_sched_info {
  rnti_t crnti;
};

struct ul_sched_result {
  pci_t pci;
  /// Allocation of PUSCHs
  static_vector<ul_sched_info, MAX_GRANTS> puschs;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_RESULT_H
