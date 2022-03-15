
#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_MODULATOR_IMPL_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_MODULATOR_IMPL_H

#include "srsgnb/phy/upper/channel_processors/pdsch_modulator.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/phy/cyclic_prefix.h"

namespace srsgnb {

/// Describes the PDSCH modulator generic implementation constructor configuration.
struct pdsch_modulator_config_t {
  /// Provides the modulation mapper. Ownership is transferred to the PDSCH modulator.
  std::unique_ptr<modulation_mapper> modulator;
  /// Provides the pseudo-random sequence generator. Ownership is transferred to the PDSCH modulator.
  std::unique_ptr<pseudo_random_generator> scrambler;
};

/// Describes a generic implementation of a PDSCH modulator, defined by TS 38.211 section 7.3.1
class pdsch_modulator_impl : public pdsch_modulator
{
private:
  std::unique_ptr<modulation_mapper>       modulator;
  std::unique_ptr<pseudo_random_generator> scrambler;

  /// \brief Generates the DMRS for PDSCH resource element pattern.
  ///
  /// \param[in] config Provides the PDSCH modulation configuration.
  /// \return A RE pattern that describes the position of the DMRS.
  static re_pattern generate_dmrs_pattern(const config_t& config);

  /// \brief Scrambles a codeword. Implements TS 38.211 section 7.3.1.1 Scrambling.
  ///
  /// \param[out] b_hat Output bits after scrambling.
  /// \param[in] b Inputs bits to scramble.
  /// \param[in] q Codeword index {0,1}.
  /// \param[in] config Provides the scrambler configuration.
  void scramble(span<uint8_t> b_hat, span<const uint8_t> b, unsigned q, const config_t& config);

  /// \brief Modulates a codeword. Implements TS 38.211 section 7.3.1.2 Modulation.
  ///
  /// \param[out] d_pdsch Output symbols.
  /// \param[in] b_hat Inputs bits to perform the modulation mapping.
  /// \param[in] modulation Indicates the modulation scheme (QPSK, 16QAM, ...).
  /// \param[in] scaling Indicates the signal scaling if the value is valid (not 0, NAN nor INF).
  void modulate(span<cf_t> d_pdsch, span<const uint8_t> b_hat, modulation_scheme modulation, float scaling);

  /// \brief Map codewords into layers. Implements TS 38.211 section 7.3.1.3 Layer mapping.
  ///
  /// \param[out] x_pdsch Layer mapping result destination.
  /// \param[in] d_pdsch Layer mapping codeword source.
  /// \note The number of layers and codewords is deduced from the parameters.
  void layer_map(static_vector<span<cf_t>, MAX_PORTS>& x_pdsch, static_vector<span<cf_t>, MAX_NOF_CODEWORDS> d_pdsch);

  /// \brief Map resource elements from the layer index \c layer into the physical resource grid for non-interleaved
  /// Type1 allocation.
  ///
  /// Implements TS 38.211 sections 7.3.1.4 Antenna port mapping, 7.3.1.5 Layer mapping, 7.3.1.5 Mapping to virtual
  /// resource blocks and 7.3.1.6 Mapping from virtual to physical resource blocks.
  ///
  /// \param[out] grid Provides the destination resource grid.
  /// \param[in] x_pdsch PDSCH resource elements that have been already mapped to layers.
  /// \note The number of layers and codewords is deduced from the parameters.
  void map_to_prb_type1_non_interleaved(resource_grid_writer&                grid,
                                        static_vector<span<cf_t>, MAX_PORTS> x_pdsch,
                                        const config_t&                      config);

  /// \brief Map resource elements into the physical resource grid of the given antenna ports for Type0 allocation and
  /// interleaved Type1 allocation.
  ///
  /// Implements TS 38.211 sections 7.3.1.4 Antenna port mapping, 7.3.1.5 Layer mapping, 7.3.1.5 Mapping to virtual
  /// resource blocks and 7.3.1.6 Mapping from virtual to physical resource blocks
  ///
  /// \param[out] x_pdsch Layer mapping result destination.
  /// \param[in] d_pdsch Layer mapping codeword source.
  /// \note The number of layers and codewords is deduced from the parameters.
  void
  map_to_prb_other(resource_grid_writer& grid, static_vector<span<cf_t>, MAX_PORTS> x_pdsch, const config_t& config);

  /// Temporal modulated data
  static_vector<static_vector<cf_t, MAX_CODEWORD_SIZE>, MAX_NOF_CODEWORDS>          temp_d;
  static_vector<static_vector<cf_t, MAX_RB * NRE * MAX_NSYMB_PER_SLOT>, MAX_PORTS> temp_x;

public:
  /// \brief Generic PDSCH modulator instance constructor.
  ///
  /// \param[in] args Provides the internal dependencies instances.
  pdsch_modulator_impl(pdsch_modulator_config_t& args) :
    modulator(std::move(args.modulator)), scrambler(std::move(args.scrambler))
  {
    // Do nothing.
  }

  // See interface for the documentation.
  void
  modulate(resource_grid_writer& grid, span<const span<const uint8_t> > codewords, const config_t& config) override;
};

/// \brief Creates a generic PDSCH modulator.
///
/// \param[in] args Provides the internal dependencies instances.
/// \return A unique pointer with the modulator.
std::unique_ptr<pdsch_modulator> create_pdsch_modulator(pdsch_modulator_config_t& config);

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_MODULATOR_IMPL_H
