#ifndef SRSGNB_PHY_CHANNEL_CODING_CRC_CALCULATOR_H
#define SRSGNB_PHY_CHANNEL_CODING_CRC_CALCULATOR_H

#include "srsgnb/adt/byte_buffer.h"
#include <memory>

namespace srsgnb {

enum class crc_generator_poly {
  CRC24A,
  CRC24B,
  CRC24C,
  CRC16,
  CRC11,
  CRC6,
  // etc...
};

// Checksum type
using crc_calculator_checksum_t = unsigned;

// Calculate CRC here, let the CRC attachment be done in a lower abstraction level function that uses this.
class crc_calculator
{
public:
  virtual crc_calculator_checksum_t calculate(const byte_buffer& data) = 0;
};

std::unique_ptr<crc_calculator> create_crc_calculator(crc_generator_poly poly);

} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_CODING_CRC_CALCULATOR_H