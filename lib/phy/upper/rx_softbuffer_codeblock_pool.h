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
#include "rx_softbuffer_impl.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/phy/upper/rx_softbuffer_pool.h"
#include "srsran/phy/upper/unique_rx_softbuffer.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/math_utils.h"

namespace srsran {

/// Manages a codeblock buffer pool.
class rx_softbuffer_codeblock_pool
{
private:
  /// Describes a codeblock buffer entry.
  struct entry {
    /// Indicates if the entry is reserved.
    bool reserved;
    /// Contains the codeblock soft bits.
    std::vector<log_likelihood_ratio> soft_bits;
    /// Contains the codeblock data bits.
    dynamic_bit_buffer data_bits = {};
  };

  /// Stores all codeblock entries.
  std::vector<entry> entries;

public:
  /// Default CB identifier for unreserved codeblocks.
  static constexpr unsigned UNRESERVED_CB_ID = UINT32_MAX;

  /// \brief Creates a receive buffer codeblock pool.
  /// \param[in] nof_codeblocks Indicates the maximum number of codeblocks.
  /// \param[in] max_codeblock_size Indicates the maximum codeblock size.
  /// \param[in] external_soft_bits Set to true to indicate that soft bits are not stored in the buffer.
  rx_softbuffer_codeblock_pool(unsigned nof_codeblocks, unsigned max_codeblock_size, bool external_soft_bits)
  {
    entries.resize(nof_codeblocks);
    for (entry& e : entries) {
      e.reserved = false;
      e.soft_bits.resize(external_soft_bits ? 0 : max_codeblock_size);
      // The maximum number of data bits is
      // max_codeblock_size * max(BG coding rate) = max_codeblock_size * (1/3)
      e.data_bits.resize(divide_ceil(max_codeblock_size, 3));
    }
  }

  /// \brief Reserves a codeblock softbuffer.
  /// \return The codeblock identifier in the pool if it is reserved successfully. Otherwise, \c UNRESERVED_CB_ID
  unsigned reserve()
  {
    // Find the first available codeblock.
    for (unsigned cb_id = 0; cb_id != entries.size(); ++cb_id) {
      if (!entries[cb_id].reserved) {
        entries[cb_id].reserved = true;
        return cb_id;
      }
    }
    return UNRESERVED_CB_ID;
  }

  /// \brief Frees a codeblock softbuffer.
  /// \param[in] cb_id Indicates the codeblock identifier in the pool.
  void free(unsigned cb_id)
  {
    // Skip if the codeblock identifier is equal to the unreserved identifier.
    if (cb_id == UNRESERVED_CB_ID) {
      return;
    }

    srsran_assert(cb_id < entries.size(), "Codeblock index ({}) is out of range ({}).", cb_id, entries.size());
    srsran_assert(entries[cb_id].reserved, "Codeblock index ({}) is not reserved.", cb_id);
    entries[cb_id].reserved = false;
  }

  /// \brief Gets a codeblock soft-bit buffer.
  /// \param[in] cb_id Indicates the codeblock identifier.
  /// \return A view to the codeblock soft-bit buffer.
  span<log_likelihood_ratio> get_soft_bits(unsigned cb_id)
  {
    srsran_assert(cb_id < entries.size(), "Codeblock index ({}) is out of range ({}).", cb_id, entries.size());
    srsran_assert(entries[cb_id].reserved, "Codeblock index ({}) is not reserved.", cb_id);
    return entries[cb_id].soft_bits;
  }

  /// \brief Gets a codeblock data-bit buffer.
  /// \param[in] cb_id Indicates the codeblock identifier.
  /// \return A view to the codeblock data-bit buffer.
  bit_buffer& get_data_bits(unsigned cb_id)
  {
    srsran_assert(cb_id < entries.size(), "Codeblock index ({}) is out of range ({}).", cb_id, entries.size());
    srsran_assert(entries[cb_id].reserved, "Codeblock index ({}) is not reserved.", cb_id);
    return entries[cb_id].data_bits;
  }
};

} // namespace srsran
