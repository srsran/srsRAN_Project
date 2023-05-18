/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "channel_precoder_impl.h"

using namespace srsran;

void channel_precoder_impl::apply_precoding(re_buffer_writer_view&                        output,
                                            const re_buffer_reader_view&                  input,
                                            const precoding_configuration::weight_matrix& precoding)
{
  // Number of RE in a single slice, i.e., port or layer.
  unsigned nof_re_slice = output.get_nof_re();
  unsigned nof_tx_ports = output.get_nof_slices();
  unsigned nof_layers   = input.get_nof_slices();

  srsran_assert(
      nof_re_slice == input.get_nof_re(),
      "The number of output RE per slice, i.e., {}, does not match the number of input RE per slice, i.e., {}.",
      nof_re_slice,
      input.get_nof_re());

  srsran_assert(nof_tx_ports >= nof_layers,
                "The number of TX ports, i.e., {}, must be equal to or larger than the number of layers, i.e., {}.",
                nof_tx_ports,
                nof_layers);

  srsran_assert(nof_tx_ports == precoding.get_dimension_size(weight_dims::port),
                "The number on TX ports on the output buffer, i.e., {}, does not match the number of ports of the "
                "precoding matrix, i.e., {}.",
                nof_tx_ports,
                precoding.get_dimension_size(weight_dims::port));

  srsran_assert(nof_layers == precoding.get_dimension_size(weight_dims::layer),
                "The number on layers on the input buffer, i.e., {}, does not match the number of layers of the "
                "precoding matrix, i.e., {}.",
                nof_layers,
                precoding.get_dimension_size(weight_dims::layer));

  for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
    // View of the output RE for a single antenna port.
    span<cf_t> port_re_view = output.get_slice(i_port);

    // View of the precoding weights applicable to a single antenna port, i.e., the coefficients applied to each
    // layer for the atenna port.
    span<const cf_t> port_weights_view = precoding.get_view<static_cast<unsigned>(weight_dims::port)>({i_port});

    // View of the input RE for the first layer.
    span<const cf_t> layer_re_view = input.get_slice(0);

    // Set the output RE to the contribution of the first layer.
    for (unsigned i_re = 0; i_re != nof_re_slice; ++i_re) {
      port_re_view[i_re] = layer_re_view[i_re] * port_weights_view[0];
    }

    // Accumulate the contributions of all other layers.
    for (unsigned i_layer = 1; i_layer != nof_layers; ++i_layer) {
      layer_re_view = input.get_slice(i_layer);
      for (unsigned i_re = 0; i_re != nof_re_slice; ++i_re) {
        port_re_view[i_re] += layer_re_view[i_re] * port_weights_view[i_layer];
      }
    }
  }
}

using namespace srsran;
