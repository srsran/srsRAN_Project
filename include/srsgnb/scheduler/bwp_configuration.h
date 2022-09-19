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

#include "dmrs.h"
#include "prb_grant.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/ran/frame_types.h"
#include "srsgnb/ran/ofdm_symbol_range.h"
#include "srsgnb/ran/pdcch/coreset.h"
#include "srsgnb/ran/prach/restricted_set_config.h"
#include "srsgnb/ran/pucch/pucch_configuration.h"
#include "srsgnb/ran/resource_block.h"
#include <bitset>

namespace srsgnb {

/// \brief Search Space identifier. This value is UE-specific, which means that a UE can have up to
/// "maxNrofSearchSpaces=40" Search Spaces configured. The ID space is used across BWPs of a serving cell.
enum search_space_id : uint8_t { MIN_SEARCH_SPACE_ID = 0, MAX_SEARCH_SPACE_ID = 39, MAX_NOF_SEARCH_SPACES = 40 };

/// \remark See TS 38.331, "SearchSpaceId" - The number of Search Spaces per BWP is limited to 10 including the common
/// and UE specific Search Spaces.
constexpr size_t MAX_NOF_SEARCH_SPACE_PER_BWP = 10;

inline search_space_id to_search_space_id(unsigned ss_id)
{
  return static_cast<search_space_id>(ss_id);
}
inline bool is_search_space_valid(search_space_id ss_id)
{
  return ss_id < MAX_NOF_SEARCH_SPACES;
}

/// SearchSpace configuration.
/// \remark See TS 38.331, "SearchSpace".
struct search_space_configuration {
  /// SearchSpace Type.
  enum class type { common, ue_dedicated };
  /// SearchSpace Common Type DCI Formats.
  struct common_dci_format {
    bool f0_0_and_f1_0;
    bool f2_0;
    bool f2_1;
    bool f2_2;
    bool f2_3;
  };
  /// SearchSpace UE-specific DCI formats.
  enum class ue_specific_dci_format { f0_0_and_f1_0, f0_1_and_1_1 };

  search_space_id id;
  coreset_id      cs_id;
  /// Periodicity of slots for PDCCH monitoring. Possible values: {1, 2, 4, 5, 8, 10, 16, 20, 40, 80, 160, 320, 640,
  /// 1280, 2560}.
  unsigned monitoring_slot_period;
  /// Offset of slot for PDCCH monitoring. Possible values: {0, ..., monitoring_slot_period}.
  unsigned monitoring_slot_offset;
  /// Number of consecutive slots that a SearchSpace lasts in every occasion. Values: (1..2559).
  unsigned duration;
  /// The first symbol(s) for PDCCH monitoring in the slots for PDCCH monitoring. The most significant bit represents
  /// the first OFDM in a slot.
  optional<std::bitset<NOF_OFDM_SYM_PER_SLOT_NORMAL_CP>> monitoring_symbols_within_slot;
  /// Number of PDCCH candidates per aggregation level. The aggregation level for the array element with index "x"
  /// is L=1U << x. The possible values for each element are {0, 1, 2, 3, 4, 5, 6, 8}.
  std::array<uint8_t, 5> nof_candidates;
  type                   type;
  union {
    common_dci_format      common;
    ue_specific_dci_format ue_specific;
  };
};

/// \remark See TS 38.331, "PDCCH-ConfigCommon"
struct pdcch_config_common {
  /// Contains Coreset#0.
  optional<coreset_configuration> coreset0;
  /// Contains common Coreset.
  optional<coreset_configuration> common_coreset;
  /// Contains SearchSpaceZero and commonSearchSpaceList. Size: (0..4).
  std::vector<search_space_configuration> search_spaces;
  search_space_id                         sib1_search_space_id;
  search_space_id                         other_si_search_space_id;
  search_space_id                         paging_search_space_id;
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
  bool               cp_extended;
  subcarrier_spacing scs;
  /// Common RBs where the BWP is located. CRB=0 overlaps with pointA.
  crb_interval crbs;

