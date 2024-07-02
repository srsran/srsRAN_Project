/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "channel_precoder_impl.h"

using namespace srsran;

void channel_precoder_impl::apply_precoding(re_buffer_writer<>&            output,
                                            const re_buffer_reader<>&      input,
                                            const precoding_weight_matrix& precoding) const
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

  srsran_assert(nof_tx_ports == precoding.get_nof_ports(),
                "The number on TX ports on the output buffer, i.e., {}, does not match the number of ports of the "
                "precoding matrix, i.e., {}.",
                nof_tx_ports,
                precoding.get_nof_ports());

  srsran_assert(nof_layers == precoding.get_nof_layers(),
                "The number on layers on the input buffer, i.e., {}, does not match the number of layers of the "
                "precoding matrix, i.e., {}.",
                nof_layers,
                precoding.get_nof_layers());

  for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
    // View of the output RE for a single antenna port.
    span<cf_t> port_re_view = output.get_slice(i_port);

    // View of the precoding weights applicable to a single antenna port, i.e., the coefficients applied to each
    // layer for the antenna port.
    span<const cf_t> port_weights_view = precoding.get_port_coefficients(i_port);

    // Apply precoding.
    apply_precoding_port(port_re_view, input, port_weights_view);
  }
}
