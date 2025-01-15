/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"

namespace srsran {

/// Implements a dummy PUSCH decoder buffer.
class pusch_decoder_buffer_dummy : public pusch_decoder_buffer
{
public:
  // See interface for documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned /* block_size */) override
  {
    return span<log_likelihood_ratio>();
  }

  // See interface for documentation.
  void on_new_softbits(span<const log_likelihood_ratio> /* softbits */) override
  {
    // Ignore.
  }

  // See interface for documentation.
  void on_end_softbits() override
  {
    // Ignore.
  }
};

} // namespace srsran
