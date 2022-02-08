/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CHANNEL_CODING_CRC_CALCULATOR_IMPL_H_
#define SRSGNB_CHANNEL_CODING_CRC_CALCULATOR_IMPL_H_

#include "srsgnb/phy/channel_coding/crc_calculator.h"
#include <array>
#include <cstdint>
#include <map>

namespace srsgnb {

class crc_calculator_impl : public crc_calculator
{
private:
  struct crc_table_s : public std::array<uint64_t, 256> {
    unsigned order;
    uint64_t crcmask;
    uint64_t polynom;

    crc_table_s(unsigned poly, unsigned order_);
  };

  // CRC tables por the different polynomials
  static const std::map<crc_generator_poly, crc_table_s> crc_tables;

  // Attributes
  const crc_table_s& table;
  const unsigned     order;
  const uint64_t     crcmask;
  uint64_t           crc;

  // Methods
  void                      reset();
  void                      put_byte(unsigned char byte);
  crc_calculator_checksum_t get_checksum() const;
  void                      reversecrcbit(unsigned nbits);

public:
  explicit crc_calculator_impl(crc_generator_poly poly_);

  crc_calculator_checksum_t calculate_byte(const byte_buffer& input) override;
  crc_calculator_checksum_t calculate_bit(const bit_buffer& input) override;
};

} // namespace srsgnb
#endif // SRSGNB_CHANNEL_CODING_CRC_CALCULATOR_IMPL_H_
