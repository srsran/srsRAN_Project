/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
/// \brief Helper functions for PUCCH Format 3 and PUCCH Format 4.

#pragma once

#include "srsran/adt/bounded_integer.h"
#include "srsran/phy/generic_functions/transform_precoding/transform_precoder.h"
#include "srsran/phy/support/mask_types.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/upper/channel_estimation.h"
#include "srsran/phy/upper/equalization/modular_ch_est_list.h"
#include "srsran/ran/pucch/pucch_constants.h"

namespace srsran {

/// \brief DM-RS symbol mask for PUCCH Format 3 and PUCCH Format 4.
///
/// \param[in] nof_symbols       Number of symbols assigned to PUCCH resource.
/// \param[in] frequency_hopping Set to \c true if intra-slot frequency hopping is enabled for the PUCCH resource,
/// set to \c false otherwise.
/// \param[in] additional_dmrs   Whether \e additionalDMRS parameter is set for the PUCCH resource.
/// \returns The symbol mask for symbols containing DM-RS for that configuration, as per TS38.211 Table 6.4.1.3.3.2-1.
inline symbol_slot_mask get_pucch_formats3_4_dmrs_symbol_mask(
    bounded_integer<unsigned, pucch_constants::FORMAT3_MIN_NSYMB, pucch_constants::FORMAT3_MAX_NSYMB> nof_symbols,
    bool                                                                                              frequency_hopping,
    bool                                                                                              additional_dmrs)
{
  symbol_slot_mask mask(nof_symbols.value());

  switch (nof_symbols.value()) {
    case 4:
      if (frequency_hopping) {
        mask.set(0);
        mask.set(2);
      } else {
        mask.set(1);
      }
      break;
    case 5:
      mask.set(0);
      mask.set(3);
      break;
    case 6:
    case 7:
      mask.set(1);
      mask.set(4);
      break;
    case 8:
      mask.set(1);
      mask.set(5);
      break;
    case 9:
      mask.set(1);
      mask.set(6);
      break;
    case 10:
      if (additional_dmrs) {
        mask.set(1);
        mask.set(3);
        mask.set(6);
        mask.set(8);
      } else {
        mask.set(2);
        mask.set(7);
      }
      break;
    case 11:
      if (additional_dmrs) {
        mask.set(1);
        mask.set(3);
        mask.set(6);
        mask.set(9);
      } else {
        mask.set(2);
        mask.set(7);
      }
      break;
    case 12:
      if (additional_dmrs) {
        mask.set(1);
        mask.set(4);
        mask.set(7);
        mask.set(10);
      } else {
        mask.set(2);
        mask.set(8);
      }
      break;
    case 13:
      if (additional_dmrs) {
        mask.set(1);
        mask.set(4);
        mask.set(7);
        mask.set(11);
      } else {
        mask.set(2);
        mask.set(9);
      }
      break;
    case 14:
      if (additional_dmrs) {
        mask.set(1);
        mask.set(5);
        mask.set(8);
        mask.set(12);
      } else {
        mask.set(3);
        mask.set(10);
      }
      break;
  }

  return mask;
}

/// \brief Gets REs and channel estimates given a PUCCH Format 3/4 resource, performs equalization and reverts
/// transform precoding.
///
/// Extracts and loads the inner buffers with the PUCCH control data RE from the provided \c resource_grid, and their
/// corresponding channel estimates from \c channel_ests. The DM-RS RE are skipped. Equalization and transform
/// precoding reversion is done symbol by symbol.
///
/// \param[out] eq_re              Destination buffer for resource elements at the equalizer output.
/// \param[out] eq_noise_vars      Destination buffer for noise variances at the equalizer output.
/// \param[out] equalizer          Channel equalizer.
/// \param[out] precoder           Transform precoder.
/// \param[in]  grid               Input resource grid.
/// \param[in]  estimates          Channel estimates.
/// \param[in]  dmrs_symb_mask     The symbol mask for symbols containing DM-RS for the PUCCH configuration.
/// \param[in]  start_symbol_index Start symbol index within the slot {0, ..., 13}.
/// \param[in]  nof_symbols        Number of symbols assigned to PUCCH resource.
/// \param[in]  nof_prb            Number of PRB assigned to PUCCH resource.
/// \param[in]  first_prb          Lowest PRB index used for the PUCCH transmission within the resource grid.
/// \param[in]  second_hop_prb     Lowest PRB index used for the PUCCH transmission within the BWP {0, ..., 274}
///                                if intra-slot frequency hopping is enabled, empty otherwise.
/// \param[in]  rx_ports           Port indexes used for the PUCCH reception.
inline void pucch_3_4_extract_and_equalize(span<cf_t>                  eq_re,
                                           span<float>                 eq_noise_vars,
                                           channel_equalizer&          equalizer,
                                           transform_precoder&         precoder,
                                           const resource_grid_reader& grid,
                                           const channel_estimate&     estimates,
                                           const symbol_slot_mask&     dmrs_symb_mask,
                                           unsigned                    start_symbol_index,
                                           unsigned                    nof_symbols,
                                           unsigned                    nof_prb,
                                           unsigned                    first_prb,
                                           std::optional<unsigned>     second_hop_prb,
                                           span<const uint8_t>         rx_ports)
{
  // Number of receive antenna ports.
  auto nof_rx_ports = static_cast<unsigned>(rx_ports.size());

  // Number of REs per OFDM symbol.
  unsigned nof_re_symb = nof_prb * NRE;

  // Index of the first symbol allocated to the second hop, when intra-slot frequency hopping is enabled.
  unsigned second_hop_start = (nof_symbols / 2) + start_symbol_index;

  // Extract the Rx port noise variances from the channel estimation.
  std::array<float, MAX_PORTS> noise_var_estimates;
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    noise_var_estimates[i_port] = estimates.get_noise_variance(i_port);
  }

