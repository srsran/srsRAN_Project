/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "channel_precoder_generic.h"

using namespace srsran;

void channel_precoder_generic::apply_precoding_port(span<srsran::cf_t>              port_re,
                                                    const srsran::re_buffer_reader& input_re,
                                                    span<const srsran::cf_t>        port_weights)
{
  unsigned nof_re     = input_re.get_nof_re();
  unsigned nof_layers = input_re.get_nof_slices();

  // View of the input RE for the first layer.
  span<const cf_t> layer_re_view = input_re.get_slice(0);

  // Set the output RE to the contribution of the first layer.
  for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
    port_re[i_re] = layer_re_view[i_re] * port_weights[0];
  }

  // Accumulate the contributions of all other layers.
  for (unsigned i_layer = 1; i_layer != nof_layers; ++i_layer) {
    layer_re_view = input_re.get_slice(i_layer);
    for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
      port_re[i_re] += layer_re_view[i_re] * port_weights[i_layer];
    }
  }
}