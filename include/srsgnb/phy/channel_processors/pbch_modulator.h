#ifndef SRSGNB_PHY_PBCH_MODULATOR_H
#define SRSGNB_PHY_PBCH_MODULATOR_H

#include "srsgnb/adt/span.h"
#include <array>

namespace srsgnb {

class pbch_modulator
{
public:
  virtual ~pbch_modulator() = default;

  /**
   * @brief Modulates a PBCH message
   * @param[in] bits Input bits
   * @param[out] ouput Modulated symbols
   */
  virtual void modulate(std::array<uint8_t, 864>& bits, std::array<uint8_t, 432>& symbols) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_PBCH_MODULATOR_H
