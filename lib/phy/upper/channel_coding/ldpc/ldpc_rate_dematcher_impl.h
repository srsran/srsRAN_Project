/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief LDPC rate dematcher declaration.

#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_RATE_DEMATCHER_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_RATE_DEMATCHER_IMPL_H

#include "ldpc_graph_impl.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_rate_dematcher.h"

namespace srsgnb {

/// \brief LDPC rate dematching implementation.
///
/// It reverts the rate matching procedure described in TS38.212 Section 5.4.2.
class ldpc_rate_dematcher_impl : public ldpc_rate_dematcher
{
public:
  void rate_dematch(span<int8_t> output, span<const int8_t> input, const config_t& cfg) override;

private:
  /// Initializes the rate dematcher internal state.
  void init(const config_t& cfg);

  /// Allots LLRs from the rate-matched input sequence to the full-sized output codeblock (i.e., reverts bit selection).
  void allot_llrs(span<int8_t> out, span<const int8_t> in) const;

  /// Reverts the bit interleaving procedure.
  void deinterleave_llrs(span<int8_t> out, span<const int8_t> in) const;

  // Data members.
  /// Auxiliary buffer.
  std::array<int8_t, ldpc::MAX_CODEBLOCK_LENGTH> auxiliary_buffer{};
  /// Redundancy version, values in {0, 1, 2, 3}.
  unsigned rv = 0;
  /// Modulation scheme.
  unsigned modulation_order = 1;
  /// Buffer length.
  unsigned buffer_length = 0;
  /// Shift \f$ k_0 \f$ as defined in TS38.212 Table 5.4.2.1-2.
  unsigned shift_k0 = 0;
  /// Number of systematic bits (including filler bits) in the codeblock.
  unsigned nof_systematic_bits = 0;
  /// Number of filler bits.
  unsigned nof_filler_bits = 0;
  /// New data flag: true if the processed codeblock is not a repeated one.
  bool is_new_data = true;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_RATE_DEMATCHER_IMPL_H
