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

/// \file
/// \brief Repository to manage the run-time contents and status of an external HARQ buffer.

#pragma once

#include "srsran/support/srsran_assert.h"
#include <vector>

namespace srsran {
namespace hal {

/// Fixed CB-offset increment used in the accelerator's HARQ memory.
/// Note that it is assumed that the HARQ memory is organized in N slots of HARQ_INCR bytes.
static constexpr unsigned HARQ_INCR = 32768;

/// External HARQ buffer context.
struct ext_harq_buffer_context_entry {
  /// HARQ soft-data length of the CB stored in that segment of the buffer.
  unsigned soft_data_len = 0;
  /// Flag indicating if the entry is empty or not.
  bool empty = true;
};

/// Abstracted interfacing to bbdev-based hardware-accelerators.
class ext_harq_buffer_context_repository
{
public:
  /// Creates a harq buffer context repository.
  /// \param[in] nof_codeblocks_    Indicates the number of codeblocks to store in the repository.
  /// \param[in] ext_harq_buff_size Size of the external HARQ buffer (in bytes).
  /// \param[in] debug_mode_        Requests to implement the debug mode (meant for unittesting).
  explicit ext_harq_buffer_context_repository(unsigned nof_codeblocks_, unsigned ext_harq_buff_size, bool debug_mode_) :
    nof_codeblocks(nof_codeblocks_)
  {
    unsigned requested_size = nof_codeblocks_ * HARQ_INCR;
    srsran_assert(requested_size <= ext_harq_buff_size,
                  "Requested size ({} bytes) for {} codeblocks exceeds external HARQ buffer capacity ({} bytes).",
                  requested_size,
                  nof_codeblocks_,
                  ext_harq_buff_size);
    repo.resize(nof_codeblocks);
    debug_mode = debug_mode_;
  }

  /// Get the entry for the provided absolute codeblock identifier.
  ext_harq_buffer_context_entry* get(unsigned absolute_codeblock_id, bool new_data)
  {
    srsran_assert(absolute_codeblock_id < nof_codeblocks,
                  "Absolute CB index {} out of bounds - HARQ buffer context has capacity for {} CBs.",
                  absolute_codeblock_id,
                  nof_codeblocks);
    // Initialize the entry if not already created (or in first transmissions).
    if (repo[absolute_codeblock_id].empty || new_data) {
      repo[absolute_codeblock_id].soft_data_len = 0;
      repo[absolute_codeblock_id].empty         = false;
    }

    return &repo[absolute_codeblock_id];
  }

  /// Free the entry reserved for the provided absolute codeblock identifier.
  void free(unsigned absolute_codeblock_id)
  {
    srsran_assert(absolute_codeblock_id < nof_codeblocks,
                  "Absolute CB index {} out of bounds - HARQ buffer context has capacity for {} CBs.",
                  absolute_codeblock_id,
                  nof_codeblocks);
    // Free the entry.
    // Note that in debug mode, the entry won't be freed to enable HARQ unitesting.
    if (!debug_mode) {
      repo[absolute_codeblock_id].empty = true;
    }
  }

private:
  // HARQ buffer context repository.
  std::vector<ext_harq_buffer_context_entry> repo;
  // Number of codeblocks that can be stored in the repository.
  unsigned nof_codeblocks;
  // Enables an optional debug mode implementation.
  bool debug_mode;
};

} // namespace hal
} // namespace srsran
