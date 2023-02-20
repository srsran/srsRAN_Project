/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/re_pattern.h"

namespace srsran {

/// Defines the DMRS for PDSCH and PUSCH types
class dmrs_type
{
public:
  /// DM-RS types set to the higher layer parameter \e dmrs-Type given by TS38.331 Section 6.3.2, Information Element
  /// \e DMRS-DownlinkConfig for downlink and \e DMRS-UplinkConfig for uplink.
  enum options { TYPE1 = 1, TYPE2 };

  /// Default dmrs_type as normal.
  constexpr dmrs_type() = default;

  /// Construct from value. It allows implicit conversion from a DMRS type option.
  constexpr dmrs_type(options opt) : value(opt) {}

  /// Construct from another DMRS type.
  constexpr dmrs_type(const dmrs_type& other) : value(other.value) {}

  /// Comparison to value.
  constexpr bool operator==(options opt) const { return value == opt; }
  constexpr bool operator!=(options opt) const { return value != opt; }

  /// Comparison to other instance.
  constexpr bool operator==(const dmrs_type& other) const { return value == other.value; }
  constexpr bool operator!=(const dmrs_type& other) const { return value != other.value; }

  /// \brief Calculates the number of resource elements that carry DMRS for PDSCH or PUSCH per resource block and
  /// symbol.
  ///
  /// \param[in] type Provides the type of DMRS.
  /// \return The number of resource elements occupied by DMRS in a resource block and symbol.
  constexpr unsigned nof_dmrs_per_rb() const { return (value == dmrs_type::TYPE1) ? 6 : 4; }

  /// Defines the maximum number of ports for DMRS type 1.
  static constexpr unsigned DMRS_MAX_PORTS_TYPE1 = 8;

  /// Defines the maximum number of ports for DMRS type 2.
  static constexpr unsigned DMRS_MAX_PORTS_TYPE2 = 12;

  /// Provides the maximum number of ports supported by the DMRS type.
  constexpr unsigned dmrs_max_ports_type() const
  {
    return (value == TYPE1) ? DMRS_MAX_PORTS_TYPE1 : DMRS_MAX_PORTS_TYPE2;
  }

  /// \brief Gets a DMRS transmission pattern.
  ///
  /// It creates a RE pattern that describes the reserved resource elements for DM-RS in PDSCH or PUSCH transmissions.
  ///
  /// \param[in] bwp_start_rb                Indicates the start of the BWP.
  /// \param[in] bwp_size_rb                 Indicates the size of the BWP.
  /// \param[in] nof_cdm_groups_without_data Indicates the number of CDM groups without data for the transmission.
  /// \param[in] symbol_mask                 Indicates, with a mask, the symbols carrying DM-RS.
  /// \return An RE pattern describing the reserved elements for DMRS.
  re_pattern get_dmrs_pattern(unsigned         bwp_start_rb,
                              unsigned         bwp_size_rb,
                              unsigned         nof_cdm_groups_without_data,
                              symbol_slot_mask symbol_mask) const
  {
    // Temporal pattern.
    re_pattern dmrs_pattern;

    // Validate symbol mask size.
    srsgnb_assert(symbol_mask.size() <= dmrs_pattern.symbols.size(),
                  "The symbol mask size {} exceeds the maximum size {}.",
                  symbol_mask.size(),
                  dmrs_pattern.symbols.size());

    // Generate DMRS pattern.
    dmrs_pattern.rb_begin  = bwp_start_rb;
    dmrs_pattern.rb_end    = bwp_start_rb + bwp_size_rb;
    dmrs_pattern.rb_stride = 1;
    if (value == TYPE1) {
      bounded_bitset<2> cdm_groups_pattern(2);
      cdm_groups_pattern.fill(0, nof_cdm_groups_without_data);
      bounded_bitset<6> groups(6);
      groups.fill(0, 6, true);
      dmrs_pattern.re_mask = groups.kronecker_product<2>(cdm_groups_pattern);
    } else {
      bounded_bitset<6> cdm_groups_pattern(6);
      cdm_groups_pattern.fill(0, 2 * nof_cdm_groups_without_data);
      bounded_bitset<2> groups(2);
      groups.fill(0, 2, true);
      dmrs_pattern.re_mask = groups.kronecker_product<6>(cdm_groups_pattern);
    }
    dmrs_pattern.symbols = symbol_mask;

    return dmrs_pattern;
  }

private:
  /// Indicates the DM-RS type.
  options value = TYPE1;
};

/// Defines the maximum number of ports the DM-RS can be mapped to.
static constexpr unsigned DMRS_MAX_NPORTS =
    std::max(dmrs_type(dmrs_type::TYPE1).dmrs_max_ports_type(), dmrs_type(dmrs_type::TYPE2).dmrs_max_ports_type());

} // namespace srsran
