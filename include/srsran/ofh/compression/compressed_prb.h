/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/bit_buffer.h"
#include "srsran/ran/resource_block.h"

namespace srsran {
namespace ofh {

/// \brief Describes storage of one compressed PRB.
///
/// Compressed IQ samples are stored in packed format. The class provides methods to pack an array of compressed PRB
/// samples as well as to extract samples from packed storage.
class compressed_prb
{
  /// Maximum allowed bit width.
  static constexpr size_t MAX_WIDTH = 16;
  /// Number of bytes required to store compressed IQ samples of one PRB.
  static constexpr size_t CAPACITY_BITS = NOF_SUBCARRIERS_PER_RB * 2 * MAX_WIDTH;

public:
  /// Constructors.
  compressed_prb() = default;

  /// Packs compressed data in the internal storage.
  void pack_compressed_data(span<const int16_t> compressed_iq, unsigned iq_width, uint8_t compression_param = 0);

  /// Extract \c length bits from packed buffer starting from position \c pos.
  int16_t extract_bits(unsigned pos, unsigned length) const;

  /// Returns value of a compression parameter.
  uint8_t get_compression_param() const { return comp_param; }

  /// Sets the value of a compression parameter.
  void set_compression_param(uint8_t param) { comp_param = param; }

  /// Returns the bit buffer storage to allow writing raw bytes.
  span<uint8_t> get_buffer() { return data.get_buffer(); }

  /// Returns a span of bytes containing packed compressed IQ samples.
  span<const uint8_t> get_packed_data() const { return data.get_buffer().first(bytes_used); }

  /// Sets the size, in bytes, of the compressed IQ samples.
  void set_stored_size(unsigned bytes_used_) { bytes_used = bytes_used_; }

  /// Fill with zeros.
  void zero()
  {
    comp_param = 0;
    data.zero();
  }

private:
  /// Packed bits of compressed IQ samples.
  static_bit_buffer<CAPACITY_BITS> data{CAPACITY_BITS};
  /// Compression parameter specific for every compression type.
  uint8_t comp_param;
  /// Number of bytes of the internal bit buffer actually used by PRB samples.
  unsigned bytes_used;
};

} // namespace ofh
} // namespace srsran
