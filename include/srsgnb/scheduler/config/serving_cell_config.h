/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "bwp_configuration.h"
#include "srsgnb/ran/carrier_configuration.h"
#include "srsgnb/ran/pdcch/downlink_preemption.h"
#include "srsgnb/ran/pdsch/pdsch_mcs.h"
#include "srsgnb/ran/pdsch/pdsch_prb_bundling.h"
#include "srsgnb/ran/pdsch/pdsch_rate_match_pattern.h"
#include "srsgnb/ran/pucch/pucch_tpc.h"
#include "srsgnb/ran/pucch/srs_configuration.h"
#include "srsgnb/ran/pucch/srs_tpc.h"
#include "srsgnb/ran/pusch/pusch_configuration.h"
#include "srsgnb/ran/pusch/pusch_tpc.h"
#include "srsgnb/ran/ssb_configuration.h"
#include "srsgnb/ran/tci.h"

namespace srsgnb {

/// \c xOverhead options, as per TS 38.331. Used in \c PDSCH-ServingCellConfig and \c PUSCH-ServingCellConfig.
enum class x_overhead {
  not_set = 0,
  xoh6    = 6,
  xoh12   = 12,
  xoh18   = 18,
};

/// Converts a uint into a \ref serv_cell_index.
inline serv_cell_index_t uint_to_serv_cell_index(unsigned idx)
{
  return idx < MAX_NOF_SERVING_CELLS ? static_cast<serv_cell_index_t>(idx)
                                     : static_cast<serv_cell_index_t>(MAX_NOF_SERVING_CELLS);
}

/// "PDCCH-Config" - UE-dedicated PDCCH configuration as per TS 38.331.
struct pdcch_config {
  /// List of CORESETs to be used by the UE. In case of CORESET Id overlaps with commonControlResourceSet,
  /// the CORESET in this list takes precedence. The network configures at most 3 CORESETs per BWP per cell (including
  /// UE-specific and common CORESETs).
  std::vector<coreset_configuration> coresets;
  /// List of SearchSpaces to be used by the UE. The network configures at most 10 Search Spaces per BWP per cell
  /// (including UE-specific and common Search Spaces).
  std::vector<search_space_configuration> search_spaces;
  /// Configuration of downlink preemption indications to be monitored in this cell.
  optional<downlink_preemption> dl_preemption;
  /// TPC Commands Configuration to configure UE for extracting TPC commands for PUSCH from a group-TPC messages on DCI.
  optional<pusch_tpc_command_config> pusch_tpc_cmd_cfg;
  /// TPC Commands Configuration to configure UE for extracting TPC commands for PUCCH from a group-TPC messages on DCI.
  optional<pucch_tpc_command_config> pucch_tpc_cmd_cfg;
  /// TPC Commands Configuration to configure UE for extracting TPC commands for SRS from a group-TPC messages on DCI.
  optional<srs_tpc_command_config> srs_tpc_cmd_cfg;
  // TODO: add remaining fields.

  bool operator==(const pdcch_config& rhs) const
  {
    return coresets == rhs.coresets && search_spaces == rhs.search_spaces && dl_preemption == rhs.dl_preemption &&
           pusch_tpc_cmd_cfg == rhs.pusch_tpc_cmd_cfg && pucch_tpc_cmd_cfg == rhs.pucch_tpc_cmd_cfg &&
           srs_tpc_cmd_cfg == rhs.srs_tpc_cmd_cfg;
  }
  bool operator!=(const pdcch_config& rhs) const { return !(rhs == *this); }
};

/// "PDSCH-Config" - UE-dedicated PDSCH Configuration as per TS38.331.
struct pdsch_config {
  /// \brief Interleaving unit configurable between 2 and 4 PRBs.
  /// \remark See TS 38.211, clause 7.3.1.6.
  enum class vrb_to_prb_interleaver { n2, n4 };

  /// \brief Resource allocation type of to DCI format 1_1.
  /// \remark See TS 38.214, clause 5.1.2.2.
  enum class resource_allocation { resource_allocation_type_0, resource_allocation_type_1, dynamic_switch };

  /// \brief Aggregation factor.
  /// \remark See TS 38.214, clause 5.1.2.1.
  enum class pdsch_aggregation_factor { n2, n4, n8 };

