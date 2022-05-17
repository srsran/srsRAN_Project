/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_FORMAT2_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_FORMAT2_IMPL_H

#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include "srsgnb/ran/pucch_mapping.h"

namespace srsgnb {

/// Describes a generic implementation of a DMRS for PDCCH processor.
class dmrs_pucch_processor_format2_impl : public dmrs_pucch_processor
{
private:
  /// Defines the DMRS index increment in frequency domain.
  static constexpr unsigned STRIDE = 3;

  /// Defines the number of DMRS per active resource block.
  static constexpr unsigned NOF_DMRS_PER_RB = NRE / STRIDE;

  /// Defines the maximum number of DMRS for PDCCH that can be found in a symbol.
  static constexpr unsigned MAX_NOF_DMRS_PER_SYMBOL = PUCCH_FORMAT2_MAX_NPRB * NOF_DMRS_PER_RB;

  /// Pseudo-random sequence generator instance.
  std::unique_ptr<pseudo_random_generator> prg = create_pseudo_random();

  /// \brief     Computes the initial pseudo-random state.
  /// \param[in] symbol Denotes the symbol index.
  /// \param[in] config Provides the required parameters.
  /// \return    The initial pseudo-random state.
  ///
  /// \remark implemented according to TS 38.211 section 6.4.1.3.2.1.
  static unsigned c_init(unsigned symbol, const config_t& config);

  /// \brief Implements TS 38.211 section 6.4.1.3.2.1 Sequence generation.
  ///
  /// \param[out] sequence  Provides the sequence destination.
  /// \param[in]  symbol    Denotes the symbol index.
  /// \param[in]  start_prb Denotes the index of first PRB allocated for DM-RS in the current symbol.
  /// \param[in]  config    Provides the required parameters to calculate the sequences.
  void sequence_generation(span<cf_t> sequence, unsigned symbol, unsigned start_prb, const config_t& config);

  /// \brief Implements TS 38.211 section 6.4.1.3.2.2 Mapping to physical resources.
  ///
  /// \param[out] ce        Container for storing extracted slot symbol DMRS pilots.
  /// \param[in]  grid      Provides the source grid to read the signal.
  /// \param[in]  start_prb Provides a PRB index for the given symbol.
  /// \param[in]  nof_prb   Provides number of PRBs allocated for DM-RS pilots.
  /// \param[in]  symbol    Denotes the symbol index.
  void
  mapping(span<cf_t> ce, const resource_grid_reader& grid, unsigned start_prb, unsigned nof_prb, unsigned symbol) const;

public:
  void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const config_t& config) override;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_FORMAT2_IMPL_H
