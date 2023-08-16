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
#include "srsran/phy/upper/channel_processors/pusch/pusch_codeword_buffer.h"
#include "srsran/srsvec/copy.h"

namespace srsran {

/// \brief Implements PUSCH codeword buffer.
///
/// Initially the codeword buffer is completed. The first time a new block is requested as a view (see
/// \ref pusch_codeword_buffer::get_next_block_view) or a new block is notified (see \ref
/// pusch_codeword_buffer::on_new_block), the codeword buffer is not completed and it concatenates blocks of soft bits.
///
/// Once the codeblock is complete, it must be notified (see \ref pusch_codeword_buffer::on_end_codeword), after that
/// the entire view of the codeblock is available (see \ref get_codeword).
class pusch_codeword_buffer_impl : public pusch_codeword_buffer
{
public:
  /// \brief Creates a PUSCH codeword buffer with a limited size.
  /// \param[in] max_re Determines the maximum number of soft bits that the buffer can store.
  pusch_codeword_buffer_impl(unsigned max_re) : codeword_buffer(max_re) {}

  // See interface for documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override
  {
    // Resets state if it is completed.
    if (completed) {
      completed = false;
      count     = 0;
    }

    // Makes sure the block size does not overflow the buffer.
    srsran_assert(
        count + block_size <= codeword_buffer.size(),
        "The sum of current buffer number of elements (i.e., {}) and the and the block size (i.e., {}), exceeds the "
        "total number of elements of the buffer (i.e., {}).",
        count,
        block_size,
        codeword_buffer.size());

    // Create view of the soft bit buffer.
    return span<log_likelihood_ratio>(codeword_buffer).subspan(count, block_size);
  }

  // See interface for documentation.
  void on_new_block(span<const log_likelihood_ratio> /* demodulated */,
                    span<const log_likelihood_ratio> scrambled) override
  {
    append(scrambled);
  }

  // See interface for documentation.
  void on_end_codeword() override
  {
    srsran_assert(!completed, "The codeword cannot be completed twice.");
    completed = true;
  }

  /// \brief Gets the complete codeword.
  /// \remark An assertion is triggered if the codeword is not complete.
  span<const log_likelihood_ratio> get_codeword() const
  {
    srsran_assert(completed, "The codeword soft bits are not available if the codeword is not completed.");
    return span<const log_likelihood_ratio>(codeword_buffer).first(count);
  }

private:
  /// Flags if the codeword is complete.
  bool completed = true;
  /// Counts the current number of soft bits.
  unsigned count = 0;
  /// Soft bits storage.
  std::vector<log_likelihood_ratio> codeword_buffer;

  /// Appends softbits to the current buffer.
  void append(span<const log_likelihood_ratio> softbits)
  {
    // Create view of the soft bit buffer.
    span<log_likelihood_ratio> codeword_block = get_next_block_view(softbits.size());

    // Copy soft bits.
    if ((codeword_block.data() != softbits.data()) || (codeword_block.size() != softbits.size())) {
      srsvec::copy(codeword_block, softbits);
    }

    // Increment number of soft bits in the buffer.
    count += softbits.size();
  }
};

} // namespace srsran