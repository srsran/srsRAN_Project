#ifndef SRSGNB_PHY_PDSCH_PROCESSOR_H
#define SRSGNB_PHY_PDSCH_PROCESSOR_H

#include "srsgnb/adt/byte_buffer.h"
#include <complex>

namespace srsgnb {

class pdsch_processor
{
public:
  virtual bool process(const byte_buffer& tb) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_PDSCH_PROCESSOR_H
