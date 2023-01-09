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

#pragma once

#include "ldpc_graph_impl.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_rate_dematcher.h"

namespace srsgnb {

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

} // namespace srsgnb
