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
#include "srsgnb/ran/pucch/pucch_tpc.h"
#include "srsgnb/ran/pucch/srs_tpc.h"
#include "srsgnb/ran/pusch/pusch_tpc.h"
#include "srsgnb/ran/ssb_configuration.h"

namespace srsgnb {

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
};

/// "PDSCH-Config" - UE-dedicated PDSCH Configuration as per TS38.331.
struct pdsch_config {
  /// Identifier used to initialize data scrambling (c_init) for PDSCH. If the field is absent, the UE applies the PCI.
  /// See TS38.331, \e dataScramblingIdentityPDSCH, and TS38.211, 7.3.1.1. Values: {0,...,1023}.
  optional<uint16_t>             data_scrambling_id_pdsch;
  optional<dmrs_downlink_config> pdsch_mapping_type_a_dmrs;
  optional<dmrs_downlink_config> pdsch_mapping_type_b_dmrs;
  /// PDSCH time domain resource allocations. Size: (0..maxNrofDL-Allocations=16).
  std::vector<pdsch_time_domain_resource_allocation> pdsch_td_alloc_list;
  // TODO: Remaining.
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
};

/// Uplink configuration, as per \c UplinkConfig, in \c ServingCellConfig, TS 38.331.
struct uplink_config {
  bwp_uplink_dedicated init_ul_bwp;
  // TODO: add remaining fields.
};

/// \c ServingCellConfig, as per TS38.331.
struct serving_cell_config {
  /// Initial Downlink BWP.
  bwp_downlink_dedicated init_dl_bwp;
  /// List of UE DL BWPs. Size: (0..maxNrofBWPs=4)
  std::vector<bwp_downlink> dl_bwps;
  /// \c uplinkConfig, containing the UL configuration.
  optional<uplink_config> ul_config;
};

} // namespace srsgnb
