#ifndef SRSGNB_PHY_CHANNEL_CODING_POLAR_CODE_H
#define SRSGNB_PHY_CHANNEL_CODING_POLAR_CODE_H

#include "srsgnb/adt/bit_buffer.h"
#include <cstdint>
#include <memory>

namespace srsgnb {

class polar_code
{
public:
  virtual ~polar_code()                           = default;
  virtual uint16_t             get_N() const      = 0;
  virtual uint16_t             get_K() const      = 0;
  virtual uint16_t             get_nPC() const    = 0;
  virtual span<const uint16_t> get_K_set() const  = 0;
  virtual span<const uint16_t> get_PC_set() const = 0;

  virtual void set(const uint16_t K, const uint16_t E, const uint8_t nMax) = 0;
};

std::unique_ptr<polar_code> create_polar_code();

} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_CODING_POLAR_CODE_H