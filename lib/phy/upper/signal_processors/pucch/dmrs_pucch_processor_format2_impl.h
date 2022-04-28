
#ifndef SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_FORMAT2_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_FORMAT2_IMPL_H

#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pucch_processor.h"

namespace srsgnb {

/// Describes a generic implementation of a DMRS for PDCCH processor.
class dmrs_pucch_processor_format2_impl : public dmrs_pucch_processor
{
private:
  /// Pseudo-random sequence generator instance.
  std::unique_ptr<pseudo_random_generator> prg = create_pseudo_random();

  /// \brief Computes the initial pseudo-random state.
  /// \param[in] symbol Denotes the symbol index.
  /// \param[in] config Provides the required parameters.
  /// \return The initial pseudo-random state.
  static unsigned c_init(unsigned symbol, const config_t& config);

  /// \brief Implements TS 38.211 section 6.4.1.3.2.1 Sequence generation.
  ///
  /// \param[out] sequence Provides the sequence destination.
  /// \param[in] symbol    Denotes the symbol index.
  /// \param[in] config    Provides the required parameters to calculate the sequences.
  void sequence_generation(span<const cf_t> sequence, unsigned symbol, const config_t& config) const;

  /// \brief Implements TS 38.211 section 6.4.1.3.2.2 Mapping to physical resources.
  ///
  /// \param[out] grid Provides the grid destination to map the signal.
  /// \param[in]  sequence Provides the generated sequence for the given symbol.
  /// \param[in]  symbol Denotes the symbol index.
  /// \param[in]  config Provides the required fields to map the signal.
  void mapping(resource_grid_writer& grid,
               span<const cf_t>      sequence,
               unsigned              symbol,
               const config_t&       config);

public:
  void estimate(channel_estimate &estimate, const resource_grid_reader& grid, const config_t& config) override;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_FORMAT2_IMPL_H