  /// \brief Maximum number of code words a single DCI may schedule.
  /// \remark See TS 38.331, "maxNrofCodeWordsScheduledByDCI".
  enum class max_no_codeword_scheduled_by_dci { n1, n2 };

  /// Identifier used to initialize data scrambling (c_init) for PDSCH. If the field is absent, the UE applies the PCI.
  /// See TS38.331, \e dataScramblingIdentityPDSCH, and TS38.211, 7.3.1.1. Values: {0,...,1023}.
  optional<uint16_t> data_scrambling_id_pdsch;
  /// DMRS configuration for PDSCH transmissions using PDSCH mapping type A (chosen dynamically via
  /// PDSCH-TimeDomainResourceAllocation).
  optional<dmrs_downlink_config> pdsch_mapping_type_a_dmrs;
  /// DMRS configuration for PDSCH transmissions using PDSCH mapping type B (chosen dynamically via
  /// PDSCH-TimeDomainResourceAllocation).
  optional<dmrs_downlink_config> pdsch_mapping_type_b_dmrs;
  /// Interleaving unit. If field is absent, the UE performs non-interleaved VRB-to-PRB mapping. The field
  /// vrb-ToPRB-Interleaver applies to DCI format 1_1.
  optional<vrb_to_prb_interleaver> vrb_to_prb_itlvr;
  /// A list of Transmission Configuration Indicator (TCI) states indicating a transmission configuration which includes
  /// QCL-relationships between the DL RSs in one RS set and the PDSCH DMRS ports (see TS 38.214, clause 5.1.5).
  static_vector<tci_state, MAX_NOF_TCI_STATES> tci_states;
  /// Configuration of resource allocation type 0 and resource allocation type 1 for non-fallback DCI.
  resource_allocation res_alloc;
  /// Number of repetitions for data. When the field is absent the UE applies the value 1.
  optional<pdsch_aggregation_factor> aggr_factor;
  /// PDSCH time domain resource allocations. Size: (0..maxNrofDL-Allocations=16).
  std::vector<pdsch_time_domain_resource_allocation> pdsch_td_alloc_list;
  /// Resources patterns which the UE should rate match PDSCH around. The UE rate matches around the union of all
  /// resources indicated in the rate match patterns. Rate match patterns defined here on cell level apply only to PDSCH
  /// of the same numerology. See 38.214, clause 5.1.4,1.
  static_vector<rate_match_pattern, MAX_NOF_RATE_MATCH_PATTERNS> rate_match_pattrn;
  // TODO: RateMatchPatternGroup1 and RateMatchPatternGroup2

  /// Selection between config 1 and config 2 for RBG size for PDSCH. The UE ignores this field if resourceAllocation is
  /// set to resourceAllocationType1.
  rbg_size rbg_sz;
  /// Indicates which MCS table the UE shall use for PDSCH. (see TS 38.214 [19], clause 5.1.3.1). The field mcs-Table
  /// applies to DCI format 1_0 and DCI format 1_1.
  pdsch_mcs_table mcs_table{pdsch_mcs_table::qam64};
  /// This changes the number of MCS/RV/NDI bits in the DCI message from 1 to 2.
  optional<max_no_codeword_scheduled_by_dci> nof_cw_sched_by_dci;
  /// Indicates the PRB bundle type and bundle size(s). If dynamic is chosen, the actual bundleSizeSet1 or
  /// bundleSizeSet2 to use is indicated via DCI. Constraints on bundleSize(Set) setting depending on
  /// vrb-ToPRB-Interleaver and rbg-Size settings are described in TS 38.214, clause 5.1.2.3. If a bundleSize(Set)
  /// value is absent, the UE applies the value n2. The field prb-BundlingType applies to DCI format 1_1.
  prb_bundling prb_bndlg;

  // TODO: Remaining.

