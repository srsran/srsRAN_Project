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
#include "srsgnb/ran/ssb_configuration.h"

namespace srsgnb {

/// "PDCCH-Config" - UE-dedicated PDCCH configuration as per TS 38.331.
struct pdcch_config {
  /// List of CORESETs to be used by the UE. In case of CORESET Id overlaps with commonControlResourceSet,
  /// the CORESET in this list takes precedence. Size: (0..3).
  std::vector<coreset_configuration>      coresets;
  std::vector<search_space_configuration> search_spaces;
  // TODO: add remaining fields.
};

/// "PDSCH-Config" - UE-dedicated PDSCH Configuration as per TS38.331.
struct pdsch_config {
  /// Identifier used to initialize data scrambling (c_init) for PDSCH. If the field is absent, the UE applies the PCI.
  /// See TS38.331, \e dataScramblingIdentityPDSCH, and TS38.211, 7.3.1.1. Values: {0,...,1023}.
  optional<uint16_t> data_scrambling_id_pdsch;
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
  std::vector<bwp_downlink> dl_bwps_to_addmod_list;
  /// List of UE DL BWPs to remove. Size: (0..maxNrofBWPs=4)
  std::vector<bwp_id_t> dl_bwps_to_rel_list;
  uplink_config         ul_cfg;
};

} // namespace srsgnb
