#ifndef SRSGNB_PHY_UPPER_PBCH_MODULATOR_H
#define SRSGNB_PHY_UPPER_PBCH_MODULATOR_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/resource_grid.h"
#include <memory>

namespace srsgnb {

class pbch_modulator
{
public:
  /// Defines the number of bits to modulate
  static constexpr unsigned M_bit = 864;

  /// Defines the number of symbols after modulation
  static constexpr unsigned M_symb = M_bit / 2;

  /// Describes the PBCH modulator arguments
  struct args_t {
    /// Physical cell identifier
    unsigned phys_cell_id;
    /// SS/PBCH block index
    unsigned ssb_idx;
    /// First subcarrier in the resource grid
    unsigned ssb_first_subcarrier;
    /// First symbol in slot resource grid
    unsigned ssb_first_symbol;
    /// PSS linear signal amplitude
    float amplitude;
  };

  /// Default destructor
  virtual ~pbch_modulator() = default;

  /// Modulates a PBCH message according to TS 38.211 section 7.3.3 Physical broadcast channel
  ///
  /// \param[in] bits Input bits of M_bit size
  /// \param[out] grid is the destination resource grid
  virtual void put(span<const uint8_t> bits, resource_grid_writer& grid, const args_t& args) = 0;
};

std::unique_ptr<pbch_modulator> create_pbch_modulator();

} // namespace srsgnb

#endif //  SRSGNB_PHY_UPPER_PBCH_MODULATOR_H
