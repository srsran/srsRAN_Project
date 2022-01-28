#ifndef SRSGNB_PHY_CHANNEL_MODULATION_MODULATION_MAPPER_H
#define SRSGNB_PHY_CHANNEL_MODULATION_MODULATION_MAPPER_H

#include "srsgnb/adt/byte_buffer.h"
#include <complex>

namespace srsgnb {

enum class modulation_scheme {
  Pi2_BPSK,
  QPSK,
  // etc...
};

class modulation_mapper
{
public:
  virtual std::vector<std::complex<float> > modulate(const byte_buffer& input, modulation_scheme scheme) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_MODULATION_MODULATION_MAPPER_H
