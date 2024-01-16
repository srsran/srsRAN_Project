/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/adt/concurrent_queue.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/span.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include <cstdint>
#include <vector>

namespace srsran {

/// Manages a codeblock buffer pool.
class rx_buffer_codeblock_pool
{
private:
  /// Codeblock identifier list type.
  using codeblock_identifier_list =
      concurrent_queue<unsigned, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>;

  /// Describes a codeblock buffer entry.
  struct entry {
    /// Contains the codeblock soft bits.
    std::vector<log_likelihood_ratio> soft_bits;
    /// Contains the codeblock data bits.
    dynamic_bit_buffer data_bits;
  };

  /// Stores all codeblock entries.
  std::vector<entry> entries;
  /// List containing the free codeblocks identifiers.
  codeblock_identifier_list free_list;

public:
  /// \brief Creates a receive buffer codeblock pool.
  /// \param[in] nof_codeblocks Indicates the maximum number of codeblocks.
  /// \param[in] max_codeblock_size Indicates the maximum codeblock size.
  /// \param[in] external_soft_bits Set to true to indicate that soft bits are not stored in the buffer.
  rx_buffer_codeblock_pool(unsigned nof_codeblocks, unsigned max_codeblock_size, bool external_soft_bits) :
    free_list(nof_codeblocks)
  {
    entries.resize(nof_codeblocks);
    unsigned cb_id = 0;
    for (entry& e : entries) {
      e.soft_bits.resize(external_soft_bits ? 0 : max_codeblock_size);
      // The maximum number of data bits is
      // max_codeblock_size * max(BG coding rate) = max_codeblock_size * (1/3)
      e.data_bits.resize(divide_ceil(max_codeblock_size, 3));
      // Push codeblock identifier into the free list.
      while (!free_list.try_push(cb_id++)) {
      }
    }
  }

  /// \brief Reserves a codeblock buffer.
  /// \return The codeblock identifier in the pool if it is reserved successfully. Otherwise, \c nullopt
  optional<unsigned> reserve()
  {
    // Try to get an available codeblock.
    return free_list.try_pop();
  }

  /// \brief Frees a codeblock buffer.
  /// \param[in] cb_id Indicates the codeblock identifier in the pool.
  void free(unsigned cb_id)
  {
    // Push codeblock identifier back in the pool.
    while (!free_list.try_push(cb_id)) {
    }
  }

  /// \brief Gets a codeblock soft-bit buffer.
  /// \param[in] cb_id Indicates the codeblock identifier.
  /// \return A view to the codeblock soft-bit buffer.
  span<log_likelihood_ratio> get_soft_bits(unsigned cb_id)
  {
    srsran_assert(cb_id < entries.size(), "Codeblock index ({}) is out of range ({}).", cb_id, entries.size());
    return entries[cb_id].soft_bits;
  }

  /// \brief Gets a codeblock data-bit buffer.
  /// \param[in] cb_id Indicates the codeblock identifier.
  /// \return A view to the codeblock data-bit buffer.
  bit_buffer& get_data_bits(unsigned cb_id)
  {
    srsran_assert(cb_id < entries.size(), "Codeblock index ({}) is out of range ({}).", cb_id, entries.size());
    return entries[cb_id].data_bits;
  }
};

} // namespace srsran
