#ifndef SRSGNB_PHY_UPPER_DMRS_MAPPING_H
#define SRSGNB_PHY_UPPER_DMRS_MAPPING_H

#include "srsgnb/phy/upper/re_pattern.h"

namespace srsgnb {

/// Defines the DMRS for PDSCH and PUSCH types
class dmrs_type
{
public:
  /// DMRS types, as described in TS 38.331.
  enum options { TYPE1 = 0, TYPE2 };

  /// Default dmrs_type as normal.
  constexpr dmrs_type() = default;

  /// Construct from value. It allows implicit conversion from a DMRS type option.
  constexpr dmrs_type(options opt) : value(opt) {}

  /// Construct from another DMRS type.
  constexpr dmrs_type(const dmrs_type& other) : value(other.value) {}

  /// Comparison to value.
  constexpr bool operator==(options opt) const { return value == opt; }

  /// Comparison to other instance.
  constexpr bool operator==(const dmrs_type& other) const { return value == other.value; }

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
  /// It creates a RE pattern that describes the reserved resource elements for DMRS in PDSCH or PUSCH transmissions.
  ///
  /// \param[in] bwp_start_rb                Indicates the start of the BWP.
  /// \param[in] bwp_size_rb                 Indicates the size of the BWP.
  /// \param[in] nof_cdm_groups_without_data Indicates the number of CDM groups without data for the transmission.
  /// \param[in] symbol_mask                 Indicates, with a mask, the symbols carrying DMRS.
  /// \return An RE pattern describing the reserved elements for DMRS.
  re_pattern get_dmrs_pattern(unsigned         bwp_start_rb,
                              unsigned         bwp_size_rb,
                              unsigned         nof_cdm_groups_without_data,
                              span<const bool> symbol_mask) const
  {
    // Temporal pattern.
    re_pattern dmrs_pattern;

    // Validate symbol mask size.
    srsran_assert(symbol_mask.size() <= dmrs_pattern.symbols.size(),
                  "The symbol mask size {} exceeds the maximum size {}.",
                  symbol_mask.size(),
                  dmrs_pattern.symbols.size());

    // Generate DMRS pattern.
    dmrs_pattern.rb_begin  = bwp_start_rb;
    dmrs_pattern.rb_end    = bwp_start_rb + bwp_size_rb;
    dmrs_pattern.rb_stride = 1;
    if (value == TYPE1) {
      for (unsigned re_idx = 0; re_idx != NRE; ++re_idx) {
        dmrs_pattern.re_mask[re_idx] = ((re_idx % 2) < nof_cdm_groups_without_data);
      }
    } else {
      for (unsigned re_idx = 0; re_idx != NRE; ++re_idx) {
        dmrs_pattern.re_mask[re_idx] = (((re_idx / 2) % 3) < nof_cdm_groups_without_data);
      }
    }
    std::copy(symbol_mask.begin(), symbol_mask.end(), dmrs_pattern.symbols.begin());

    return dmrs_pattern;
  }

private:
  /// Indicates the cyclic prefix type.
  options value = TYPE1;
};

/// Defines the maximum number of ports DMRS can be mapped to.
static constexpr unsigned DMRS_MAX_NPORTS =
    std::max(dmrs_type(dmrs_type::TYPE1).dmrs_max_ports_type(), dmrs_type(dmrs_type::TYPE2).dmrs_max_ports_type());

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_DMRS_MAPPING_H