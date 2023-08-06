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

#include "srsran/adt/span.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"

namespace srsran {

class bit_buffer;

/// \brief Describes a receiver softbuffer.
///
/// The aim is to provide the physical layer shared channel FEC with a persistent storage of CRC and soft bits buffer
/// among transmissions for a given user and HARQ process.
///
/// The CRCs are set to \c true if the codeblocks have been successfully decoded.  By looking at these flags, codeblocks
/// decoded on a prior transmission do not require decoding.
///
/// It is the responsibility of the shared channel receiver to reset CRCs and set to zero the soft bits when new data is
/// indicated.
class rx_softbuffer
{
public:
  /// Default destructor.
  virtual ~rx_softbuffer() = default;

  /// Gets the number of codeblocks.
  virtual unsigned get_nof_codeblocks() const = 0;

  /// Resets all codeblocks CRCs.
  virtual void reset_codeblocks_crc() = 0;

  /// \brief Gets all codeblocks CRCs.
  /// \return A view of the codeblocks CRCs: each entry represents the CRC state of a codeblock.
  virtual span<bool> get_codeblocks_crc() = 0;

  /// \brief Gets the codeblock absolute identifier.
  ///
  /// The absolute codeblock identifier refers to the codeblock identifier within the common codeblock memory pool. It
  /// is available for decoders which use external memory.
  ///
  /// \param[in] codeblock_id Indicates the codeblock identifier.
  /// \remark The codeblock identifier must not exceed get_max_nof_codeblocks()-1.
  virtual unsigned get_absolute_codeblock_id(unsigned codeblock_id) const = 0;

  /// \brief Gets a codeblock soft-bit buffer.
  /// \param[in] codeblock_id Indicates the codeblock identifier.
  /// \param[in] codeblock_size Indicates the codeblock size.
  /// \return A view of the codeblock soft bits.
  /// \remark The codeblock identifier must not exceed get_max_nof_codeblocks()-1.
  virtual span<log_likelihood_ratio> get_codeblock_soft_bits(unsigned codeblock_id, unsigned codeblock_size) = 0;

  /// \brief Gets a codeblock data-bit buffer.
  /// \param[in] codeblock_id Indicates the codeblock identifier.
  /// \param[in] data_size Indicates the data size, i.e. the number of data/information bits in the codeblock.
  /// \return A view of the codeblock data bits (the information message).
  /// \remark The codeblock identifier must not exceed get_max_nof_codeblocks()-1.
  /// \note Data bits do not include filler bits, codeblock-specific CRC bits or zero-padding bits.
  virtual bit_buffer get_codeblock_data_bits(unsigned codeblock_id, unsigned data_size) = 0;
};

} // namespace srsran
