
#ifndef SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PDCCH_PROCESSOR_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PDCCH_PROCESSOR_IMPL_H

#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdcch_processor.h"

namespace srsgnb {

/// Describes a generic implementation of a DMRS for PDCCH processor.
class dmrs_pdcch_processor_impl : public dmrs_pdcch_processor
{
private:
  /// Defines the DMRS index increment in frequency domain.
  static constexpr unsigned STRIDE = 4;

  /// Defines the DMRS index offset in a resource block.
  static constexpr unsigned OFFSET = 1;

  /// Defines the number of DMRS per active resource block.
  static constexpr unsigned NOF_DMRS_PER_RB = NRE / STRIDE;

  /// Defines the maximum number of DMRS for PDCCH that can be found in a symbol.
  static constexpr unsigned MAX_NOF_DMRS_PER_SYMBOL = MAX_RB * NOF_DMRS_PER_RB;

  /// Pseudo-random sequence generator instance.
  std::unique_ptr<pseudo_random_generator> prg = create_pseudo_random();

  /// \brief Computes the initial pseudo-random state.
  /// \param[in] symbol Denotes the symbol index.
  /// \param[in] config Provides the required parameters.
  /// \return The initial pseudo-random state.
  static unsigned c_init(unsigned symbol, const config_t& config);

  /// \brief Implements TS 38.211 section 7.4.1.3.1 Sequence generation.
  ///
  /// This method generates the sequence described in TS 38.211 section 7.4.1.3.1, considering the only values required
  /// to fill the resource blocks according to TS 38.211 section 7.3.2.
  ///
  /// \param[out] sequence Provides the sequence destination.
  /// \param[in] symbol Denotes the symbol index.
  /// \param[in] config Provides the required parameters to calculate the sequences.
  void sequence_generation(span<cf_t> sequence, unsigned symbol, const config_t& config) const;

  /// \brief Implements TS 38.211 section 7.4.1.3.2 Mapping to physical resources.
  ///
  /// This method implements the signal mapping as described in TS 38.211 section 7.4.1.3.2. In addition, it applies
  /// precoding if configured.
  ///
  /// \param[out] grid Provides the grid destination to map the signal.
  /// \param[in] sequence Provides the generated sequence for the given symbol.
  /// \param[in] mask Provides a base subcarrier allocation mask for the given symbol.
  /// \param[in] symbol Denotes the symbol index.
  /// \param[in] config Provides the required fields to map the signal.
  void mapping(resource_grid_writer& grid,
               span<const cf_t>      sequence,
               span<const bool>      mask,
               unsigned              symbol,
               const config_t&       config);

public:
  void map(resource_grid_writer& grid, const config_t& config) override;
};

/// Creates a generic DMRS for PDCCH instance.
std::unique_ptr<dmrs_pdcch_processor> create_dmrs_pdcch_processor();

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PDCCH_PROCESSOR_IMPL_H
