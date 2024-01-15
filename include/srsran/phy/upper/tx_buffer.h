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

#include "srsran/adt/span.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"

namespace srsran {

/// \brief Describes a PDSCH rate matcher buffer.
///
/// The aim is to provide the physical layer shared channel FEC with a persistent storage of the rate matcher buffer
/// among transmissions for a given user and HARQ process.
///
class tx_buffer
{
public:
  /// Default destructor.
  virtual ~tx_buffer() = default;

  /// Gets the number of codeblocks.
  virtual unsigned get_nof_codeblocks() const = 0;

  /// \brief Gets the codeblock absolute identifier.
  ///
  /// The absolute codeblock identifier refers to the codeblock identifier within the common codeblock memory pool. It
  /// is available for decoders which use external memory.
  ///
  /// \param[in] codeblock_id Codeblock identifier.
  /// \remark The codeblock identifier must not exceed get_max_nof_codeblocks()-1.
  virtual unsigned get_absolute_codeblock_id(unsigned codeblock_id) const = 0;

  /// \brief Gets an encoded read-write codeblock.
  /// \param[in] codeblock_id   Codeblock identifier.
  /// \param[in] codeblock_size Codeblock size.
  /// \return A view of the codeblock bits.
  /// \remark The codeblock identifier must not exceed get_max_nof_codeblocks()-1.
  virtual bit_buffer get_codeblock(unsigned codeblock_id, unsigned codeblock_size) = 0;
};

} // namespace srsran
