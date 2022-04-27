
#ifndef SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_FORMAT1_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_FORMAT1_IMPL_H

#include "srsgnb/phy/upper/signal_processors/pucch_orthogonal_sequence.h"
#include "srsgnb/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include "srsgnb/phy/upper/channel_processors/pucch_helper.h"

namespace srsgnb {

/// Describes a generic implementation of a DMRS for PUCCH processor.
class dmrs_pucch_processor_format1_impl : public dmrs_pucch_processor
{
private:
  /// Format 1 implementation expects pre-generated sequence collection on instantiation.
  const low_papr_sequence_collection* sequence_collection;

  /// Pre-generated orthogonal cover code.
  const pucch_orthogonal_sequence* occ;

  // Internal struct holding sequence generation parameters
  struct seq_generation_config {
    // sequence group
    unsigned u;
    // sequence number
    unsigned v;
    // sequential number of processed DMRS symbol
    unsigned m;
    // total number of DMRS symbols
    unsigned n_pucch;
  };

  /// \brief Implements TS 38.211 section 6.4.1.3.1.1 Sequence generation.
  ///
  /// \param[out] sequence     Provides the sequence destination.
  /// \param[in]  pucch_config Provides dmrs pucch config.
  /// \param[in]  gen_config   Aggregates additional parameters required to calculate the sequences.
  /// \param[in]  symbol       Denotes the symbol index.
  void sequence_generation(span<srsgnb::cf_t>                    sequence,
                           const dmrs_pucch_processor::config_t& pucch_config,
                           const seq_generation_config&          gen_config,
                           unsigned                              symbol) const;

  /// \brief Implements TS 38.211 section 6.4.1.3.1.2 Mapping to physical resources.
  ///
  /// \param[out] grid     Provides the grid destination to map the signal.
  /// \param[in] sequence  Provides the generated sequence for the given symbol.
  /// \param[in] start_prb Provides a PRB index for the given symbol.
  /// \param[in] symbol    Denotes the symbol index.
  void mapping(resource_grid_writer& grid, span<const cf_t> sequence, unsigned start_prb, unsigned symbol) const;

public:
  dmrs_pucch_processor_format1_impl(const low_papr_sequence_collection* c, const pucch_orthogonal_sequence* occ) :
    sequence_collection(c),
    occ(occ)
  {}

  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_FORMAT1_IMPL_H
