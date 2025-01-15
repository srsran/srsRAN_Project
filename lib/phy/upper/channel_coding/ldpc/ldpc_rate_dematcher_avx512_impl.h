/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ldpc_rate_dematcher_impl.h"

namespace srsran {

/// \brief LDPC Rate dematcher specialized with AVX512 instruction set.
///
/// It implements \ref ldpc_rate_dematcher interface. It relies on AVX512F, AVX512BW and AVX512VBMI instruction sets.
class ldpc_rate_dematcher_avx512_impl : public ldpc_rate_dematcher_impl
{
protected:
  // See interface for documentation.
  void combine_softbits(span<log_likelihood_ratio>       out,
                        span<const log_likelihood_ratio> in0,
                        span<const log_likelihood_ratio> in1) const override;

  // See interface for documentation.
  void deinterleave_qpsk(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const override;

  // See interface for documentation.
  void deinterleave_qam16(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const override;

  // See interface for documentation.
  void deinterleave_qam64(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const override;

  // See interface for documentation.
  void deinterleave_qam256(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const override;
};

} // namespace srsran
