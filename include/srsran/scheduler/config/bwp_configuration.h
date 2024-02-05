/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "dmrs.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/frame_types.h"
#include "srsran/ran/pcch/pcch_configuration.h"
#include "srsran/ran/pdcch/coreset.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/prach/rach_config_common.h"
#include "srsran/ran/prach/restricted_set_config.h"
#include "srsran/ran/pucch/pucch_configuration.h"
#include "srsran/ran/resource_allocation/ofdm_symbol_range.h"
#include "srsran/ran/resource_block.h"
#include "srsran/scheduler/vrb_alloc.h"
#include <bitset>

namespace srsran {

/// \remark See TS 38.331, "PDCCH-ConfigCommon"
struct pdcch_config_common {
  /// Contains Coreset#0.
  optional<coreset_configuration> coreset0;
  /// Contains common Coreset.
  optional<coreset_configuration> common_coreset;
  /// Contains SearchSpaceZero and commonSearchSpaceList. Size: (0..4).
  std::vector<search_space_configuration> search_spaces;
  search_space_id                         sib1_search_space_id;
  optional<search_space_id>               other_si_search_space_id;
  optional<search_space_id>               paging_search_space_id;
  /// SearchSpace of RA procedure. If field is invalid, the UE does not receive RAR in this BWP.
  search_space_id ra_search_space_id;
};

/// BWP-Id used to identify a BWP from the perspective of a UE.
/// \remark See TS 38.331, "BWP-Id" and "maxNrofBWPs".
enum bwp_id_t : uint8_t { MIN_BWP_ID = 0, MAX_BWP_ID = 3, MAX_NOF_BWPS = 4 };

/// Converts integer value to BWP-Id".
constexpr inline bwp_id_t to_bwp_id(std::underlying_type_t<bwp_id_t> value)
{
  return static_cast<bwp_id_t>(value);
}

/// Generic parameters of a bandwidth part as defined in TS 38.211, clause 4.5 and TS 38.213, clause 12.
/// \remark See TS 38.331, Bandwidth-Part (BWP).
struct bwp_configuration {
  cyclic_prefix      cp;
  subcarrier_spacing scs;
  /// Common RBs where the BWP is located. CRB=0 overlaps with pointA.
  crb_interval crbs;

  bool operator==(const bwp_configuration& other) const
  {
    return std::tie(cp, scs, crbs) == std::tie(other.cp, other.scs, other.crbs);
  }

  bool operator<(const bwp_configuration& other) const
  {
    return std::tie(cp, scs, crbs) < std::tie(other.cp, other.scs, other.crbs);
  }
};

/// \brief Physical shared channels Mapping Type.
/// \remark see TS38.214 Section 5.3 for PDSCH and TS38.214 Section 6.4 for PUSCH.
enum class sch_mapping_type {
  /// TypeA time allocation, it can start only at symbol 2 or 3 within a slot.
  typeA,
  /// TypeB time allocation.
  typeB
};

struct pdsch_time_domain_resource_allocation {
  /// Values: (0..32).
  uint8_t           k0;
  sch_mapping_type  map_type;
  ofdm_symbol_range symbols;

  bool operator==(const pdsch_time_domain_resource_allocation& rhs) const
  {
    return k0 == rhs.k0 && map_type == rhs.map_type && symbols == rhs.symbols;
  }
  bool operator!=(const pdsch_time_domain_resource_allocation& rhs) const { return !(rhs == *this); }
};

struct pdsch_config_common {
  /// PDSCH time domain resource allocations. Size: (0..maxNrofDL-Allocations=16).
  std::vector<pdsch_time_domain_resource_allocation> pdsch_td_alloc_list;
};

/// Used to configure the common, cell-specific parameters of a DL BWP.
/// \remark See TS 38.331, BWP-DownlinkCommon.
struct bwp_downlink_common {
  bwp_configuration   generic_params;
  pdcch_config_common pdcch_common;
  pdsch_config_common pdsch_common;
};

struct pusch_time_domain_resource_allocation {
  /// Values: (0..32).
  unsigned         k2;
  sch_mapping_type map_type;
  /// OFDM symbol boundaries for PUSCH. Network configures the fields so it does not cross the slot boundary.
  ofdm_symbol_range symbols;

  bool operator==(const pusch_time_domain_resource_allocation& rhs) const
  {
    return k2 == rhs.k2 && map_type == rhs.map_type && symbols == rhs.symbols;
  }
  bool operator!=(const pusch_time_domain_resource_allocation& rhs) const { return !(rhs == *this); }
};

/// \remark See TS 38.331, "PUSCH-ConfigCommon".
struct pusch_config_common {
  /// PUSCH time domain resource allocations. Size: (0..maxNrofUL-Allocations=16).
  std::vector<pusch_time_domain_resource_allocation> pusch_td_alloc_list;
  /// \brief \c msg3-DeltaPreamble, part of \c PUSCH-ConfigCommon, TS 38.331.
  /// Power offset between msg3 and RACH preamble transmission. The actual value is 2 * msg3-DeltaPreamble in dB.
  bounded_integer<int, -1, 6> msg3_delta_preamble;
  /// \brief \c p0-NominalWithGrant, part of \c PUSCH-ConfigCommon, TS 38.331.
  /// P0 value for PUSCH with grant (except msg3). Value in dBm. Only even values allowed.
  bounded_integer<int, -202, 24> p0_nominal_with_grant;

