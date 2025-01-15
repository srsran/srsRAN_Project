/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
