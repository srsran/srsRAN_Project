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

#include "amplitude_controller_scaling_impl.h"
#include "srsran/srsvec/sc_prod.h"

using namespace srsran;

amplitude_controller_metrics amplitude_controller_scaling_impl::process(span<cf_t> output, span<const cf_t> input)
{
  srsran_srsvec_assert_size(output, input);

  // Apply scaling factor.
  srsvec::sc_prod(input, amplitude_gain, output);

  // Return empty metrics.
  return {0.0F, 0.0F, 0.0F, 0.0F, 0UL, 0UL, 0.0, false};
}
