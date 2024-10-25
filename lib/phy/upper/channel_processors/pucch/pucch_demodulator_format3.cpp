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
/// \brief PUCCH Format 3 demodulator definition.

#include "pucch_demodulator_format3.h"
#include "srsran/phy/support/resource_grid_reader.h"

#include <srsran/phy/support/mask_types.h>

using namespace srsran;

// TODO: refactor to make this tables and the get_pucch3_symb_mask available in some header
/// \brief Control data RE allocation pattern for PUCCH Format 3.
///
/// Indicates the symbol positions containing control data within a PUCCH Format 3 resource
/// when the additionalDMRS flag is disabled, as per TS38.211 Section 6.4.1.3.3.2-1.
static const auto format3_symb_mask1 = to_array<symbol_slot_mask>({
    {false, true, true, false, true},                                                       // ^ (5 symbols)
    {true, false, true, true, false, true},                                                 // |
    {true, false, true, true, false, true, true},                                           // | constant section
    {true, false, true, true, true, false, true, true},                                     // |
    {true, false, true, true, true, true, false, true, true},                               // v (9 symbols)
    {true, true, false, true, true, true, true, false, true, true},                         // ^ (10 symbols)
    {true, true, false, true, true, true, true, false, true, true, true},                   // |
    {true, true, false, true, true, true, true, true, false, true, true, true},             // | variable section
    {true, true, false, true, true, true, true, true, true, false, true, true, true},       // |
    {true, true, true, false, true, true, true, true, true, true, false, true, true, true}, // v (14 symbols)
});

/// \brief Control data RE allocation pattern for PUCCH Format 3.
///
/// Indicates the symbol positions containing control data within a PUCCH Format 3 resource
/// when the additionalDMRS flag is enabled, as per TS38.211 Section 6.4.1.3.3.2-1.
static const auto format3_symb_mask2 = to_array<symbol_slot_mask>({
    {false, true, true, false, true},                                                         // ^ (5 symbols)
    {true, false, true, true, false, true},                                                   // |
    {true, false, true, true, false, true, true},                                             // | constant section
    {true, false, true, true, true, false, true, true},                                       // |
    {true, false, true, true, true, true, false, true, true},                                 // v (9 symbols)
    {true, false, true, false, true, true, false, true, false, true},                         // ^ (10 symbols)
    {true, false, true, false, true, true, false, true, true, false, true},                   // |
    {true, false, true, true, false, true, true, false, true, true, false, true},             // | variable section
    {true, false, true, true, false, true, true, false, true, true, true, false, true},       // |
    {true, false, true, true, true, false, true, true, false, true, true, true, false, true}, // v (14 symbols)
});

static symbol_slot_mask get_pucch3_symb_mask(pucch_demodulator::format3_configuration config)
{
  if (config.nof_symbols == 4) {
    if (config.second_hop_prb.has_value()) {
      return {false, true, false, true};
    }
    return {true, false, true, true};
  }

  if (config.additional_dmrs) {
    return format3_symb_mask2[config.nof_symbols - 5];
  }
  return format3_symb_mask1[config.nof_symbols - 5];
}

