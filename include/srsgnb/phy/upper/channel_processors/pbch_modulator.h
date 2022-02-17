#ifndef  SRSGNB_PHY_UPPER_PBCH_MODULATOR_H
#define  SRSGNB_PHY_UPPER_PBCH_MODULATOR_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include <array>

namespace srsgnb {

class pbch_modulator
{
public:
  /**
   * @brief Number of bits to modulate
   */
  static const unsigned M_bit = 864;

  /**
   * @brief Resultant number of symbols after modulation
   */
  static const unsigned M_symb = M_bit / 2;

  virtual ~pbch_modulator() = default;

  /**
   * @brief Modulates a PBCH message according to TS 38.211 section 7.3.3 Physical broadcast channel
   * @param[in] bits Input bits of M_bit size
   * @param[out] ouput Modulated symbols of M_symb size
   */
  virtual void modulate(span<uint8_t>& bits, span<cf_t> symbols) = 0;
};

} // namespace srsgnb

#endif //  SRSGNB_PHY_UPPER_PBCH_MODULATOR_H
