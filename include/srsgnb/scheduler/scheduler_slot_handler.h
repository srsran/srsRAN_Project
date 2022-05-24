/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SCHEDULER_SLOT_HANDLER_H
#define SRSGNB_SCHEDULER_SLOT_HANDLER_H

#include "sched_consts.h"
#include "scheduler_dci.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/bwp_configuration.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/ofdm_symbol_range.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/prb_grant.h"
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

struct pdcch_context_information {
  const bwp_configuration*     bwp_cfg;
  const coreset_configuration* coreset_cfg;
  /// CCE position of the allocated PDCCH.
  cce_position cces;
  /// RNTI used to identify the destination of this DCI (e.g. UE, RA-RNTI, SI, Paging).
  rnti_t rnti;
};

/// PDCCH DL allocation.
struct pdcch_dl_information {
  /// Context associated with PDCCH allocation.
  pdcch_context_information ctx;
  /// DL DCI unpacked content.
  dci_dl_info dci;
};

/// PDCCH UL allocation.
struct pdcch_ul_information {
  /// Context associated with PDCCH allocation.
  pdcch_context_information ctx;
  /// UL DCI unpacked content.
  dci_ul_info dci;
};

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

/// RAR grant.
/// \remark See TS 38.213, Table 8.2-1.
struct rar_ul_grant {
  // MAC subheader.
  uint16_t rapid;

  // RAR payload.
  uint16_t ta;
  rnti_t   temp_crnti;

  // RAR Payload
  bool    freq_hop_flag;
  uint8_t freq_resource_assignment;
  uint8_t time_resource_assignment;
  int8_t  mcs;
  int8_t  tpc;
  bool    csi_req;
};

/// See ORAN WG8, 9.2.3.3.10 - RAR information.
struct rar_information {
  pdcch_dl_information*                   pdcch_cfg;
  static_vector<rar_ul_grant, MAX_GRANTS> grants;
};

/// Stores the information associated to an SSB.
struct ssb_information {
  unsigned          ssb_index;
  crb_interval      crbs;
  ofdm_symbol_range symbols;
};

/// Stores the information associated to an SIB1 or other SI allocation.
struct sib_information {
  enum si_indicator_type { sib1, other_si } si_indicator;
  unsigned                    nof_txs;
  const pdcch_dl_information* pdcch_cfg;
  pdsch_configuration         pdsch_cfg;
};

/// See ORAN WG8, 9.2.3.3.12 - Downlink Broadcast Allocation.
struct dl_broadcast_allocation {
  static_vector<ssb_information, MAX_SSB_PER_SLOT> ssb_info;
  static_vector<sib_information, 1>                sibs;
};

struct dl_sched_result {
  /// Allocated DL PDCCHs. Includes both SIB, RAR and Data PDCCHs.
  static_vector<pdcch_dl_information, MAX_GRANTS> dl_pdcchs;

  /// Allocated UL PDCCHs.
  static_vector<pdcch_ul_information, MAX_GRANTS> ul_pdcchs;

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
  /// Allocation of PUSCHs
  static_vector<ul_sched_info, MAX_GRANTS> puschs;
};

struct sched_result {
  dl_sched_result dl;
  ul_sched_result ul;
};

class scheduler_slot_handler
{
public:
  virtual ~scheduler_slot_handler()                                                         = default;
  virtual const sched_result* slot_indication(slot_point sl_tx, du_cell_index_t cell_index) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SCHEDULER_SLOT_HANDLER_H
