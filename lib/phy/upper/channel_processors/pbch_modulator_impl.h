/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#ifndef SRSGNB_CHANNEL_PROCESSORS_PBCH_MODULATOR_IMPL_H_
#define SRSGNB_CHANNEL_PROCESSORS_PBCH_MODULATOR_IMPL_H_

#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsgnb/phy/upper/channel_processors/pbch_modulator.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsgnb {

class pbch_modulator_impl : public pbch_modulator
{
private:
  std::unique_ptr<modulation_mapper>       modulator;
  std::unique_ptr<pseudo_random_generator> scrambler;

  /// Implements TS 38.211 section 7.3.3.1 Scrambling
  /// \param [in] b Inputs bits to scramble
  /// \param [out] b_hat Output bits after scrambling
  /// \param [in] args PBCH modulator arguments
  void scramble(span<const uint8_t> b, span<uint8_t> b_hat, const args_t& args);

  /// Implements TS 38.211 section 7.3.3.2 Modulation
  /// \param [in] b_hat Inputs bits to scramble
  /// \param [out] d_pbch Output symbols
  void modulate(span<const uint8_t> b_hat, span<cf_t> d_pbch);

  /// Implements TS 38.211 section 7.3.3.3 Mapping to physical resources
  /// \param [in] d_pbch provides the symbols to map
  /// \param [in, out] grid is the destination resource grid
  /// \param [in] args PBCH modulator arguments
  void map(span<const cf_t> d_pbch, resource_grid_writer& grid, const args_t& args);

public:
  explicit pbch_modulator_impl() : modulator(create_modulation_mapper()), scrambler(create_pseudo_random()) {}

  ~pbch_modulator_impl() = default;

  void put(span<const uint8_t> bits, resource_grid_writer& grid, const args_t& args) override;
};

} // namespace srsgnb

#endif // SRSGNB_CHANNEL_PROCESSORS_PBCH_MODULATOR_IMPL_H_
