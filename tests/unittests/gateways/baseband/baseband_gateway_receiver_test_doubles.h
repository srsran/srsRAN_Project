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

#include "baseband_gateway_buffer_test_doubles.h"
#include "srsran/gateways/baseband/baseband_gateway_receiver.h"
#include <random>

namespace srsran {

class baseband_gateway_receiver_spy : public baseband_gateway_receiver
{
public:
  struct entry_t {
    baseband_gateway_receiver::metadata metadata;
    baseband_gateway_buffer_read_only   data;
  };

  baseband_gateway_receiver_spy()
  {
    std::mt19937                          rgen;
    std::uniform_real_distribution<float> sample_dist(-1.0F, +1.0F);

    std::generate(cached_random_samples.begin(), cached_random_samples.end(), [&rgen, &sample_dist]() {
      return cf_t(sample_dist(rgen), sample_dist(rgen));
    });
  }

  // See interface for documentation.
  metadata receive(baseband_gateway_buffer_writer& data) override
  {
    // Fill data with random samples.
    for (unsigned channel = 0; channel != data.get_nof_channels(); ++channel) {
      span<ci16_t> buffer = data[channel];
      std::generate(buffer.begin(), buffer.end(), [this]() {
        unsigned idx           = i_cached_random_sample;
        i_cached_random_sample = (i_cached_random_sample + 1) % nof_cached_random_samples;
        return cached_random_samples[idx];
      });
    }

    entries.emplace_back();
    entry_t& entry    = entries.back();
    entry.metadata.ts = current_timestamp;
    entry.data        = data;

    current_timestamp += data.get_nof_samples();

    return entry.metadata;
  }

  void set_current_timestamp(baseband_gateway_timestamp timestamp) { current_timestamp = timestamp; }

  /// Gets all receive entries.
  const std::vector<entry_t>& get_entries() const { return entries; }

  /// Clears all types of entries.
  void clear() { entries.clear(); }

private:
  static constexpr unsigned                   nof_cached_random_samples = 123;
  std::array<cf_t, nof_cached_random_samples> cached_random_samples;
  unsigned                                    i_cached_random_sample = 0;
  baseband_gateway_timestamp                  current_timestamp      = 0;
  std::vector<entry_t>                        entries;
};

} // namespace srsran
