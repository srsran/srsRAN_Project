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

#include "ldpc_rate_dematcher_impl.h"

namespace srsran {

class ldpc_rate_dematcher_neon_impl : public ldpc_rate_dematcher_impl
{
protected:
  void combine_softbits(span<log_likelihood_ratio>       out,
                        span<const log_likelihood_ratio> in0,
                        span<const log_likelihood_ratio> in1) const override;

  void deinterleave_qpsk(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const override;

  void deinterleave_qam16(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const override;

  void deinterleave_qam64(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const override;

  void deinterleave_qam256(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const override;
};

} // namespace srsran