  bool operator==(const pdsch_config& rhs) const
  {
    return data_scrambling_id_pdsch == rhs.data_scrambling_id_pdsch &&
           pdsch_mapping_type_a_dmrs == rhs.pdsch_mapping_type_a_dmrs &&
           pdsch_mapping_type_b_dmrs == rhs.pdsch_mapping_type_b_dmrs && vrb_to_prb_itlvr == rhs.vrb_to_prb_itlvr &&
           tci_states == rhs.tci_states && res_alloc == rhs.res_alloc && aggr_factor == rhs.aggr_factor &&
           pdsch_td_alloc_list == rhs.pdsch_td_alloc_list && rate_match_pattrn == rhs.rate_match_pattrn &&
           rbg_sz == rhs.rbg_sz && mcs_table == rhs.mcs_table && nof_cw_sched_by_dci == rhs.nof_cw_sched_by_dci &&
           prb_bndlg == rhs.prb_bndlg;
  }
  bool operator!=(const pdsch_config& rhs) const { return !(rhs == *this); }
};

/// "BWP-DownlinkDedicated" as per TS 38.331.
struct bwp_downlink_dedicated {
  optional<pdcch_config> pdcch_cfg;
  optional<pdsch_config> pdsch_cfg;
  // TODO: Remaining
};

/// "BWP-Downlink" as per TS 38.331.
struct bwp_downlink {
  bwp_id_t                         bwp_id;
  optional<bwp_downlink_common>    bwp_dl_common;
  optional<bwp_downlink_dedicated> bwp_dl_ded;
};

/// Used to configure the dedicated UE-specific parameters of an UL BWP.
/// \remark See TS 38.331, BWP-UplinkDedicated.
struct bwp_uplink_dedicated {
  optional<pucch_config> pucch_cfg;
  optional<pusch_config> pusch_cfg;
  optional<srs_config>   srs_cfg;
};

/// \brief Used to configure UE specific PUSCH parameters that are common across the UE's BWPs of one serving cell.
/// \remark See TS 38.331, PUSCH-ServingCellConfig.
struct pusch_serving_cell_config {
  struct pusch_code_block_group_transmission {
    /// Maximum number of code-block-groups (CBGs) per TB. See TS 38.213, clause 9.1.
    enum class max_code_block_groups_per_transport_block { n2, n4, n6, n8 };

    max_code_block_groups_per_transport_block max_cgb_per_tb;
  };

  optional<pusch_code_block_group_transmission> cbg_tx;
  x_overhead                                    x_ov_head{x_overhead::not_set};
};

/// Uplink configuration, as per \c UplinkConfig, in \c ServingCellConfig, TS 38.331.
struct uplink_config {
  bwp_uplink_dedicated                init_ul_bwp;
  optional<pusch_serving_cell_config> pusch_scell_cfg;
  // TODO: add remaining fields.
};

/// \c PDSCH-CodeBlockGroupTransmission, as per TS38.331.
struct pdsch_code_block_group_transmission {
  /// \c maxCodeBlockGroupsPerTransportBlock.
  enum class max_code_block_groups_per_tb { n1, n2, n6, n8 };

  max_code_block_groups_per_tb max_cbg_per_tb;
  bool                         code_block_group_flush_indicator;
};

/// \c PDSCH-ServingCellConfig, as per TS38.331.
struct pdsch_serving_cell_config {
  /// \c nrofHARQ-ProcessesForPDSCH.
  enum class nof_harq_proc_for_pdsch { n2, n4, n6, n10, n12, n16, not_set };

  optional<pdsch_code_block_group_transmission> code_block_group_tx;
  x_overhead                                    x_ov_head{x_overhead::not_set};
  nof_harq_proc_for_pdsch                       nof_harq_proc{nof_harq_proc_for_pdsch::not_set};
  optional<serv_cell_index_t>                   pucch_cell;
  /// Values {1,...,8};
  unsigned       max_mimo_layers;
  optional<bool> processing_type_2_enabled;
};

/// \c ServingCellConfig, as per TS38.331.
struct serving_cell_config {
  /// DU-specific cell identifier.
  du_cell_index_t cell_index;
  /// Initial Downlink BWP.
  bwp_downlink_dedicated init_dl_bwp;
  /// List of UE DL BWPs. Size: (0..maxNrofBWPs=4)
  static_vector<bwp_downlink, MAX_NOF_BWPS> dl_bwps;
  /// \c uplinkConfig, containing the UL configuration.
  optional<uplink_config> ul_config;
  /// \c pdsch-ServingCellConfig.
  optional<pdsch_serving_cell_config> pdsch_serv_cell_cfg;
};

/// UE-dedicated configuration for serving cell, as per TS38.331.
struct cell_config_dedicated {
  serv_cell_index_t   serv_cell_idx;
  serving_cell_config serv_cell_cfg;
};

} // namespace srsgnb
