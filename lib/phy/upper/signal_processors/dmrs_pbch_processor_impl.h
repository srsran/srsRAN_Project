/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pbch_processor.h"

namespace srsgnb {

class dmrs_pbch_processor_impl : public dmrs_pbch_processor
{
private:
  /// DMRS PBCH Sequence length in the SSB
  static const unsigned NOF_RE = 144;

  std::unique_ptr<pseudo_random_generator> prg;

  /// \brief Computes the initial pseudo-random state
  /// \param [in] config provides the required parameters to calculate the value
  /// \return the initial pseudo-random state
  static unsigned c_init(const config_t& config);

  /// \brief Implements TS 38.211 section 7.4.1.4.1 Sequence generation
  /// \param [out] sequence provides the destination of the sequence generation
  /// \param [in] config provides the required fields to generate the signal
  void generation(std::array<cf_t, NOF_RE>& sequence, const config_t& config) const;

  /// \brief Implements TS 38.211 section 7.4.1.4.2 Mapping to physical resources
  /// \param [in] sequence provides the source of the sequence
  /// \param [out] grid provides the grid destination to map the signal
  /// \param [in] config provides the required fields to map the signal
  void mapping(const std::array<cf_t, NOF_RE>& sequence, resource_grid_writer& grid, const config_t& config) const;

public:
  dmrs_pbch_processor_impl(std::unique_ptr<pseudo_random_generator> pseudo_random_generator) :
    prg(std::move(pseudo_random_generator))
  {
    srsgnb_assert(prg, "Invalid PRG.");
  }

  // See interface for documentation.
  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace srsgnb
