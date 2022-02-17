#ifndef SRSGNB_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_H
#define SRSGNB_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include <complex>
#include <memory>

namespace srsgnb {

enum class modulation_scheme {
  BPSK   = 1, /*!< \brief BPSK. */
  QPSK   = 2, /*!< \brief QPSK. */
  QAM16  = 4, /*!< \brief QAM16. */
  QAM64  = 6, /*!< \brief QAM64. */
  QAM256 = 8  /*!< \brief QAM256. */
};

class modulation_mapper
{
public:
  virtual ~modulation_mapper() = default;

  virtual void modulate(span<const uint8_t> input, span<cf_t> symbols, modulation_scheme scheme) = 0;
};

std::unique_ptr<modulation_mapper> create_modulation_mapper();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_H
