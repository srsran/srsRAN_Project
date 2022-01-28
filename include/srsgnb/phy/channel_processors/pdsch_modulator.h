#ifndef SRSGNB_PHY_PDSCH_MODULATOR_H
#define SRSGNB_PHY_PDSCH_MODULATOR_H

#include "srsgnb/adt/byte_buffer.h"
#include <complex>

namespace srsgnb {

class pdsch_modulator
{
public:
  virtual std::vector<std::complex<float> > modulate(const byte_buffer& input) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_PDSCH_MODULATOR_H
