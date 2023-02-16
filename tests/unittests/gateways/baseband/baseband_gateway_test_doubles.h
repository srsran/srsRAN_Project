/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/gateways/baseband/baseband_gateway.h"
#include <random>

namespace srsgnb {

/// \brief Read-only baseband buffer.
///
/// Records all the baseband samples that pass through the baseband gateway spy classes.
class baseband_gateway_buffer_read_only : public baseband_gateway_buffer
{
private:
  unsigned                       nof_channels;
  unsigned                       nof_samples;
  std::vector<radio_sample_type> data;

public:
  // See interface for documentation.
  unsigned get_nof_channels() const override { return nof_channels; }

  // See interface for documentation.
  unsigned get_nof_samples() const override { return nof_samples; }

  // See interface for documentation.
  span<radio_sample_type> get_channel_buffer(unsigned channel_idx) override
  {
    report_fatal_error_if_not(!data.empty(), "Data is empty. Was the buffer moved?");
    report_fatal_error_if_not(
        channel_idx < nof_channels, "Channel index ({}) is out-of-range ({}).", channel_idx, nof_channels);
    unsigned offset = nof_samples * channel_idx;
    return span<radio_sample_type>(data).subspan(offset, nof_samples);
  }

  // See interface for documentation.
  span<const radio_sample_type> get_channel_buffer(unsigned channel_idx) const override
  {
    report_fatal_error_if_not(!data.empty(), "Data is empty. Was the buffer moved?");
    report_fatal_error_if_not(
        channel_idx < nof_channels, "Channel index ({}) is out-of-range ({}).", channel_idx, nof_channels);
    unsigned offset = nof_samples * channel_idx;
    return span<const radio_sample_type>(data).subspan(offset, nof_samples);
  }

  /// Creates a default empty buffer.
  baseband_gateway_buffer_read_only() : nof_channels(0), nof_samples(0)
  {
    // Do nothing.
  }

  /// Move constructor.
  baseband_gateway_buffer_read_only(baseband_gateway_buffer_read_only&& other) noexcept :
    nof_channels(other.nof_channels), nof_samples(other.nof_samples), data(std::move(other.data))
  {
    // Reset the other number of channels and samples to 0 to be consistent with other.data.size().
    other.nof_channels = 0;
    other.nof_samples  = 0;
  }

  /// Copy constructor.
  baseband_gateway_buffer_read_only(const baseband_gateway_buffer_read_only& other) noexcept :
    nof_channels(other.nof_channels), nof_samples(other.nof_samples), data(std::move(other.data))
  {
    // Do nothing.
  }

  /// Constructor from a parent class.
  baseband_gateway_buffer_read_only(const baseband_gateway_buffer& other) noexcept :
    nof_channels(other.get_nof_channels()), nof_samples(other.get_nof_samples()), data(nof_channels * nof_samples)
  {
    for (unsigned channel = 0; channel != nof_channels; ++channel) {
      span<radio_sample_type> buffer = span<radio_sample_type>(data).subspan(nof_samples * channel, nof_samples);
      srsvec::copy(buffer, other[channel]);
    }
  }

  /// Overloads assign operator.
  baseband_gateway_buffer_read_only operator=(const baseband_gateway_buffer_read_only& other)
  {
    nof_channels = other.nof_channels;
    nof_samples  = other.nof_samples;
    data         = other.data;
    return *this;
  }
};

/// \brief Baseband gateway spy class.
///
/// Records all interactions from a lower physical layer with a baseband gateway. The receive method generates random
/// baseband samples and metadata.
class baseband_gateway_spy : public baseband_gateway,
                             private baseband_gateway_transmitter,
                             private baseband_gateway_receiver
{
private:
  srslog::basic_logger&                                     logger;
  std::mt19937                                              rgen;
  std::uniform_int_distribution<baseband_gateway_timestamp> timestamp_dist;
  std::uniform_real_distribution<float>                     sample_dist;

  struct transmit_entry {
    unsigned                               stream_id;
    baseband_gateway_transmitter::metadata metadata;
    baseband_gateway_buffer_read_only      data;
  };
  std::vector<transmit_entry> transmit_entries;

  struct receive_entry {
    unsigned                            stream_id;
    baseband_gateway_receiver::metadata metadata;
    baseband_gateway_buffer_read_only   data;
  };
  std::vector<receive_entry> receive_entries;

  // See interface for documentation.
  void transmit(unsigned int                                  stream_id,
                const baseband_gateway_transmitter::metadata& metadata,
                baseband_gateway_buffer&                      data) override
  {
    logger.debug("Transmit - stream {}, {} samples at {}", stream_id, data.get_nof_samples(), metadata.ts);
    transmit_entries.emplace_back();
    transmit_entry& entry = transmit_entries.back();
    entry.stream_id       = stream_id;
    entry.metadata        = metadata;
    entry.data            = baseband_gateway_buffer_read_only(data);
  }

  // See interface for documentation.
  baseband_gateway_receiver::metadata receive(baseband_gateway_buffer& data, unsigned stream_id) override
  {
    logger.debug("Receive - stream {}, {} samples", stream_id, data.get_nof_samples());

    // Fill data with random samples.
    for (unsigned channel = 0; channel != data.get_nof_channels(); ++channel) {
      span<radio_sample_type> buffer = data.get_channel_buffer(channel);
      std::generate(
          buffer.begin(), buffer.end(), [this]() { return radio_sample_type(sample_dist(rgen), sample_dist(rgen)); });
    }

    receive_entries.emplace_back();
    receive_entry& entry = receive_entries.back();
    entry.stream_id      = stream_id;
    entry.metadata.ts    = timestamp_dist(rgen);
    entry.data           = data;

    return entry.metadata;
  }

public:
  /// Constructs an instance with a log level.
  baseband_gateway_spy(std::string log_level) :
    logger(srslog::fetch_basic_logger("BB Gateway", false)),
    rgen(456),
    timestamp_dist(0, UINT32_MAX),
    sample_dist(-1.0F, +1.0F)
  {
    logger.set_level(srslog::str_to_basic_level(log_level));
  }

  // See interface for documentation.
  baseband_gateway_transmitter& get_transmitter() override { return *this; }

  // See interface for documentation.
  baseband_gateway_receiver& get_receiver() override { return *this; }

  /// Gets all transmit entries.
  const std::vector<transmit_entry>& get_transmit_entries() const { return transmit_entries; }

  /// Gets all receive entries.
  const std::vector<receive_entry>& get_receive_entries() const { return receive_entries; }

  /// Clears all types of entries.
  void clear_all_entries()
  {
    transmit_entries.clear();
    receive_entries.clear();
  }
};

} // namespace srsgnb
