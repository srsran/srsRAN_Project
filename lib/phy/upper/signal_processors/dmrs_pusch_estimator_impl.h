/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PUSCH channel estimator definition.

#pragma once

#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include "srsgnb/phy/upper/signal_processors/port_channel_estimator.h"

namespace srsgnb {

class dmrs_pusch_estimator_impl : public dmrs_pusch_estimator
{
public:
  using layer_dmrs_pattern = port_channel_estimator::layer_dmrs_pattern;

  /// Constructor - sets the channel estimator.
  explicit dmrs_pusch_estimator_impl(std::unique_ptr<pseudo_random_generator> prg_,
                                     std::unique_ptr<port_channel_estimator>  ch_est) :
    prg(std::move(prg_)), ch_estimator(std::move(ch_est))
  {
    srsgnb_assert(prg, "Invalid PRG.");
    srsgnb_assert(ch_estimator, "Invalid port channel estimator.");
  }

  // See interface for the documentation.
  void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const configuration& config) override;

private:
  /// Parameters for PUSCH DM-RS.
  struct parameters {
    std::array<bool, NRE> re_pattern;
    std::array<float, 2>  w_f;
    std::array<float, 2>  w_t;
  };

  /// Parameters for PUSCH DM-RS configuration type 1 as per TS 38.211 Table 6.4.1.1.3-1.
  static const std::array<parameters, 8> params_type1;

  /// Parameters for PUSCH DM-RS configuration type 2 as per TS 38.211 Table 6.4.1.1.3-2.
  static const std::array<parameters, 12> params_type2;

  /// Maximum supported number of transmission layers.
  static constexpr unsigned MAX_TX_LAYERS = pusch_constants::MAX_NOF_LAYERS;
  /// DMRS for PUSCH reference point \f$k\f$ relative to Point A.
  static constexpr unsigned DMRS_REF_POINT_K_TO_POINT_A = 0;

  /// Pseudo-random generator.
  std::unique_ptr<pseudo_random_generator> prg;
  /// Antenna port channel estimator.
  std::unique_ptr<port_channel_estimator> ch_estimator;
  /// Buffer for DM-RS symbols.
  dmrs_symbol_list temp_symbols;
  /// Buffer for DM-RS symbol coordinates.
  std::array<layer_dmrs_pattern, MAX_TX_LAYERS> temp_coordinates;

  /// \brief Generates the sequence described in TS 38.211 section 6.4.1.1.1, considering the only values required
  /// in TS38.211 section 6.4.1.1.2.
  ///
  /// \param[out] sequence Sequence destination.
  /// \param[in] symbol    Symbol index within the slot.
  /// \param[in] config    Configuration parameters.
  void sequence_generation(span<cf_t> sequence, unsigned symbol, const configuration& config) const;

  /// \brief Generates the PUSCH DM-RS symbols for one transmission port.
  ///
  /// Implements the PUSCH DM-RS generation, precoding and mapping procedures described in TS38.211 Section 6.4.1.1.
  /// \param[out] symbols Lists of generated DM-RS symbols, one per transmission layer.
  /// \param[out] pattern Lists of DM-RS patterns representing the REs the DM-RS symbols should be mapped to, one per
  ///                     transmission layer.
  /// \param[in]  cfg     Configuration parameters.
  void generate(dmrs_symbol_list& symbols, span<layer_dmrs_pattern> mask, const configuration& cfg);
};

} // namespace srsgnb
