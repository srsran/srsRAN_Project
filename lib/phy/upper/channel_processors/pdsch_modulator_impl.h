/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/channel_processors/pdsch_modulator.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/ran/cyclic_prefix.h"

namespace srsgnb {

/// Describes a generic implementation of a PDSCH modulator, defined by TS 38.211 section 7.3.1
class pdsch_modulator_impl : public pdsch_modulator
{
private:
  std::unique_ptr<modulation_mapper>       modulator;
  std::unique_ptr<pseudo_random_generator> scrambler;

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

  /// \brief Maps codewords into layers. Implements TS 38.211 section 7.3.1.3 Layer mapping.
  ///
  /// \param[out] x_pdsch Layer mapping result destination.
  /// \param[in] d_pdsch Layer mapping codeword source.
  /// \note The number of layers and codewords is deduced from the parameters.
  void layer_map(static_vector<span<cf_t>, MAX_PORTS>& x_pdsch, static_vector<span<cf_t>, MAX_NOF_CODEWORDS> d_pdsch);

  /// \brief Maps contiguous resource elements from the layer index \c layer into the physical resource grid.
  ///
  /// Implements TS 38.211 sections 7.3.1.4 Antenna port mapping, 7.3.1.5 Layer mapping, 7.3.1.5 Mapping to virtual
  /// resource blocks and 7.3.1.6 Mapping from virtual to physical resource blocks.
  ///
  /// \param[out] grid Provides the destination resource grid.
  /// \param[in] x_pdsch PDSCH resource elements that have been already mapped to layers.
  /// \note The number of layers and codewords is deduced from the parameters.
  void map_to_contiguous_prb(resource_grid_writer&                grid,
                             static_vector<span<cf_t>, MAX_PORTS> x_pdsch,
                             const config_t&                      config);

  /// \brief Maps non-contiguous resource elements into the physical resource grid of the given antenna ports.
  ///
  /// Implements TS 38.211 sections 7.3.1.4 Antenna port mapping, 7.3.1.5 Layer mapping, 7.3.1.5 Mapping to virtual
  /// resource blocks and 7.3.1.6 Mapping from virtual to physical resource blocks
  ///
  /// \param[out] x_pdsch Layer mapping result destination.
  /// \param[in] d_pdsch Layer mapping codeword source.
  /// \note The number of layers and codewords is deduced from the parameters.
  void
  map_to_prb_other(resource_grid_writer& grid, static_vector<span<cf_t>, MAX_PORTS> x_pdsch, const config_t& config);

  /// Temporary modulated data.
  std::array<std::array<cf_t, MAX_CODEWORD_SIZE>, MAX_NOF_CODEWORDS>         temp_d;
  std::array<std::array<cf_t, MAX_RB * NRE * MAX_NSYMB_PER_SLOT>, MAX_PORTS> temp_x;

public:
  /// \brief Generic PDSCH modulator instance constructor.
  pdsch_modulator_impl(std::unique_ptr<modulation_mapper>       modulator_,
                       std::unique_ptr<pseudo_random_generator> scrambler_) :
    modulator(std::move(modulator_)), scrambler(std::move(scrambler_))
  {
    srsgnb_assert(modulator, "Invalid modulator");
    srsgnb_assert(scrambler, "Invalid scrambler");
  }

  // See interface for the documentation.
  void modulate(resource_grid_writer& grid, span<const span<const uint8_t>> codewords, const config_t& config) override;
};

} // namespace srsgnb