  bool operator==(const bwp_configuration& other) const
  {
    return scs == other.scs and crbs == other.crbs and cp_extended == other.cp_extended;
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
  unsigned          k0;
  sch_mapping_type  map_type;
  ofdm_symbol_range symbols;
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

/// \remark See TS 38.331, RACH-ConfigGeneric.
struct rach_config_generic {
  /// Values: {0,...,255}.
  uint8_t prach_config_index;
  /// Msg2 RAR window length in #slots. Network configures a value < 10msec. Values: (1, 2, 4, 8, 10, 20, 40, 80).
  unsigned ra_resp_window;
  /// Number of PRACH occasions FDMed in one time instance as per TS38.211, clause 6.3.3.2.
  unsigned msg1_fdm;
  /// Offset of lowest PRACH transmission occasion in frequency domain respective to PRB 0,
  /// as per TS38.211, clause 6.3.3.2. Possible values: {0,...,MAX_NOF_PRB - 1}.
  unsigned msg1_frequency_start;
  /// Zero-correlation zone configuration number as per TS38.331 "zeroCorrelationZoneConfig", used to derive N_{CS}.
  uint16_t zero_correlation_zone_config;
};

/// Used to specify the cell-specific random-access parameters as per TS38.331, "RACH-ConfigCommon".
struct rach_config_common {
  rach_config_generic rach_cfg_generic;
  /// Total number of prambles used for contention based and contention free RA. Values: (1..64).
  unsigned total_nof_ra_preambles;
  /// PRACH root sequence index. Values: (1..839).
  /// \remark See TS 38.211, clause 6.3.3.1.
  bool     prach_root_seq_index_l839_present;
  unsigned prach_root_seq_index;
  /// \brief Subcarrier spacing of PRACH as per TS38.331, "RACH-ConfigCommon". If invalid, the UE applies the SCS as
  /// derived from the prach-ConfigurationIndex in RACH-ConfigGeneric as per TS38.211 Tables 6.3.3.1-[1-3].
  subcarrier_spacing    msg1_scs;
  restricted_set_config restricted_set;
  /// Enables the transform precoder for Msg3 transmission according to clause 6.1.3 of TS 38.214.
  bool msg3_transform_precoder;
};

struct pusch_time_domain_resource_allocation {
  /// Values: (0..32).
  unsigned         k2;
  sch_mapping_type map_type;
  /// OFDM symbol boundaries for PUSCH. Network configures the fields so it does not cross the slot boundary.
  ofdm_symbol_range symbols;
};

/// \remark See TS 38.331, "PUSCH-ConfigCommon".
struct pusch_config_common {
  /// PUSCH time domain resource allocations. Size: (0..maxNrofUL-Allocations=16).
  std::vector<pusch_time_domain_resource_allocation> pusch_td_alloc_list;
};

/// \remark See TS 38.331, "PUCCH-ConfigCommon".
struct pucch_config_common {
  enum class pucch_group_hop_opt { neither, disabled, enable };
  /// Values: {0,...,15}.
  uint8_t             pucch_resource_common;
  pucch_group_hop_opt pucch_group_hopping;
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
};

/// \brief This class provides basic parameters of a downlink carrier and transmission.
/// \remark See TS 38.331, "FrequencyInfoDL-SIB".
struct frequency_info_dl {
  /// Represents the offset to Point A, as defined in TS 38.211, clause 4.4.4.2. Values: (0..2199).
  unsigned offset_to_point_a;
  /// Set of carriers for different subcarrier spacings. The network configures this for all SCSs that are used in
  /// DL BWPs in this serving cell. Size: (1..maxSCSs=5).
  std::vector<scs_specific_carrier> scs_carrier_list;
};

/// \brief Downlink Configuration, common to the serving cell.
/// \remark See TS 38.331, "DownlinkConfigCommonSIB".
struct dl_config_common {
  frequency_info_dl   freq_info_dl;
  bwp_downlink_common init_dl_bwp;
};

struct frequency_info_ul {
  /// Absolute frequency (in ARFCN) of the CRB0.
  unsigned absolute_freq_point_a;
  /// Set of carriers for different subcarrier spacings. The network configures this for all SCSs that are used in
  /// UL BWPs in this serving cell. Size: (1..maxSCSs=5).
  std::vector<scs_specific_carrier> scs_carrier_list;
  bool                              freq_shift_7p5khz_present;
  // TODO: Add other fields.
};

/// \brief Uplink Configuration, common to the serving cell.
/// \remark See TS 38.331, "UplinkConfigCommonSIB".
struct ul_config_common {
  frequency_info_ul freq_info_ul;
  bwp_uplink_common init_ul_bwp;
};

/////////////////////////////////     "Dedicated" FIELDS    /////////////////////////////////////

/// Used to configure the dedicated UE-specific parameters of an UL BWP.
/// \remark See TS 38.331, BWP-UplinkDedicated.
struct bwp_uplink_dedicated {
  pucch_config pucch_cnf;
};

} // namespace srsgnb
