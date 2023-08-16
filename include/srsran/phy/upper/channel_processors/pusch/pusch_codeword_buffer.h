/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  /// \brief Writes a block of codeword soft bits.
  /// \param[in] demodulated Soft bits after the modulation demapping.
  /// \param[in] descrambled Soft bits after the scrambling.
  /// \remark \c demodulated and \c scrambled must have the same size.
  virtual void on_new_block(span<const log_likelihood_ratio> demodulated,
                            span<const log_likelihood_ratio> descrambled) = 0;

  /// Notifies the end of the codeword.
  virtual void on_end_codeword() = 0;
};

} // namespace srsran