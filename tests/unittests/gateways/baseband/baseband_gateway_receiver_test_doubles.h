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
#include "baseband_gateway_buffer_test_doubles.h"
#include "srsran/gateways/baseband/baseband_gateway_receiver.h"
#include <random>

namespace srsran {

class baseband_gateway_receiver_spy : public baseband_gateway_receiver
{
public:
  struct entry_t {
    unsigned                            stream_id;
    baseband_gateway_receiver::metadata metadata;
    baseband_gateway_buffer_read_only   data;
  };

  baseband_gateway_receiver_spy() : timestamp_dist(0, UINT32_MAX), sample_dist(-1.0F, +1.0F) {}

  // See interface for documentation.
  unsigned get_buffer_size() override { return buffer_size; }

  // See interface for documentation.
  metadata receive(baseband_gateway_buffer& data) override
  { // Fill data with random samples.
    for (unsigned channel = 0; channel != data.get_nof_channels(); ++channel) {
      span<radio_sample_type> buffer = data.get_channel_buffer(channel);
      std::generate(
          buffer.begin(), buffer.end(), [this]() { return radio_sample_type(sample_dist(rgen), sample_dist(rgen)); });
    }

    entries.emplace_back();
    entry_t& entry    = entries.back();
    entry.stream_id   = stream_id;
    entry.metadata.ts = timestamp_dist(rgen);
    entry.data        = data;

    return entry.metadata;
  }

  void set_stream_id(unsigned stream_id_) { stream_id = stream_id_; }

  void set_buffer_size(unsigned buffer_size_) { buffer_size = buffer_size_; }

  /// Gets all receive entries.
  const std::vector<entry_t>& get_entries() const { return entries; }

  /// Clears all types of entries.
  void clear() { entries.clear(); }

private:
  std::mt19937                                              rgen;
  std::uniform_int_distribution<baseband_gateway_timestamp> timestamp_dist;
  std::uniform_real_distribution<float>                     sample_dist;
  unsigned                                                  buffer_size = 0;
  unsigned                                                  stream_id   = UINT32_MAX;
  std::vector<entry_t>                                      entries;
};

} // namespace srsran