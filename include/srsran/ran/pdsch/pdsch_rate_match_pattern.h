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

#include "srsran/adt/optional.h"
#include "srsran/phy/constants.h"
#include <cstdint>

namespace srsran {

/// \brief Rate matching pattern id used to identify a Rate matching pattern configuration.
/// \remark See TS 38.331, "RateMatchPatternId" and "maxNrofRateMatchPatterns".
enum rate_match_pattern_id_t : uint8_t {
  MIN_RATE_MATCH_PATTERN_ID   = 0,
  MAX_RATE_MATCH_PATTERN_ID   = 3,
  MAX_NOF_RATE_MATCH_PATTERNS = 4
};

/// \brief Rate matching pattern for PDSCH.
/// \remark See TS 38.331, RateMatchPattern.
struct rate_match_pattern {
  /// A symbol level bitmap in time domain. It indicates with a bit set to true that the UE shall rate match
  /// around the corresponding symbol. This pattern recurs (in time domain) with the configured periodicityAndPattern.
  /// \remark See TS 38.214, clause 5.1.4.1. For ECP the first 12 bits represent the symbols within the slot and the
  /// last two bits within the bitstring are ignored by the UE.
  struct symbols_in_rb {
    using one_slot = bounded_bitset<14>;
    using two_slot = bounded_bitset<28>;

    /// \brief Type of symbols in resource block.
    enum class symbols_in_rb_type { one_slot, two_slot };

    symbols_in_rb_type          type;
    variant<one_slot, two_slot> symbols;

    bool operator==(const symbols_in_rb& rhs) const { return type == rhs.type && symbols == rhs.symbols; }
    bool operator!=(const symbols_in_rb& rhs) const { return !(rhs == *this); }
  };

  /// A time domain repetition pattern at which the pattern defined by symbolsInResourceBlock and resourceBlocks
  /// recurs. This slot pattern repeats itself continuously. Absence of this field indicates the value n1, i.e., the
  /// symbolsInResourceBlock recurs every 14 symbols.
  /// \remark See TS 38.214, clause 5.1.4.1.
  struct periodicity_and_pattern {
    using periodicity_and_pattern_n2  = bounded_bitset<2>;
    using periodicity_and_pattern_n4  = bounded_bitset<4>;
    using periodicity_and_pattern_n5  = bounded_bitset<5>;
    using periodicity_and_pattern_n8  = bounded_bitset<8>;
    using periodicity_and_pattern_n10 = bounded_bitset<10>;
    using periodicity_and_pattern_n20 = bounded_bitset<20>;
    using periodicity_and_pattern_n40 = bounded_bitset<40>;

    /// \brief Type of periodicity and pattern.
    enum class periodicity_and_pattern_type { n2, n4, n5, n8, n10, n20, n40 };

    periodicity_and_pattern_type type;
    variant<periodicity_and_pattern_n2,
            periodicity_and_pattern_n4,
            periodicity_and_pattern_n5,
            periodicity_and_pattern_n8,
            periodicity_and_pattern_n10,
            periodicity_and_pattern_n20,
            periodicity_and_pattern_n40>
        prd_and_patt;

    bool operator==(const periodicity_and_pattern& rhs) const
    {
      return type == rhs.type && prd_and_patt == rhs.prd_and_patt;
    }
    bool operator!=(const periodicity_and_pattern& rhs) const { return !(rhs == *this); }
  };

  /// Indicates rate matching pattern by a pair of bitmaps resourceBlocks and symbolsInResourceBlock to define
  /// the rate match pattern within one or two slots, and a third bitmap periodicityAndPattern to define the repetition
  /// pattern with which the pattern defined by the above bitmap pair occurs.
  struct bitmaps {
    /// A resource block level bitmap in the frequency domain. A bit in the bitmap set to 1 indicates that the UE shall
    /// apply rate matching in the corresponding resource block in accordance with the symbolsInResourceBlock bitmap. If
    /// used as cell-level rate matching pattern, the bitmap identifies "common resource blocks (CRB)". If used as
    /// BWP-level rate matching pattern, the bitmap identifies "physical resource blocks" inside the BWP. The first/
    /// leftmost bit corresponds to resource block 0, and so on.
    bounded_bitset<MAX_RB>            rb;
    symbols_in_rb                     sym_in_rb;
    optional<periodicity_and_pattern> repeat_pattern;

    bool operator==(const bitmaps& rhs) const
    {
      return rb == rhs.rb && sym_in_rb == rhs.sym_in_rb && repeat_pattern == rhs.repeat_pattern;
    }
    bool operator!=(const bitmaps& rhs) const { return !(rhs == *this); }
  };

  rate_match_pattern_id_t pattern_id;
  /// This ControlResourceSet is used as a PDSCH rate matching pattern, i.e., PDSCH reception rate matches around it.
  /// In frequency domain, the resource is determined by the frequency domain resource of the CORESET with the
  /// corresponding CORESET ID. Time domain resource is determined by the parameters of the associated search space of
  /// the CORESET.
  variant<bitmaps, coreset_id> pattern_type;
  /// The field is mandatory present if the RateMatchPattern is defined on cell level. The field is absent when the
  /// RateMatchPattern is defined on BWP level. If the RateMatchPattern is defined on BWP level, the UE applies the SCS
  /// of the BWP.
  optional<subcarrier_spacing> scs;

  // TODO: Remaining

  bool operator==(const rate_match_pattern& rhs) const
  {
    return pattern_id == rhs.pattern_id && pattern_type == rhs.pattern_type;
  }
  bool operator!=(const rate_match_pattern& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
