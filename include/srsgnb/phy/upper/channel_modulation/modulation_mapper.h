#ifndef SRSGNB_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_H
#define SRSGNB_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/modulation_scheme.h"
#include <complex>
#include <memory>

namespace srsgnb {

class modulation_mapper
{
public:
  virtual ~modulation_mapper() = default;

  virtual void modulate(span<const uint8_t> input, span<cf_t> symbols, modulation_scheme scheme) = 0;
};

std::unique_ptr<modulation_mapper> create_modulation_mapper();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_H
