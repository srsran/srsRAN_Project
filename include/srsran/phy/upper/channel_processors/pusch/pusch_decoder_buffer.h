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

/// \brief PUSCH decoder buffer interface.
///
/// Interfaces the UL-SCH demultiplexer with the different message decoders (SCH data, HARQ ACK, CSI Part 1, and CSI
/// Part 2). The UL-SCH demultiplexer notifies processed continuous blocks of data using the method \ref on_new_softbits
/// and notifies the end of data using the method \ref on_end_softbits.
class pusch_decoder_buffer
{
public:
  /// Default destructor.
  virtual ~pusch_decoder_buffer() = default;

  /// \brief Gets a view of the next block of demultiplexed soft bits.
  ///
  /// This method is temporal. It allows the UL-SCH demultiplexer to get a view of the soft bits storage in the decoder
  /// to avoid to copy.
  ///
  /// \param[in] block_size Block size of continuous soft bits to write.
  /// \return A read-write view of the next soft bits available for writing.
  virtual span<log_likelihood_ratio> get_next_block_view(unsigned block_size) = 0;

  /// \brief Notifies a new block of soft bits.
  /// \param[in] softbits Soft bits belonging to a codeword.
  /// \remark \c demodulated and \c scrambled must have the same size.
  virtual void on_new_softbits(span<const log_likelihood_ratio> softbits) = 0;

  /// \brief Notifies the end of the softbits.
  ///
  /// It can be used by the decoder to start decoding.
  virtual void on_end_softbits() = 0;
};

} // namespace srsran