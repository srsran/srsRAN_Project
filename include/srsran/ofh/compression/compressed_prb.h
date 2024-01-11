/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/bit_buffer.h"
#include "srsran/ofh/compression/compression_params.h"
#include "srsran/ran/resource_block.h"

namespace srsran {
namespace ofh {

/// \brief Describes the storage of one compressed PRB.
///
/// Compressed IQ samples are stored in packed format.
class compressed_prb
{
  /// Number of bits required to store compressed IQ samples of one PRB.
  static constexpr size_t CAPACITY_BITS = NOF_SUBCARRIERS_PER_RB * 2 * MAX_IQ_WIDTH;

public:
  compressed_prb() = default;

  /// Returns the compression parameter.
  uint8_t get_compression_param() const { return comp_param; }

  /// Sets the compression parameter.
  void set_compression_param(uint8_t param) { comp_param = param; }

  /// Returns the internal bit buffer storage.
  static_bit_buffer<CAPACITY_BITS>& get_bit_buffer() { return data; }

  /// Returns the internal bit buffer storage.
  const static_bit_buffer<CAPACITY_BITS>& get_bit_buffer() const { return data; }

  /// Returns the internal buffer storage.
  span<uint8_t> get_byte_buffer() { return data.get_buffer(); }

  /// Returns the bit buffer storage.
  span<const uint8_t> get_byte_buffer() const { return data.get_buffer(); }

  /// Returns a span of bytes containing packed compressed IQ samples.
  span<const uint8_t> get_packed_data() const
  {
    srsran_assert(bytes_used != 0, "No bytes yet packed");
    srsran_assert(bytes_used * 8 <= CAPACITY_BITS, "Bit buffer overflow");
    return data.get_buffer().first(bytes_used);
  }

  /// Sets the size, in bytes, of the compressed IQ samples.
  void set_stored_size(unsigned bytes_used_) { bytes_used = bytes_used_; }

private:
  /// Packed bits of compressed IQ samples.
  static_bit_buffer<CAPACITY_BITS> data{CAPACITY_BITS};
  /// Number of bytes of the internal bit buffer actually used by PRB samples.
  unsigned bytes_used = 0;
  /// Compression parameter specific for every compression type.
  uint8_t comp_param = 0;
};

} // namespace ofh
} // namespace srsran
