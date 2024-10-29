/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Helper functions for PUCCH Format 3 and PUCCH Format 4.

#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/upper/pucch_formats_3_4_helpers.h"

namespace srsran {

inline void pucch_3_4_get_data_re_ests(re_buffer_writer<cbf16_t>&  ch_re,
                                       dynamic_ch_est_list&        ch_estimates,
                                       const resource_grid_reader& resource_grid,
                                       const channel_estimate&     channel_ests,
                                       symbol_slot_mask            dmrs_symb_mask,
                                       unsigned                    nof_rx_ports,
                                       unsigned                    nof_symbols,
                                       unsigned                    first_prb,
                                       unsigned                    nof_prb,
                                       std::optional<unsigned>     second_hop_prb,
                                       unsigned                    start_symbol_index)
{
  for (unsigned i_port = 0, i_port_end = nof_rx_ports; i_port != i_port_end; ++i_port) {
    // Get a view of the data RE destination buffer for a single Rx port.
    span<cbf16_t> re_port_buffer = ch_re.get_slice(i_port);

    // Get a view of the channel estimates destination buffer for a single Rx port and Tx layer.
    span<cbf16_t> ests_port_buffer = ch_estimates.get_channel(i_port, 0);

    // First OFDM subcarrier containing PUCCH Format 3.
    unsigned first_subc = first_prb * NRE;
    // Number of REs per OFDM symbol.
    unsigned nof_re_symb = nof_prb * NRE;
    // The number of symbols allocated to the first subcarrier when intra-slot frequency hopping is enabled.
    unsigned second_hop_start = nof_symbols / 2;
    for (unsigned i = 0; i < nof_symbols; i++) {
      // Skip DM-RS symbols.
      if (dmrs_symb_mask.test(i)) {
        continue;
      }

      if ((i >= second_hop_start) && second_hop_prb.has_value()) {
        // Intra-slot frequency hopping.
        first_subc = second_hop_prb.value() * NRE;
      }

      unsigned i_symbol = i + start_symbol_index;

      // Get a view of the data RE destination buffer for a single symbol.
      span<cbf16_t> re_symb_buffer = re_port_buffer.first(nof_re_symb);

      // Extract data RE from the resource grid.
      resource_grid.get(re_symb_buffer, i_port, i_symbol, first_subc);

      // Advance the RE destination buffer view.
      re_port_buffer = re_port_buffer.subspan(nof_re_symb, re_port_buffer.size() - nof_re_symb);

      // Get a view over the channel estimation coefficients for a single OFDM symbol.
      span<const cbf16_t> ests_symbol = channel_ests.get_symbol_ch_estimate(i_symbol, i_port);

      // Copy channel estimation coefficients for a single OFDM symbol.
      span<const cbf16_t> ests_pucch3 = ests_symbol.subspan(first_subc, nof_re_symb);
      std::copy(ests_pucch3.begin(), ests_pucch3.end(), ests_port_buffer.begin());

      // Advance the channel estimation coefficients buffer view.
      ests_port_buffer = ests_port_buffer.subspan(ests_pucch3.size(), ests_port_buffer.size() - ests_pucch3.size());
    }

    // Assert that all port data RE buffer elements have been filled.
    srsran_assert(re_port_buffer.empty(),
                  "Number of extracted port data RE does not match destination buffer dimensions: "
                  "{} unused elements.",
                  re_port_buffer.size());

    // Assert that all port channel estimates buffer elements have been filled.
    srsran_assert(
        ests_port_buffer.empty(),
        "Number of extracted port channel estimation coefficients does not match destination buffer dimensions: "
        "{} unused elements.",
        ests_port_buffer.size());
  }
}

} // namespace srsran
