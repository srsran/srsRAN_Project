/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "channel_equalizer_test_data.h"
#include "srsgnb/phy/upper/equalization/equalization_factories.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

static constexpr float MAX_ERROR_EQ = 1e-6;

void read_symbols(re_measurement<cf_t>& symbols, const re_measurement_exploded& syms_expl)
{
  re_measurement_dimensions re_dims;
  re_dims.nof_subc    = syms_expl.nof_prb * NRE;
  re_dims.nof_symbols = syms_expl.nof_symbols;
  re_dims.nof_slices  = syms_expl.nof_slices;

  symbols.resize(re_dims);
  const std::vector<cf_t> all_syms     = syms_expl.measurements.read();
  unsigned                slice_length = re_dims.nof_symbols * re_dims.nof_subc;
  TESTASSERT_EQ(slice_length * re_dims.nof_slices, all_syms.size(), "Wrong number of symbols.");

  span<const cf_t> all_syms_span(all_syms);
  for (unsigned i_slice = 0, skip = 0; i_slice != re_dims.nof_slices; ++i_slice) {
    symbols.set_slice(all_syms_span.subspan(skip, slice_length), i_slice);
    skip += slice_length;
  }
}

void read_ch_estimates(channel_estimate& ch_est, const ch_estimates_exploded& ch_est_expl)
{
  channel_estimate::channel_estimate_dimensions ch_dims;
  ch_dims.nof_prb       = ch_est_expl.nof_prb;
  ch_dims.nof_symbols   = ch_est_expl.nof_symbols;
  ch_dims.nof_rx_ports  = ch_est_expl.nof_rx_ports;
  ch_dims.nof_tx_layers = ch_est_expl.nof_tx_layers;

  ch_est.resize(ch_dims);
  std::vector<cf_t> all_ests     = ch_est_expl.estimates.read();
  unsigned          slice_length = ch_dims.nof_symbols * ch_dims.nof_prb * NRE;
  unsigned          nof_paths    = ch_dims.nof_tx_layers * ch_dims.nof_rx_ports;
  TESTASSERT_EQ(slice_length * nof_paths, all_ests.size(), "Wrong number of channel estimates.");

  span<const cf_t> all_ests_span(all_ests);
  unsigned         skip = 0;
  for (unsigned i_tx = 0; i_tx != ch_dims.nof_tx_layers; ++i_tx) {
    for (unsigned i_rx = 0; i_rx != ch_dims.nof_rx_ports; ++i_rx) {
      span<cf_t> path = ch_est.get_path_ch_estimate(i_rx, i_tx);
      srsvec::copy(path, all_ests_span.subspan(skip, slice_length));
      skip += slice_length;

      ch_est.set_noise_variance(ch_est_expl.noise_var, i_rx, i_tx);
    }
  }
}

int main()
{
  dynamic_re_measurement<cf_t> test_rx_symbols({MAX_RB * NRE, MAX_NSYMB_PER_SLOT, MAX_PORTS});
  dynamic_re_measurement<cf_t> test_eq_symbols_expected(
      {MAX_RB * NRE, MAX_NSYMB_PER_SLOT, pusch_constants::MAX_NOF_LAYERS});
  dynamic_re_measurement<cf_t> test_eq_symbols_actual(
      {MAX_RB * NRE, MAX_NSYMB_PER_SLOT, pusch_constants::MAX_NOF_LAYERS});
  dynamic_re_measurement<float> test_noise_vars({MAX_RB * NRE, MAX_NSYMB_PER_SLOT, pusch_constants::MAX_NOF_LAYERS});
  channel_estimate              test_ch_estimate;

  dynamic_re_measurement<float> eq_symbols_error({MAX_RB * NRE, MAX_NSYMB_PER_SLOT, pusch_constants::MAX_NOF_LAYERS});

  // Create channel equalizer.
  std::shared_ptr<channel_equalizer_factory> equalizer_factory = create_channel_equalizer_zf_impl_factory();
  std::unique_ptr<channel_equalizer>         test_equalizer    = equalizer_factory->create();

  for (const auto& t_case : channel_equalizer_test_data) {
    // For now, check only Zero Forcing equalizer.
    if (t_case.equalizer_type == "ZF") {
      read_symbols(test_rx_symbols, t_case.received_symbols);
      read_symbols(test_eq_symbols_expected, t_case.equalized_symbols);
      read_ch_estimates(test_ch_estimate, t_case.ch_estimates);
      test_noise_vars.resize(test_eq_symbols_expected.size());
      test_eq_symbols_actual.resize(test_eq_symbols_expected.size());
      eq_symbols_error.resize(test_eq_symbols_expected.size());

      test_equalizer->equalize(
          test_eq_symbols_actual, test_noise_vars, test_rx_symbols, test_ch_estimate, t_case.scaling);

      // Compute Error.
      for (unsigned i_layer = 0; i_layer != t_case.ch_estimates.nof_tx_layers; ++i_layer) {
        for (unsigned i_symbol = 0; i_symbol != t_case.ch_estimates.nof_symbols; ++i_symbol) {
          span<const cf_t> eq_subcs_actual   = test_eq_symbols_actual.get_symbol(i_symbol, i_layer);
          span<const cf_t> eq_subcs_expected = test_eq_symbols_expected.get_symbol(i_symbol, i_layer);
          span<float>      eq_subcs_error    = eq_symbols_error.get_symbol(i_symbol, i_layer);
          for (unsigned i_subc = 0; i_subc != t_case.ch_estimates.nof_prb * NRE; ++i_subc) {
            eq_subcs_error[i_subc] = std::abs(eq_subcs_expected[i_subc] - eq_subcs_actual[i_subc]);
          }
        }
      }

      // Assert Error with respect to the golden equalized symbols.
      for (unsigned i_layer = 0; i_layer != t_case.ch_estimates.nof_tx_layers; ++i_layer) {
        for (unsigned i_symbol = 0; i_symbol != t_case.ch_estimates.nof_symbols; ++i_symbol) {
          span<const float> eq_subcs_error = eq_symbols_error.get_symbol(i_symbol, i_layer);
          for (unsigned i_subc = 0; i_subc != t_case.ch_estimates.nof_prb * NRE; ++i_subc) {
            TESTASSERT(eq_subcs_error[i_subc] <= MAX_ERROR_EQ, "Equalizer error too large!");
          }
        }
      }
    }
  }
}
