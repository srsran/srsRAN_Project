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
#include "srsran/gateways/baseband/baseband_gateway_transmitter.h"
#include <random>

namespace srsran {

class baseband_gateway_transmitter_spy : public baseband_gateway_transmitter
{
public:
  struct entry_t {
    unsigned                               stream_id;
    baseband_gateway_transmitter::metadata metadata;
    baseband_gateway_buffer_read_only      data;
  };

  // See interface for documentation.
  void transmit(const baseband_gateway_buffer_reader& data, const baseband_gateway_transmitter::metadata& md) override
  {
    entries.emplace_back();
    entry_t& entry  = entries.back();
    entry.stream_id = stream_id;
    entry.metadata  = md;
    entry.data      = baseband_gateway_buffer_read_only(data);
  }

  void set_stream_id(unsigned stream_id_) { stream_id = stream_id_; }

  /// Gets all transmit entries.
  const std::vector<entry_t>& get_entries() const { return entries; }

  /// Clears all types of entries.
  void clear() { entries.clear(); }

private:
  unsigned             stream_id = UINT32_MAX;
  std::vector<entry_t> entries;
};

} // namespace srsran