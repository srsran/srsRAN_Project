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

/// \file
/// \brief LDPC rate dematcher declaration.

#pragma once

#include "ldpc_graph_impl.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_rate_dematcher.h"

namespace srsran {

/// \brief LDPC rate dematching implementation.
///
/// It reverts the rate matching procedure described in TS38.212 Section 5.4.2.
class ldpc_rate_dematcher_impl : public ldpc_rate_dematcher
{
public:
  void rate_dematch(span<log_likelihood_ratio>       output,
                    span<const log_likelihood_ratio> input,
                    bool                             new_data,
                    const codeblock_metadata&        cfg) override;

protected:
  /// \brief Combines two sequences of soft bits by taking saturated addition.
  ///
  /// \param[out] out Resulting sequence of soft bits.
  /// \param[in] in0  First input sequence of soft bits.
  /// \param[in] in1  Second input sequence of soft bits.
  virtual void combine_softbits(span<log_likelihood_ratio>       out,
                                span<const log_likelihood_ratio> in0,
                                span<const log_likelihood_ratio> in1) const;

  /// \brief Deinterleaves a sequence of soft bits with a modulation order of two.
  ///
  /// \param[out] out Resulting sequence of soft bits.
  /// \param[in] in   Input sequence of soft bits.
  virtual void deinterleave_qpsk(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const;

  /// \brief Deinterleaves a sequence of soft bits with a modulation order of four.
  ///
  /// \param[out] out Resulting sequence of soft bits.
  /// \param[in] in   Input sequence of soft bits.
  virtual void deinterleave_qam16(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const;

  /// \brief Deinterleaves a sequence of soft bits with a modulation order of six.
  ///
  /// \param[out] out Resulting sequence of soft bits.
  /// \param[in] in   Input sequence of soft bits.
  virtual void deinterleave_qam64(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const;

  /// \brief Deinterleaves a sequence of soft bits with a modulation order of eight.
  ///
  /// \param[out] out Resulting sequence of soft bits.
  /// \param[in] in   Input sequence of soft bits.
  virtual void deinterleave_qam256(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const;

private:
  /// Initializes the rate dematcher internal state.
  void init(bool new_data, const codeblock_metadata::tb_common_metadata& cfg);

  /// Allots LLRs from the rate-matched input sequence to the full-sized output codeblock (i.e., reverts bit selection).
  void allot_llrs(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const;

  /// Reverts the bit interleaving procedure.
  void deinterleave_llrs(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const;

  // Data members.
  /// Auxiliary buffer.
  std::array<log_likelihood_ratio, ldpc::MAX_CODEBLOCK_RM_SIZE> auxiliary_buffer{};
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

} // namespace srsran
