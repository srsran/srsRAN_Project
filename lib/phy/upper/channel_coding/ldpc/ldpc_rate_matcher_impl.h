/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief LDPC rate matcher declaration.

#pragma once

#include "ldpc_graph_impl.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_rate_matcher.h"

namespace srsran {

/// LDPC rate matching implementation, as per TS38.212 Section 5.4.2.
class ldpc_rate_matcher_impl : public ldpc_rate_matcher
{
public:
  // See interface for the documentation.
  void rate_match(span<uint8_t>                                 output,
                  span<const uint8_t>                           input,
                  const codeblock_metadata::tb_common_metadata& cfg) override;

private:
  /// Initializes the rate matcher internal state.
  void init(const codeblock_metadata::tb_common_metadata& cfg);

  /// \brief Carries out bit selection, as per TS38.212 Section 5.4.2.1.
  ///
  /// \param[out] out Sequence of selected bits.
  /// \param[in]  in  Input codeblock.
  void select_bits(span<uint8_t> out, span<const uint8_t> in) const;

  /// \brief Carries out bit interleaving, as per TS38.212 Section 5.4.2.2.
  ///
  /// \param[out] out Sequence of interleaved bits.
  /// \param[in]  in  Sequence of selected bits (see ldpc_rate_matcher_impl::select_bits).
  void interleave_bits(span<uint8_t> out, span<const uint8_t> in) const;

  // Data members

  /// Bit selection circular buffer.
  span<const uint8_t> buffer = {};
  /// Auxiliary buffer.
  std::array<uint8_t, ldpc::MAX_CODEBLOCK_RM_SIZE> auxiliary_buffer = {};
  /// Redundancy version, values in {0, 1, 2, 3}.
  unsigned rv = 0;
  /// Modulation scheme.
  unsigned modulation_order = 1;
  /// Buffer length.
  unsigned buffer_length = 0;
  /// Shift \f$ k_0 \f$ as defined in TS38.212 Table 5.4.2.1-2
  unsigned shift_k0 = 0;
};

} // namespace srsran
