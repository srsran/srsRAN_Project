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
/// \brief PUSCH channel estimator definition.

#pragma once

#include "srsran/phy/upper/sequence_generators/low_papr_sequence_generator.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/channel_estimator/port_channel_estimator.h"
#include "srsran/phy/upper/signal_processors/pusch/dmrs_pusch_estimator.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

class dmrs_pusch_estimator_impl : public dmrs_pusch_estimator
{
public:
  using layer_dmrs_pattern = port_channel_estimator::layer_dmrs_pattern;

  /// Constructor - sets the channel estimator.
  explicit dmrs_pusch_estimator_impl(std::unique_ptr<pseudo_random_generator>     prg_,
                                     std::unique_ptr<low_papr_sequence_generator> tp_sequence_generator_,
                                     std::unique_ptr<port_channel_estimator>      ch_est,
                                     task_executor&                               executor_) :
    prg(std::move(prg_)),
    low_papr_sequence_gen(std::move(tp_sequence_generator_)),
    ch_estimator(std::move(ch_est)),
    executor(executor_)
  {
    srsran_assert(prg, "Invalid PRG.");
    srsran_assert(low_papr_sequence_gen, "Invalid sequence generator.");
    srsran_assert(ch_estimator, "Invalid port channel estimator.");
  }

  // See interface for the documentation.
  void estimate(channel_estimate&              estimate,
                dmrs_pusch_estimator_notifier& notifier,
                const resource_grid_reader&    grid,
                const configuration&           config) override;

private:
  /// Maximum supported number of transmission layers.
  static constexpr unsigned MAX_TX_LAYERS = pusch_constants::MAX_NOF_LAYERS;
  /// DMRS for PUSCH reference point \f$k\f$ relative to Point A.
  static constexpr unsigned DMRS_REF_POINT_K_TO_POINT_A = 0;

  /// Pseudo-random generator.
  std::unique_ptr<pseudo_random_generator> prg;
  /// Sequence generator for transform precoding.
  std::unique_ptr<low_papr_sequence_generator> low_papr_sequence_gen;
  /// Antenna port channel estimator.
  std::unique_ptr<port_channel_estimator> ch_estimator;
  /// Buffer for DM-RS symbols.
  dmrs_symbol_list temp_symbols;
  /// Buffer for DM-RS symbol coordinates.
  std::array<layer_dmrs_pattern, MAX_TX_LAYERS> temp_pattern;
  /// Configuration of the port channel estimator.
  port_channel_estimator::configuration est_cfg;
  /// Counter of ports still pending to be estimated.
  std::atomic<unsigned> pending_ports;
  /// Task executor for running the port channel estimator.
  task_executor& executor;

  /// \brief Generates the sequence described in TS38.211 Section 6.4.1.1.1, considering the only values required
  /// in TS38.211 Section 6.4.1.1.2.
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

} // namespace srsran
