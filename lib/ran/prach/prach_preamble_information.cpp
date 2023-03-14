/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/ran/frame_types.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

prach_preamble_information srsran::get_prach_preamble_long_info(prach_format_type format)
{
  srsran_assert(is_long_preamble(format), "Invalid preamble format. It must be a long preamble.");
  switch (format) {
    case prach_format_type::zero:
      return {839U,
              prach_subcarrier_spacing::kHz1_25,
              phy_time_unit::from_units_of_kappa(24576),
              phy_time_unit::from_units_of_kappa(3168),
              true};
    case prach_format_type::one:
      return {839U,
              prach_subcarrier_spacing::kHz1_25,
              phy_time_unit::from_units_of_kappa(2 * 24576),
              phy_time_unit::from_units_of_kappa(21024),
              true};
    case prach_format_type::two:
      return {839U,
              prach_subcarrier_spacing::kHz1_25,
              phy_time_unit::from_units_of_kappa(4 * 24576),
              phy_time_unit::from_units_of_kappa(4688),
              true};
    case prach_format_type::three:
    default:
      return {839U,
              prach_subcarrier_spacing::kHz5,
              phy_time_unit::from_units_of_kappa(4 * 6144),
              phy_time_unit::from_units_of_kappa(3168),
              true};
  }
}

prach_preamble_information srsran::get_prach_preamble_short_info(prach_format_type        format,
                                                                 prach_subcarrier_spacing ra_scs)
{
  srsran_assert(is_short_preamble(format), "Invalid preamble format. It must be a long preamble.");
  return {};
}

prach_symbols_slots_duration srsran::get_prach_duration_info(const prach_configuration& prach_cfg,
                                                             subcarrier_spacing         pusch_scs)
{
  srsran_assert(pusch_scs == subcarrier_spacing::kHz15 or pusch_scs == subcarrier_spacing::kHz30,
                "Only SCS 15kHz and 30kHz supported");

  prach_symbols_slots_duration output{};

  // With SCS 15kHz and 30kHz, only normal CP is supported.
  static const unsigned nof_symbols_per_slot = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
  static const double   symbol_duration_msec =
      (double)SUBFRAME_DURATION_MSEC / (double)(get_nof_slots_per_subframe(pusch_scs) * nof_symbols_per_slot);

  // Derive PRACH subcarrier spacing and other parameters.
  const prach_preamble_information info = get_prach_preamble_long_info(prach_cfg.format);

  const double length_msecs = (info.cp_length.to_seconds() + info.symbol_length.to_seconds()) * 1000;
  output.nof_symbols        = ceil(length_msecs / symbol_duration_msec);
  // Map the starting symbol with from the SCS 15kHz FR1 reference for PRACH into PUSCH SCS.
  const unsigned start_symbol_pusch_scs_in_subframe =
      prach_cfg.starting_symbol * (1U << to_numerology_value(pusch_scs));
  // Start slot within the subframe. For FR1, the start_symbol_pusch_scs_in_subframe refers to the SCS 15kHz.
  output.start_slot_pusch_scs   = start_symbol_pusch_scs_in_subframe / nof_symbols_per_slot;
  output.start_symbol_pusch_scs = start_symbol_pusch_scs_in_subframe % nof_symbols_per_slot;
  output.prach_length_slots =
      static_cast<unsigned>(ceil(static_cast<double>(output.start_symbol_pusch_scs + output.nof_symbols) /
                                 (static_cast<double>(nof_symbols_per_slot))));

  return output;
}