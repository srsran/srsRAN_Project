/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/srsvec/copy.h"

namespace srsran {

class pusch_decoder_buffer_spy : public pusch_decoder_buffer
{
public:
  pusch_decoder_buffer_spy() = default;

  pusch_decoder_buffer_spy(unsigned buffer_size) { data.resize(buffer_size); }

  // See interface for documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override
  {
    data.resize(block_size + count);
    return span<log_likelihood_ratio>(data).subspan(count, block_size);
  }

  // See interface for documentation.
  void on_new_softbits(span<const log_likelihood_ratio> softbits) override
  {
    span<log_likelihood_ratio> block = get_next_block_view(softbits.size());

    if (block.data() != softbits.data()) {
      srsvec::copy(block, softbits);
    }

    count += softbits.size();
  }

  // See interface for documentation.
  void on_end_softbits() override
  {
    // Ignore.
  }

  span<const log_likelihood_ratio> get_data() const { return span<const log_likelihood_ratio>(data).first(count); }

private:
  unsigned                          count = 0;
  std::vector<log_likelihood_ratio> data;
};

} // namespace srsran