  /// Non RACH-ConfigCommon parameters.
  /// \brief Power level corresponding to MSG-3 TPC command in dB, as per Table 8.2-2, TS 38.213.
  /// Values {-6,...,8} and must be a multiple of 2.
  bounded_integer<int, -6, 8> msg3_delta_power;
};

/// \remark See TS 38.331, "PUCCH-ConfigCommon".
struct pucch_config_common {
  /// Values: {0,...,15}.
  uint8_t             pucch_resource_common;
  pucch_group_hopping group_hopping;
  /// Values: {0, ..., 1023}.
  optional<unsigned> hopping_id;
  /// Values: {-202, ..., 24}
  int p0_nominal;
};

/// Used to configure the common, cell-specific parameters of an UL BWP.
/// \remark See TS 38.331, BWP-UplinkCommon.
struct bwp_uplink_common {
  bwp_configuration             generic_params;
  optional<rach_config_common>  rach_cfg_common;
  optional<pusch_config_common> pusch_cfg_common;
  optional<pucch_config_common> pucch_cfg_common;
};

/// \brief It provides parameters determining the location and width of the actual carrier.
/// \remark See TS 38.331, "SCS-SpecificCarrier".
struct scs_specific_carrier {
  /// Offset between Point A (lowest subcarrier of common RB 0) and the lowest usable subcarrier in this carrier in
  /// number of PRBs. Values: (0..2199).
  unsigned           offset_to_carrier;
  subcarrier_spacing scs;
  /// Width of this carrier in number of PRBs. Values: (0..MAX_NOF_PRBS).
  unsigned carrier_bandwidth;
  /// Indicates the downlink Tx Direct Current location for the carrier. A value in the range 0..3299 indicates the
  /// subcarrier index within the carrier. The values in the value range 3301..4095 are reserved and ignored by the UE.
  /// If this field is absent, the UE assumes the default value of 3300 (i.e. "Outside the carrier").
  optional<unsigned> tx_direct_current_location;
};

/// \brief Used to indicate a frequency band
struct freq_band_indicator {
  nr_band band;
};

/// \brief This class provides basic parameters of a downlink carrier and transmission.
/// \remark See TS 38.331, "FrequencyInfoDL" and "FrequencyInfoDL-SIB".
struct frequency_info_dl {
  /// Absolute frequency (as ARFCN) of the SSB.
  unsigned absolute_frequency_ssb;
  /// Absolute frequency (in ARFCN) of the reference resource block (common RB0).
  unsigned absolute_freq_point_a;
  /// Represents the offset to Point A, as defined in TS 38.211, clause 4.4.4.2. Values: (0..2199).
  unsigned offset_to_point_a;
  /// Set of carriers for different subcarrier spacings. The network configures this for all SCSs that are used in
  /// DL BWPs in this serving cell. Size: (1..maxSCSs=5).
  std::vector<scs_specific_carrier> scs_carrier_list;

  /// Set of frequency bands.
  std::vector<freq_band_indicator> freq_band_list;
};

/// \brief Downlink Configuration, common to the serving cell.
/// \remark See TS 38.331, "DownlinkConfigCommonSIB".
struct dl_config_common {
  frequency_info_dl   freq_info_dl;
  bwp_downlink_common init_dl_bwp;
  pcch_config         pcch_cfg;
};

struct frequency_info_ul {
  /// Absolute frequency (in ARFCN) of the CRB0.
  unsigned absolute_freq_point_a;
  /// Set of carriers for different subcarrier spacings. The network configures this for all SCSs that are used in
  /// UL BWPs in this serving cell. Size: (1..maxSCSs=5).
  std::vector<scs_specific_carrier> scs_carrier_list;
  bool                              freq_shift_7p5khz_present;
  /// Maximum transmit power allowed in this serving cell. Values: {-30,...,33}dBm. See TS 38.331, \c p-Max under \c
  /// FrequencyInfoUL.
  optional<bounded_integer<int, -30, 33>> p_max;

  /// Set of frequency bands.
  std::vector<freq_band_indicator> freq_band_list;
  // TODO: Add other fields.
};

/// \brief Uplink Configuration, common to the serving cell.
/// \remark See TS 38.331, "UplinkConfigCommonSIB".
struct ul_config_common {
  frequency_info_ul freq_info_ul;
  bwp_uplink_common init_ul_bwp;
};

} // namespace srsran
