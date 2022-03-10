
#ifndef SRSGNB_SCHED_RESULT_H
#define SRSGNB_SCHED_RESULT_H

#include "prb_grant.h"
#include "sched_consts.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"
#include <cstddef>

namespace srsgnb {

/// Maximum grants per slot. Implementation-specific.
const size_t MAX_GRANTS = 16;
/// Maximum Logical channels per TB. Implementation-specific.
const size_t MAX_LC_GRANTS = 4;

struct pdcch_config {};

struct pdsch_config {};

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
struct msg3_freq_alloc {
  prb_interval prbs;
  rnti_t       tc_rnti;
};

struct rar_alloc_info {
  unsigned                                   rapid;
  unsigned                                   ta;
  static_vector<msg3_freq_alloc, MAX_GRANTS> msg3s;
  pdcch_config                               pdcch;
};

struct dl_sched_result {
  pci_t      pci;
  slot_point slot_value;

  /// Allocation of dedicated RARs
  static_vector<rar_alloc_info, MAX_GRANTS> rars;

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