void pucch_demodulator_format3::demodulate(span<log_likelihood_ratio>                      llr,
                                           const resource_grid_reader&                     grid,
                                           const channel_estimate&                         estimates,
                                           const pucch_demodulator::format3_configuration& config)
{
  // Number of receive antenna ports.
  auto nof_rx_ports = static_cast<unsigned>(config.rx_ports.size());

  // Number of data Resource Elements in a slot for a single Rx port.
  symbol_slot_mask symb_mask   = get_pucch3_symb_mask(config);
  unsigned         nof_re_port = symb_mask.count() * config.nof_prb * NRE;

  // Assert that allocations are valid.
  srsran_assert(config.nof_prb && config.nof_prb <= pucch_constants::FORMAT3_MAX_NPRB,
                "Invalid Number of PRB allocated to PUCCH Format 3, i.e., {}. Valid range is 1 to {}.",
                config.nof_prb,
                pucch_constants::FORMAT3_MAX_NPRB);

  srsran_assert((config.first_prb + config.nof_prb) * NRE <= grid.get_nof_subc(),
                "PUCCH Format 3: PRB allocation outside grid (first hop). Requested [{}, {}), grid has {} PRBs.",
                config.first_prb,
                config.first_prb + config.nof_prb,
                grid.get_nof_subc() / NRE);

  srsran_assert(!config.second_hop_prb.has_value() ||
                    ((config.second_hop_prb.value() + config.nof_prb) * NRE <= grid.get_nof_subc()),
                "PUCCH Format 3: PRB allocation outside grid (second hop). Requested [{}, {}), grid has {} PRBs.",
                config.second_hop_prb.value(),
                config.second_hop_prb.value() + config.nof_prb,
                grid.get_nof_subc() / NRE);

  interval<unsigned, true> nof_symbols_range(pucch_constants::FORMAT3_MIN_NSYMB, pucch_constants::FORMAT3_MAX_NSYMB);
  srsran_assert(nof_symbols_range.contains(config.nof_symbols),
                "Invalid Number of OFDM symbols allocated to PUCCH Format 3, i.e., {}. Valid range is {}.",
                config.nof_symbols,
                nof_symbols_range);

  // Resize data and channel estimation buffers.
  ch_re.resize(nof_rx_ports, nof_re_port);
  ch_estimates.resize(nof_re_port, nof_rx_ports, SINGLE_TX_LAYER);

  // Resize equalized data and post equalization noise variance buffers.
  eq_re.resize(nof_re_port);
  eq_noise_vars.resize(nof_re_port);

  // Extract data RE and channel estimation coefficients.
  get_data_re_ests(grid, estimates, config);

  // Equalize
  // Transform precoder -> void deprecode_ofdm_symbol(span<cf_t> out, span<const cf_t> in)
  srsran_assertion_failure("PUCCH Format 3 not supported.");
}
void pucch_demodulator_format3::get_data_re_ests(const resource_grid_reader&                     resource_grid,
                                                 const channel_estimate&                         channel_ests,
                                                 const pucch_demodulator::format3_configuration& config)
{
  // Prepare RB mask. RB allocation is contiguous for PUCCH Format 3.
  prb_mask.resize(config.nof_prb);
  prb_mask.fill(0, config.nof_prb, true);

  // Get a mask for the symbols in the PUCCH Format 3 resource that contain data.
  symbol_slot_mask symb_mask = get_pucch3_symb_mask(config);

  for (unsigned i_port = 0, i_port_end = config.rx_ports.size(); i_port != i_port_end; ++i_port) {
    // Get a view of the data RE destination buffer for a single Rx port.
    span<cbf16_t> re_port_buffer = ch_re.get_slice(i_port);

    // Get a view of the channel estimates destination buffer for a single Rx port and Tx layer.
    span<cbf16_t> ests_port_buffer = ch_estimates.get_channel(i_port, 0);

    // First OFDM subcarrier containing PUCCH Format 3.
    unsigned first_subc = config.first_prb * NRE;

    // Number of REs per OFDM symbol
    unsigned nof_re_symb = config.nof_prb * NRE;

    for (unsigned i = 0; i < config.nof_symbols; i++) {
      // Skip DM-RS symbols.
      if (!symb_mask.test(i)) {
        continue;
      }

      unsigned i_symbol = i + config.start_symbol_index;

      /* No intra-slot frequency hopping yet.
      if ((i_symbol > config.start_symbol_index) && config.second_hop_prb.has_value()) {
        first_subc = config.second_hop_prb.value() * NRE;
      }
      */

      // Get a view of the data RE destination buffer for a single symbol.
      span<cbf16_t> re_symb_buffer = re_port_buffer.subspan(0, nof_re_symb);

      // Extract data RE from the resource grid.
      resource_grid.get(re_symb_buffer, i_port, i_symbol, first_subc);

      // Advance the RE destination buffer view.
      re_port_buffer = re_port_buffer.subspan(nof_re_symb, re_port_buffer.size());

      // Get a view over the channel estimation coefficients for a single OFDM symbol.
      span<const cbf16_t> ests_symbol = channel_ests.get_symbol_ch_estimate(i_symbol, i_port);

      // Copy channel estimation coefficients for a single OFDM symbol.
      span<const cbf16_t> ests_pucch3 = ests_symbol.subspan(first_subc, nof_re_symb);
      std::copy(ests_pucch3.begin(), ests_pucch3.end(), ests_port_buffer.begin());

      // Advance the channel estimation coefficients buffer view.
      ests_port_buffer = ests_port_buffer.subspan(ests_pucch3.size(), ests_port_buffer.size());
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
