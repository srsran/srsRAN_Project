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

class pusch_codeword_buffer_spy : public pusch_codeword_buffer
{
public:
  pusch_codeword_buffer_spy() = default;

  pusch_codeword_buffer_spy(unsigned buffer_size) : demodulated(buffer_size), descrambled(buffer_size) {}

  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override
  {
    srsran_assert(!completed, "Wrong state.");
    srsran_assert(descrambled.size() >= block_size + count,
                  "Block size (i.e., {}) plus the current number of bits (i.e., {}) exceeds the data size (i.e., {}).",
                  block_size,
                  count,
                  descrambled.size());
    return span<log_likelihood_ratio>(descrambled).subspan(count, block_size);
  }

  void on_new_block(span<const log_likelihood_ratio> demodulated_,
                    span<const log_likelihood_ratio> descrambled_) override
  {
    srsran_assert(!completed, "Wrong state.");
    srsran_assert(demodulated_.size() == descrambled_.size(), "Sizes must be equal.");

    span<log_likelihood_ratio> descrambled_block =
        span<log_likelihood_ratio>(descrambled).subspan(count, descrambled_.size());
    srsvec::copy(descrambled_block, descrambled_);

    span<log_likelihood_ratio> demodulated_block =
        span<log_likelihood_ratio>(demodulated).subspan(count, demodulated_.size());
    srsvec::copy(demodulated_block, demodulated_);

    count += demodulated.size();
  }

  void on_end_codeword() override
  {
    srsran_assert(!completed, "Wrong state.");
    completed = true;
  }

  span<const log_likelihood_ratio> get_demodulated() const
  {
    srsran_assert(completed, "Wrong state.");
    return span<const log_likelihood_ratio>(demodulated).first(count);
  }

  span<const log_likelihood_ratio> get_descrambled() const
  {
    srsran_assert(completed, "Wrong state.");
    return span<const log_likelihood_ratio>(descrambled).first(count);
  }

private:
  bool                              completed = false;
  unsigned                          count     = 0;
  std::vector<log_likelihood_ratio> demodulated;
  std::vector<log_likelihood_ratio> descrambled;
};

} // namespace srsran
