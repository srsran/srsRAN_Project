
#ifndef SRSGNB_CHANNEL_PROCESSORS_PDCCH_MODULATOR_IMPL_H
#define SRSGNB_CHANNEL_PROCESSORS_PDCCH_MODULATOR_IMPL_H

#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_modulator.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsgnb {

/// Describes the PDCCH modulator generic implementation constructor configuration.
struct pdcch_modulator_config_t {
  /// Provides the modulation mapper. Ownership is transferred to the PDCCH modulator.
  std::unique_ptr<modulation_mapper> modulator;
  /// Provides the pseudo-random sequence generator. Ownership is transferred to the PDCCH modulator.
  std::unique_ptr<pseudo_random_generator> scrambler;
};

class pdcch_modulator_impl : public pdcch_modulator
{
private:
  /// Defines the maximum number of resource blocks used in a transmission. It considers the maximum aggregation level
  /// of 16 and a REG size of 6 RB.
  static constexpr unsigned MAX_RB_PER_PDCCH = 16 * 6;

  /// Defines the number of resource elements available for PDCCH in a resource block.
  static constexpr unsigned NOF_RE_PER_RB = NRE - 3;

  /// Defines the maximum number of resource elements that can be utilised for a PDCCH transmission.
  static constexpr unsigned MAX_RE = NOF_RE_PER_RB * MAX_RB_PER_PDCCH;

  /// Defines the maximum number of bits that can be utilised for a PDCCH transmission.
  static constexpr unsigned MAX_BITS = MAX_RE * static_cast<unsigned>(modulation_scheme::QPSK);

  /// Provides an implementation of the the pseudo-random generator.
  std::unique_ptr<pseudo_random_generator> scrambler;

  /// Provides an implementation of the modulator.
  std::unique_ptr<modulation_mapper> modulator;

  /// \brief Scrambles a codeword. Implements TS 38.211 section 7.3.2.3 Scrambling.
  ///
  /// \param[out] b_hat Output bits after scrambling.
  /// \param[in] b Inputs bits to scramble.
  /// \param[in] config Provides the scrambler configuration.
  void scramble(span<uint8_t> b_hat, span<const uint8_t> b, const config_t& config);

  /// \brief Modulates a codeword. Implements TS 38.211 section 7.3.2.4 PDCCH modulation.
  ///
  /// \param[out] d_pdcch Output symbols.
  /// \param[in] b_hat Inputs bits to perform the modulation mapping.
  /// \param[in] scaling Indicates the signal scaling if the value is valid (not 0, NAN nor INF).
  void modulate(span<cf_t> d_pdcch, span<const uint8_t> b_hat, float scaling);

  /// \brief Map to resource elements in the grid. Implements TS 38.211 section 7.3.2.5 Mapping to physical resources.
  ///
  /// \param[out] grid Provides the destination resource grid.
  /// \param[in] d_pdcch Provides the block of complex-valued symbols to map.
  /// \param[in] config Provides the mapper configuration.
  void map(resource_grid_writer& grid, span<const cf_t> d_pdcch, const config_t& config);

public:
  void modulate(resource_grid_writer& grid, span<const uint8_t> codewords, const config_t& config) override;

  /// \brief Generic PDSCH modulator instance contructor.
  ///
  /// \param[in] args Provides the internal dependencies instances.
  pdcch_modulator_impl(pdcch_modulator_config_t& config) :
    scrambler(std::move(config.scrambler)), modulator(std::move(config.modulator))
  {
    // Do nothing.
  }
};

std::unique_ptr<pdcch_modulator> create_pdcch_modulator(pdcch_modulator_config_t& config);

} // namespace srsgnb

#endif // SRSGNB_CHANNEL_PROCESSORS_PDCCH_MODULATOR_IMPL_H
