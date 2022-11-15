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

namespace srsgnb {

/// \brief Position of first DM-RS for Downlink (see TS 38.211, clause 7.4.1.1.1) and Uplink (see TS 38.211,
/// clause 6.4.1.1.3).
enum class dmrs_typeA_position { pos2 = 2, pos3 = 3 };

inline unsigned to_symbol_index(dmrs_typeA_position pos)
{
  return static_cast<unsigned>(pos);
}

/// Type of DMRS to be used in DL and UL.
/// \remark See TS 38.331, DMRS-DownlinkConfig and DMRS-UplinkConfig.
enum class dmrs_config_type { type1 = 1, type2, not_set };

/// \brief Position for additional DM-RS in DL (see TS 38.211, Tables 7.4.1.1.2-3 and 7.4.1.1.2-4).
/// \remark See TS 38.331, DMRS-DownlinkConfig and DMRS-UplinkConfig.
enum class dmrs_additional_positions { pos0 = 0, pos1, pos2, pos3, not_set };

/// \brief The maximum number of OFDM symbols for DL front loaded DMRS. If set to len2, the UE determines the actual
/// number of DM-RS symbols by the associated DCI. (see TS 38.214, clause 7.4.1.1.2).
/// \remark See TS 38.214, DMRS-DownlinkConfig.
enum class dmrs_max_length { len1 = 1, len2, not_set };

/// Gets the number of resource elements occupied by DM-RS per PRB and symbol for a given type of DM-RS.
static constexpr unsigned get_nof_re_per_prb(dmrs_config_type type)
{
  return type == dmrs_config_type::type1 ? 6 : 4;
}

/// Gets the maximum number of CDM groups without data for a given type of DM-RS.
static constexpr unsigned get_max_nof_cdm_groups_without_data(dmrs_config_type type)
{
  return type == dmrs_config_type::type1 ? 2 : 3;
}

} // namespace srsgnb
