/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/pdcch/coreset.h"
#include "srsran/ran/pdcch/dci_packing.h"
#include "srsran/ran/pdcch/pdcch_context.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {
namespace fapi {

/// \brief Maximum number of DCIs per PDCCH PDU.
///
/// \note DCIs are grouped by CORESET, BWP and starting symbol of the PDCCH PDU.
constexpr size_t MAX_NUM_DCIS_PER_PDCCH_PDU = MAX_DL_PDCCH_PDUS_PER_SLOT;

/// PDCCH PDU maintenance information added in FAPIv3.
struct dl_pdcch_pdu_maintenance_v3 {
  uint16_t pdcch_pdu_index;
  uint16_t obsolete;

  /// DCI-specific parameters.
  struct maintenance_info {
    uint16_t dci_index;
    uint8_t  collocated_AL16_candidate;
  };

  static_vector<maintenance_info, MAX_NUM_DCIS_PER_PDCCH_PDU> info;
};

/// PDCCH PDU parameters added in FAPIv4.
struct dl_pdcch_pdu_parameters_v4 {
  /// DCI-specific parameters.
  struct dci_params {
    uint16_t nid_pdcch_dmrs;
  };
  static_vector<dci_params, MAX_NUM_DCIS_PER_PDCCH_PDU> params;

  //: TODO: spatial_stream_indices_present
  //: TODO: mu_mimo
};

/// Precoding and beamforming PDU.
struct tx_precoding_and_beamforming_pdu {
  /// Maximum number of digital beamforming interfaces supported. Implementation defined.
  static constexpr unsigned MAX_NUM_SUPPORTED_DIGITAL_BEAMFORMING_INTERFACES = 4U;
  /// Maximum number of PRGs supported. Implementation defined.
  static constexpr unsigned MAX_NUM_PRGS = 1U;

  /// Physical resource groups information.
  struct prgs_info {
    uint16_t                                                                  pm_index;
    static_vector<uint16_t, MAX_NUM_SUPPORTED_DIGITAL_BEAMFORMING_INTERFACES> beam_index;
  };

  /// Setting this variable to a value other than 0 marks the struct as not being used.
  uint8_t                                trp_scheme = 1U;
  uint16_t                               prg_size;
  uint8_t                                dig_bf_interfaces;
  static_vector<prgs_info, MAX_NUM_PRGS> prgs;
};

/// Downlink DCI PDU configuration.
struct dl_dci_pdu {
  rnti_t                           rnti;
  uint16_t                         nid_pdcch_data;
  uint16_t                         nrnti_pdcch_data;
  uint8_t                          cce_index;
  uint8_t                          aggregation_level;
  tx_precoding_and_beamforming_pdu precoding_and_beamforming;
  int8_t                           power_control_offset_ss_profile_nr;
  dci_payload                      payload;
  // Vendor specific parameters.
  std::optional<pdcch_context> context;
};

/// CORESET CCE to REG mapping type.
enum class cce_to_reg_mapping_type : uint8_t { non_interleaved, interleaved };
enum class pdcch_coreset_type : uint8_t { pbch_or_coreset0, other };

/// Downlink PDCCH PDU information.
struct dl_pdcch_pdu {
  uint16_t                                              coreset_bwp_size;
  uint16_t                                              coreset_bwp_start;
  subcarrier_spacing                                    scs;
  cyclic_prefix                                         cp;
  uint8_t                                               start_symbol_index;
  uint8_t                                               duration_symbols;
  freq_resource_bitmap                                  freq_domain_resource;
  cce_to_reg_mapping_type                               cce_reg_mapping_type;
  uint8_t                                               reg_bundle_size;
  uint8_t                                               interleaver_size;
  pdcch_coreset_type                                    coreset_type;
  uint16_t                                              shift_index;
  coreset_configuration::precoder_granularity_type      precoder_granularity;
  static_vector<dl_dci_pdu, MAX_NUM_DCIS_PER_PDCCH_PDU> dl_dci;
  dl_pdcch_pdu_maintenance_v3                           maintenance_v3;
  dl_pdcch_pdu_parameters_v4                            parameters_v4;
};

} // namespace fapi
} // namespace srsran
