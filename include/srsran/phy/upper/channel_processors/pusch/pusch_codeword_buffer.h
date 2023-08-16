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

/// \brief PUSCH demodulator codeword buffer interface.
///
///
class pusch_codeword_buffer
{
public:
  /// Default destructor.
  virtual ~pusch_codeword_buffer() = default;

  /// \brief Gets a view of the next block of demodulated soft bits.
  ///
  /// This method is temporal. It allows the PUSCH demodulator to get a view of the demodulated soft bits storage and it
  /// avoid the copy.
  ///
  /// \param[in] block_size Number of soft bits to read.
  /// \return A read-write view of the next soft bits available for writing.
  virtual span<log_likelihood_ratio> get_next_block_view(unsigned block_size) = 0;

  /// \brief Writes a block of codeword soft bits.
  ///
  /// \param[in] demodulated Soft bits after the modulation demapping.
  /// \param[in] scrambled   Soft bits after the scrambling.
  virtual void on_new_block(span<const log_likelihood_ratio> demodulated,
                            span<const log_likelihood_ratio> scrambled) = 0;

  /// Notifies the end of the codeword.
  virtual void on_end_codeword() = 0;
};

} // namespace srsran