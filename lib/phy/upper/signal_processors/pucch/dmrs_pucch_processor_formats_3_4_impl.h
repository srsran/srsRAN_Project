/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/pucch_helper.h"
#include "srsran/phy/upper/sequence_generators/low_papr_sequence_generator.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include "srsran/phy/upper/signal_processors/port_channel_estimator.h"

namespace srsran {

/// Generic implementation of a DM-RS channel estimator for PUCCH Formats 3 and 4.
class dmrs_pucch_processor_formats_3_4_impl : public dmrs_pucch_processor
{
public:
  using layer_dmrs_pattern = port_channel_estimator::layer_dmrs_pattern;

  dmrs_pucch_processor_formats_3_4_impl(std::unique_ptr<pseudo_random_generator>     prg_,
                                        std::unique_ptr<low_papr_sequence_generator> low_papr_generator_,
                                        std::unique_ptr<port_channel_estimator>      ch_est_) :
    helper(std::move(prg_)), low_papr_generator(std::move(low_papr_generator_)), ch_estimator(std::move(ch_est_))
  {
    srsran_assert(low_papr_generator, "Invalid sequence generator.");
    srsran_assert(ch_estimator, "Invalid port channel estimator.");
  }

  void estimate(channel_estimate& ce_, const resource_grid_reader& grid, const config_t& config) override;

private:
  pucch_helper helper;

  /// PUCCH Formats 3 and 4 implementation expects pre-generated sequence collection on instantiation.
  std::unique_ptr<low_papr_sequence_generator> low_papr_generator;
  /// Antenna port channel estimator.
  std::unique_ptr<port_channel_estimator> ch_estimator;

  /// Buffer for DM-RS symbols.
  dmrs_symbol_list temp_symbols;

  /// Holds DM-RS sequence generation parameters used in TS38.211 Section 6.4.1.3.3.1.
  struct sequence_generation_config {
    // Sequence group.
    unsigned u;
    // Sequence number.
    unsigned v;
  };

  /// \brief Generates a DM-RS sequence according to TS38.211 Section 6.4.1.3.2.1.
  ///
  /// \param[out] sequence   Sequence destination.
  /// \param[in]  symbol     Symbol index.
  /// \param[in]  gen_config Additional parameters required to calculate the sequences.
  /// \param[in]  config     Required parameters to calculate the sequences.
  void generate_sequence(span<cf_t>                        sequence,
                         const config_t&                   config,
                         const sequence_generation_config& gen_config,
                         unsigned                          symbol);

  /// \brief Generates the PUCCH DM-RS allocation pattern.
  ///
  /// Implements the PUCCH DM-RS mapping, as described in TS38.211 Section 6.4.1.3.3.2.
  /// \param[in] config Configuration parameters.
  /// \return The DM-RS allocation pattern.
  static layer_dmrs_pattern generate_dmrs_pattern(const config_t& config);
};
} // namespace srsran
