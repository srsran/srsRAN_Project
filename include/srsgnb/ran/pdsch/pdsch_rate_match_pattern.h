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

#include "srsgnb/adt/optional.h"
#include "srsgnb/phy/constants.h"
#include <cstdint>
#include <new>

namespace srsgnb {

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
  struct symbols_in_resource_blocks {
    /// \brief Type of symbols in resource block.
    enum class symbols_in_resource_blocks_type { one_slot, two_slot };

    symbols_in_resource_blocks_type type;
    union {
      bounded_bitset<14> one_slot;
      bounded_bitset<28> two_slot;
    };

    bool operator==(const symbols_in_resource_blocks& rhs) const
    {
      bool ret = type == rhs.type;
      switch (type) {
        case symbols_in_resource_blocks_type::one_slot:
          return ret && one_slot == rhs.one_slot;
        case symbols_in_resource_blocks_type::two_slot:
          return ret && two_slot == rhs.two_slot;
      }
      return false;
    }
    bool operator!=(const symbols_in_resource_blocks& rhs) const { return !(rhs == *this); }
  };

  /// A time domain repetition pattern at which the pattern defined by symbolsInResourceBlock and resourceBlocks
  /// recurs. This slot pattern repeats itself continuously. Absence of this field indicates the value n1, i.e., the
  /// symbolsInResourceBlock recurs every 14 symbols.
  /// \remark See TS 38.214, clause 5.1.4.1.
  struct periodicity_and_pattern {
    /// \brief Type of periodicity and pattern.
    enum class periodicity_and_pattern_type { n2, n4, n5, n8, n10, n20, n40 };

    periodicity_and_pattern_type type;
    union {
      bounded_bitset<2>  n2;
      bounded_bitset<4>  n4;
      bounded_bitset<5>  n5;
      bounded_bitset<8>  n8;
      bounded_bitset<10> n10;
      bounded_bitset<20> n20;
      bounded_bitset<40> n40;
    };

    bool operator==(const periodicity_and_pattern& rhs) const
    {
      bool ret = type == rhs.type;
      switch (type) {
        case periodicity_and_pattern_type::n2:
          return ret && n2 == rhs.n2;
        case periodicity_and_pattern_type::n4:
          return ret && n4 == rhs.n4;
        case periodicity_and_pattern_type::n5:
          return ret && n5 == rhs.n5;
        case periodicity_and_pattern_type::n8:
          return ret && n8 == rhs.n8;
        case periodicity_and_pattern_type::n10:
          return ret && n10 == rhs.n10;
        case periodicity_and_pattern_type::n20:
          return ret && n20 == rhs.n20;
        case periodicity_and_pattern_type::n40:
          return ret && n40 == rhs.n40;
      }
      return false;
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
    symbols_in_resource_blocks        sym_in_rb;
    optional<periodicity_and_pattern> repeat_pattern;

    bool operator==(const bitmaps& rhs) const
    {
      return rb == rhs.rb && sym_in_rb == rhs.sym_in_rb && repeat_pattern == rhs.repeat_pattern;
    }
    bool operator!=(const bitmaps& rhs) const { return !(rhs == *this); }
  };

  /// \brief Rate matching pattern type.
  enum class pattern_type { bitmaps, coreset };

  rate_match_pattern_id_t pattern_id;
  pattern_type            type;
  union {
    bitmaps bmaps;
    /// This ControlResourceSet is used as a PDSCH rate matching pattern, i.e., PDSCH reception rate matches around it.
    /// In frequency domain, the resource is determined by the frequency domain resource of the CORESET with the
    /// corresponding CORESET ID. Time domain resource is determined by the parameters of the associated search space of
    /// the CORESET.
    coreset_id cs_id;
  };
  /// The field is mandatory present if the RateMatchPattern is defined on cell level. The field is absent when the
  /// RateMatchPattern is defined on BWP level. If the RateMatchPattern is defined on BWP level, the UE applies the SCS
  /// of the BWP.
  optional<subcarrier_spacing> scs;

  // TODO: Remaining

  bool operator==(const rate_match_pattern& rhs) const
  {
    bool ret = pattern_id == rhs.pattern_id && type == rhs.type;
    switch (type) {
      case pattern_type::bitmaps:
        return ret && bmaps == rhs.bmaps;
      case pattern_type::coreset:
        return ret && scs == rhs.scs;
    }
    return false;
  }
  bool operator!=(const rate_match_pattern& rhs) const { return !(rhs == *this); }
};

} // namespace srsgnb
