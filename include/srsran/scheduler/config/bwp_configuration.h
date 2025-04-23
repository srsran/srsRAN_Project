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

#include "srsran/ran/bwp/bwp_configuration.h"
#include "srsran/ran/pcch/pcch_configuration.h"
#include "srsran/ran/pdcch/coreset.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/prach/rach_config_common.h"
#include "srsran/ran/pucch/pucch_configuration.h"
#include "srsran/ran/scs_specific_carrier.h"
#include "srsran/scheduler/config/pxsch_time_domain_resource.h"
#include <optional>

namespace srsran {

/// \remark See TS 38.331, "PDCCH-ConfigCommon"
struct pdcch_config_common {
  /// Contains Coreset#0.
  std::optional<coreset_configuration> coreset0;
  /// Contains common Coreset.
  std::optional<coreset_configuration> common_coreset;
  /// Contains SearchSpaceZero and commonSearchSpaceList. Size: (0..4).
  std::vector<search_space_configuration> search_spaces;
  search_space_id                         sib1_search_space_id;
  std::optional<search_space_id>          other_si_search_space_id;
  std::optional<search_space_id>          paging_search_space_id;
  /// SearchSpace of RA procedure. If field is invalid, the UE does not receive RAR in this BWP.
  search_space_id ra_search_space_id;

  bool operator==(const pdcch_config_common& other) const
  {
    return coreset0 == other.coreset0 and common_coreset == other.common_coreset and
           search_spaces == other.search_spaces and sib1_search_space_id == other.sib1_search_space_id and
           other_si_search_space_id == other.other_si_search_space_id and
           paging_search_space_id == other.paging_search_space_id and ra_search_space_id == other.ra_search_space_id;
  }
};

/// BWP-Id used to identify a BWP from the perspective of a UE.
/// \remark See TS 38.331, "BWP-Id" and "maxNrofBWPs".
enum bwp_id_t : uint8_t { MIN_BWP_ID = 0, MAX_BWP_ID = 3, MAX_NOF_BWPS = 4 };

/// Converts integer value to BWP-Id".
constexpr bwp_id_t to_bwp_id(std::underlying_type_t<bwp_id_t> value)
{
  return static_cast<bwp_id_t>(value);
}

struct pdsch_config_common {
  /// PDSCH time domain resource allocations. Size: (0..maxNrofDL-Allocations=16).
  std::vector<pdsch_time_domain_resource_allocation> pdsch_td_alloc_list;

  bool operator==(const pdsch_config_common& other) const { return pdsch_td_alloc_list == other.pdsch_td_alloc_list; }
};

/// Used to configure the common, cell-specific parameters of a DL BWP.
/// \remark See TS 38.331, BWP-DownlinkCommon.
struct bwp_downlink_common {
  bwp_configuration   generic_params;
  pdcch_config_common pdcch_common;
  pdsch_config_common pdsch_common;

  bool operator==(const bwp_downlink_common& other) const
  {
    return generic_params == other.generic_params and pdcch_common == other.pdcch_common and
           pdsch_common == other.pdsch_common;
  }
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

  bool operator==(const pusch_config_common& other) const
  {
    return pusch_td_alloc_list == other.pusch_td_alloc_list and msg3_delta_preamble == other.msg3_delta_preamble and
           p0_nominal_with_grant == other.p0_nominal_with_grant and msg3_delta_power == other.msg3_delta_power;
  }
};

/// \remark See TS 38.331, "PUCCH-ConfigCommon".
struct pucch_config_common {
  /// Values: {0,...,15}.
  uint8_t             pucch_resource_common;
  pucch_group_hopping group_hopping;
  /// Values: {0, ..., 1023}.
  std::optional<unsigned> hopping_id;
  /// Values: {-202, ..., 24}
  int p0_nominal;

  bool operator==(const pucch_config_common& other) const
  {
    return pucch_resource_common == other.pucch_resource_common and group_hopping == other.group_hopping and
           hopping_id == other.hopping_id and p0_nominal == other.p0_nominal;
  }
};

/// Used to configure the common, cell-specific parameters of an UL BWP.
/// \remark See TS 38.331, BWP-UplinkCommon.
struct bwp_uplink_common {
  bwp_configuration                  generic_params;
  std::optional<rach_config_common>  rach_cfg_common;
  std::optional<pusch_config_common> pusch_cfg_common;
  std::optional<pucch_config_common> pucch_cfg_common;

  bool operator==(const bwp_uplink_common& other) const
  {
    return generic_params == other.generic_params and rach_cfg_common == other.rach_cfg_common and
           pusch_cfg_common == other.pusch_cfg_common and pucch_cfg_common == other.pucch_cfg_common;
  }
};

/// \brief Used to indicate a frequency band
struct freq_band_indicator {
  nr_band band;
};

/// \brief This class provides basic parameters of a downlink carrier and transmission.
/// \remark See TS 38.331, "FrequencyInfoDL" and "FrequencyInfoDL-SIB".
struct frequency_info_dl {
  /// \brief Absolute frequency of the SSB as ARFCN. This is the ARFCN of the SS_ref (or SSB central frequency).
  /// SS_ref is defined is per TS 38.104, Section 5.4.3.1 and 5.4.3.2.
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

/// Configuration for BCCH.
struct bcch_config {
  enum class modification_period_coeff : uint8_t { n2 = 2, n4 = 4, n8 = 8, n16 = 16 };

  /// Modification period coefficient used to determine the modification period of the SI as per TS 38.331, 5.2.2.2.2.
  modification_period_coeff mod_period_coeff = modification_period_coeff::n4;
};

/// \brief Downlink Configuration, common to the serving cell.
/// \remark See TS 38.331, "DownlinkConfigCommonSIB".
struct dl_config_common {
  frequency_info_dl   freq_info_dl;
  bwp_downlink_common init_dl_bwp;
  bcch_config         bcch_cfg;
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
  std::optional<bounded_integer<int, -30, 33>> p_max;

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