  for (unsigned i_symbol = start_symbol_index, i_symbol_end = start_symbol_index + nof_symbols;
       i_symbol != i_symbol_end;
       ++i_symbol) {
    // Skip DM-RS symbols.
    if (dmrs_symb_mask.test(i_symbol - start_symbol_index)) {
      continue;
    }

    // Calculate the lowest resource element containing the PUCCH resource within the OFDM symbol.
    unsigned first_subc = first_prb * NRE;
    if (second_hop_prb.has_value() && (i_symbol >= second_hop_start)) {
      // Intra-slot frequency hopping.
      first_subc = *second_hop_prb * NRE;
    }

    // Create modular buffers to hold the spans for this symbol.
    modular_re_buffer_reader<cbf16_t, MAX_PORTS> re_symb(nof_rx_ports, nof_re_symb);
    modular_ch_est_list<MAX_PORTS>               estimates_symb(nof_re_symb, nof_rx_ports, pucch_constants::MAX_LAYERS);

    for (unsigned i_port = 0, i_port_end = nof_rx_ports; i_port != i_port_end; ++i_port) {
      // Extract data RE from the resource grid.
      unsigned            i_port_grid  = rx_ports[i_port];
      span<const cbf16_t> re_symb_view = grid.get_view(i_port_grid, i_symbol).subspan(first_subc, nof_re_symb);
      re_symb.set_slice(i_port, re_symb_view);

      // Extract estimates from the estimates buffer.
      span<const cbf16_t> estimates_symb_view =
          estimates.get_symbol_ch_estimate(i_symbol, i_port).subspan(first_subc, nof_re_symb);
      estimates_symb.set_channel(estimates_symb_view, i_port, 0);
    }

    // Get a view of the equalized RE buffer for a single symbol.
    span<cf_t> eq_re_symb = eq_re.first(nof_re_symb);

    // Get a view of the equalized RE buffer for a single symbol.
    span<float> eq_noise_vars_symb = eq_noise_vars.first(nof_re_symb);

    // Equalize the data RE for a single symbol.
    equalizer.equalize(eq_re_symb,
                       eq_noise_vars_symb,
                       re_symb,
                       estimates_symb,
                       span<float>(noise_var_estimates).first(nof_rx_ports),
                       1.0F);

    // Revert transform precoding for a single symbol.
    precoder.deprecode_ofdm_symbol(eq_re_symb, eq_re_symb);
    precoder.deprecode_ofdm_symbol_noise(eq_noise_vars_symb, eq_noise_vars_symb);

    // Advance the equalized RE and noise vars views.
    eq_re         = eq_re.last(eq_re.size() - nof_re_symb);
    eq_noise_vars = eq_noise_vars.last(eq_noise_vars.size() - nof_re_symb);
  }
}

} // namespace srsran
