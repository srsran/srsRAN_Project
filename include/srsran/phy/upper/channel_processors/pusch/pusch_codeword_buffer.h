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

/// \brief PUSCH codeword buffer interface.
///
/// Interfaces the PUSCH demodulator with the UL-SCH demultiplexer. The PUSCH demodulator notifies processed continuous
/// blocks of data using the method \ref on_new_block and notifies the end of data using the method \ref
/// on_end_codeword.
class pusch_codeword_buffer
{
public:
  /// Default destructor.
  virtual ~pusch_codeword_buffer() = default;

  /// \brief Gets a view of the next block of demodulated soft bits.
  ///
  /// This method is temporal. It allows the PUSCH demodulator to get a view of the demodulated soft bits storage and it
  /// avoids the copy.
  ///
  /// \param[in] block_size Number of soft bits to read.
  /// \return A read-write view of the next soft bits available for writing.
  virtual span<log_likelihood_ratio> get_next_block_view(unsigned block_size) = 0;

  /// \brief Notifies a new block of demodulated codeword soft bits (after reverting scrambling) and the corresponding
  /// scrambling sequence.
  /// \param[in] data           New demodulated soft bits.
  /// \param[in] scrambling_seq New scrambling sequence corresponding to \c new_data.
  /// \remark \c data and \c scrambling_seq must have the same size.
  virtual void on_new_block(span<const log_likelihood_ratio> data, const bit_buffer& scrambling_seq) = 0;

  /// Notifies the end of the codeword.
  virtual void on_end_codeword() = 0;
};

} // namespace srsran