/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "radio_uhd_tx_stream.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_reader_view.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

void radio_uhd_tx_stream::recv_async_msg()
{
  uhd_exception_handler exception_handler;
  bool                  valid;
  uhd::async_metadata_t async_metadata;

  // Retrieve asynchronous message.
  if (!exception_handler.safe_execution([this, &valid, &async_metadata]() {
        valid = stream->recv_async_msg(async_metadata, RECV_ASYNC_MSG_TIMEOUT_S);
      })) {
    fmt::print(stderr,
               "Error: receiving asynchronous message for stream {}. {}.\n",
               stream_id,
               exception_handler.get_error_message().c_str());
    return;
  }

  // Skip if it is not valid.
  if (!valid) {
    return;
  }

  // Handle event.
  radio_notification_handler::event_description event_description = {};
  event_description.stream_id                                     = stream_id;
  event_description.channel_id                                    = async_metadata.channel;
  event_description.source                                        = radio_notification_handler::event_source::TRANSMIT;
  event_description.type                                          = radio_notification_handler::event_type::UNDEFINED;
  switch (async_metadata.event_code) {
    case uhd::async_metadata_t::EVENT_CODE_BURST_ACK:
      state_fsm.async_event_end_of_burst_ack();
      break;
    case uhd::async_metadata_t::EVENT_CODE_TIME_ERROR:
      event_description.type = radio_notification_handler::event_type::LATE;
      state_fsm.async_event_late_underflow(async_metadata.time_spec - sample_offset);
      break;
    case uhd::async_metadata_t::EVENT_CODE_UNDERFLOW:
    case uhd::async_metadata_t::EVENT_CODE_UNDERFLOW_IN_PACKET:
      event_description.type = radio_notification_handler::event_type::UNDERFLOW;
      state_fsm.async_event_late_underflow(async_metadata.time_spec - sample_offset);
      break;
    case uhd::async_metadata_t::EVENT_CODE_SEQ_ERROR:
    case uhd::async_metadata_t::EVENT_CODE_SEQ_ERROR_IN_BURST:
    case uhd::async_metadata_t::EVENT_CODE_USER_PAYLOAD:
      event_description.type = radio_notification_handler::event_type::OTHER;
      break;
  }

  // Notify event if it is defined.
  if (event_description.type != radio_notification_handler::event_type::UNDEFINED) {
    notifier.on_radio_rt_event(event_description);
  }
}

void radio_uhd_tx_stream::run_recv_async_msg()
{
  // If stop was called, then stop enqueueing the task.
  if (state_fsm.is_stopping()) {
    state_fsm.async_task_stopped();
    return;
  }

  // Receive asynchronous message.
  recv_async_msg();

  // Enqueue the task again.
  if (not async_executor.defer([this]() { run_recv_async_msg(); })) {
    fmt::print(stderr, "Unable to run recv async UHD stream task\n");
    state_fsm.async_task_stopped();
  }
}

bool radio_uhd_tx_stream::transmit_block(unsigned&                             nof_txd_samples,
                                         const baseband_gateway_buffer_reader& buffs,
                                         unsigned                              buffer_offset,
                                         const uhd::tx_metadata_t&             md)
{
  // Extract number of samples.
  unsigned num_samples = buffs[0].size() - buffer_offset;

  // Make sure the number of channels is equal.
  report_fatal_error_if_not(buffs.get_nof_channels() == nof_channels, "Number of channels does not match.");

  // Flatten buffers.
  static_vector<void*, RADIO_MAX_NOF_CHANNELS> buffs_flat_ptr(nof_channels);
  for (unsigned channel = 0; channel != nof_channels; ++channel) {
    buffs_flat_ptr[channel] = (void*)buffs[channel].subspan(buffer_offset, num_samples).data();
  }

  // Make UHD buffers.
  uhd::tx_streamer::buffs_type buffs_cpp(buffs_flat_ptr.data(), nof_channels);

  // Safe transmission.
  return safe_execution([this, &buffs_cpp, num_samples, &md, &nof_txd_samples]() {
    nof_txd_samples = stream->send(buffs_cpp, num_samples, md, TRANSMIT_TIMEOUT_S);
  });
}

radio_uhd_tx_stream::radio_uhd_tx_stream(uhd::usrp::multi_usrp::sptr& usrp_,
                                         const stream_description&    description,
                                         task_executor&               async_executor_,
                                         radio_notification_handler&  notifier_) :
  stream_id(description.id),
  async_executor(async_executor_),
  notifier(notifier_),
  srate_hz(description.srate_hz),
  sample_offset(description.sample_offset),
  nof_channels(description.ports.size()),
  discontinuous_tx(description.discontinuous_tx),
  discontinuous_config(description.discontinuous_config),
  gpio_tx_index(description.gpio_tx_index),
  gpio_tx_sense(description.gpio_tx_sense),
  gpio_tx_use_config(description.gpio_tx_use_config),
  last_tx_timespec(0.0),
  power_ramping_buffer(nof_channels, 0)
{
  srsran_assert(std::isnormal(srate_hz) && (srate_hz > 0.0), "Invalid sampling rate {}.", srate_hz);

  // Build stream arguments.
  uhd::stream_args_t stream_args = {};
  stream_args.cpu_format         = "fc32";
  switch (description.otw_format) {
    case radio_configuration::over_the_wire_format::DEFAULT:
    case radio_configuration::over_the_wire_format::SC16:
      stream_args.otw_format = "sc16";
      break;
    case radio_configuration::over_the_wire_format::SC12:
      stream_args.otw_format = "sc12";
      break;
    case radio_configuration::over_the_wire_format::SC8:
      stream_args.otw_format = "sc8";
      break;
  }
  stream_args.args     = description.args;
  stream_args.channels = description.ports;

  if (!safe_execution([this, usrp_, &stream_args]() {
        usrp = usrp_;
        if (gpio_tx_index.has_value()) {
          uint32_t pin_mask = 1 << gpio_tx_index.value();
          usrp->set_gpio_attr("FP0", "CTRL", 0, pin_mask);
          usrp->set_gpio_attr("FP0", "OUT", gpio_tx_sense ? 0 : pin_mask,
                  pin_mask); // set RX
          usrp->set_gpio_attr("FP0", "DDR", pin_mask, pin_mask);
        }
        stream          = usrp->get_tx_stream(stream_args);
        max_packet_size = stream->get_max_num_samps();
      })) {
    printf("Error:  failed to create transmit stream %d. %s.\n", stream_id, get_error_message().c_str());
    return;
  }

  gpio_tx_prelude_samples = description.gpio_tx_prelude * description.srate_hz / 1e6;

  // Use zero padding to absorb power ramping on each new burst.
  if (discontinuous_tx) {
    power_ramping_nof_samples = description.srate_hz * static_cast<double>(description.power_ramping_us) / 1e6;

    // Align the number of padding samples to the TX buffer size.
    power_ramping_nof_samples = (power_ramping_nof_samples / max_packet_size) * max_packet_size;
    double aligned_guard_time = static_cast<double>(power_ramping_nof_samples) * 1000000.0 / srate_hz;
    fmt::print(
        "Aligning TX power ramping guard time to the UHD buffer size, i.e., {}. Aligned guard time: {:.1f} us.\n",
        max_packet_size,
        aligned_guard_time);

    power_ramping_buffer.resize(power_ramping_nof_samples);
    for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
      srsvec::zero(power_ramping_buffer.get_writer()[i_channel]);
    }
  }

  // Notify FSM that it was successfully initialized.
  state_fsm.init_successful();

  // Create asynchronous task.
  run_recv_async_msg();
}

template<class T>
static void sanitize_start_end_last(
    bool last_on,
    std::optional<T>& start,
    std::optional<T>& end) {
  if (start.has_value() && end.has_value() && start.value() == end.value()) {
    start.reset();
    end.reset();
  }
  if (last_on && start.has_value() && end.has_value()
      && start.value() < end.value()) {
    start.reset();
  }
  if (!last_on && start.has_value() && end.has_value()
      && start.value() > end.value()) {
    end.reset();
  }
}

// Assumes that start/end have been sanitized with sanitize_start_end_last.
template<class T>
static void update_last(
    bool& last_on,
    const std::optional<T>& start,
    const std::optional<T>& end) {
  if (start.has_value()) {
    if (!end.has_value()) {
      last_on = true;
    } else {
      last_on = start.value() > end.value();
    }
  } else if (end.has_value()) {
    last_on = false;
  }
}

template<class T>
static bool is_1(
    const T& time,
    const bool last_on,
    const std::optional<T>& start,
    const std::optional<T>& end) {
  if (last_on) {
    if (!end.has_value()) {
      return true;
    } else {
      return time < end.value();
    }
  } else {
    if (!start.has_value()) {
      return false;
    } else if (!end.has_value()) {
      return time >= start.value();
    } else {
      return time >= start.value() && time < end.value();
    }
  }

  // Impossible
}

void radio_uhd_tx_stream::transmit(const baseband_gateway_buffer_reader&        data,
                                   const baseband_gateway_transmitter_metadata& tx_md)
{
  // Protect stream transmitter.
  std::unique_lock<std::mutex> lock(stream_transmit_mutex);

  uhd::tx_metadata_t uhd_metadata;

  std::optional<uhd::time_spec_t> idle_start_time_spec;
  std::optional<uhd::time_spec_t> idle_end_time_spec;
  bool idle_extend_end = false;

  unsigned idle_start_index = 0;
  unsigned idle_end_index = data.get_nof_samples();

  std::optional<uhd::time_spec_t> start_time_spec;
  std::optional<uhd::time_spec_t> end_time_spec;

  unsigned start_index;
  unsigned end_index;

  std::optional<uhd::time_spec_t> dl_config_start_ts;
  std::optional<uhd::time_spec_t> dl_config_end_ts;
  bool dl_config_extend_end = false;

  unsigned dl_config_start_index = 0;
  unsigned dl_config_end_index = data.get_nof_samples();

  unsigned end_extension_count = 0;
  uhd::time_spec_t end_extension_ts =
      uhd::time_spec_t::from_ticks(0, srate_hz);

  // Subtracting sample offset because radio seems to have a shifted view
  // between GPIO and samples, doesn't work well without this and I think
  // it's because the GPIO is turning off the PA early.
  if (sample_offset < 0) {
    end_extension_count = -sample_offset;
    end_extension_ts = uhd::time_spec_t::from_ticks(
            end_extension_count, srate_hz);
  }

  if (tx_md.tx_start.has_value()) {
    // Start at specified time
    idle_start_time_spec = uhd::time_spec_t::from_ticks(tx_md.ts
            + static_cast<baseband_gateway_timestamp>(tx_md.tx_start.value()),
            srate_hz);
    idle_start_index = tx_md.tx_start.value();
  } else {
    // If not empty and not currently running start at beginning
    if (!tx_md.is_empty && !last_sample_state_tx) {
      idle_start_time_spec = uhd::time_spec_t::from_ticks(tx_md.ts, srate_hz);
    }
  }

  if (tx_md.tx_end.has_value()) {
    // End at specified time
    idle_end_time_spec = uhd::time_spec_t::from_ticks(tx_md.ts
          + static_cast<baseband_gateway_timestamp>(tx_md.tx_end.value()),
          srate_hz);
    idle_end_index = tx_md.tx_end.value();
    idle_extend_end = true;
  } else {
    // If empty and was running, stop at beginning
    if (tx_md.is_empty && last_sample_state_tx) {
      idle_end_time_spec = uhd::time_spec_t::from_ticks(tx_md.ts, srate_hz);
      idle_end_index = 0;
    }
  }

  if (tx_md.dl_config_start.has_value() && tx_md.dl_config_end.has_value()) {
    srsran_assert(tx_md.dl_config_start.value() < tx_md.dl_config_end.value(),
        "This code doesn't support an end before start on the TDD config in "
        "one block!");
  }

  sanitize_start_end_last(last_sample_state_tx, idle_start_time_spec,
          idle_end_time_spec);

  std::optional<unsigned> dl_config_start = tx_md.dl_config_start;
  std::optional<unsigned> dl_config_end = tx_md.dl_config_end;

  sanitize_start_end_last(last_tdd_config_state_tx,
      dl_config_start, dl_config_end);

  // If there's a dl_config_start, push the start of the transmission back to
  // that point
  if (dl_config_start.has_value()) {
    dl_config_start_index = dl_config_start.value();
    dl_config_start_ts = uhd::time_spec_t::from_ticks(
        tx_md.ts + dl_config_start_index, srate_hz);
    if (is_1(dl_config_start_ts.value(), last_sample_state_tx,
          idle_start_time_spec, idle_end_time_spec)) {
      // If transmission ends before TDD config start, cancel it
      if (idle_end_time_spec.has_value() &&
          idle_end_time_spec.value() <= dl_config_start_ts.value()) {
        idle_start_time_spec.reset();
        idle_end_time_spec.reset();
        idle_end_index = data.get_nof_samples();
      } else {
        idle_start_time_spec = dl_config_start_ts.value();
        idle_start_index = dl_config_start_index;
      }
    }
  // If we're not in a TDD config transmit, cancel the transmission, but mark
  // as running (for next TDD config start)
  } else if (!last_tdd_config_state_tx) {
    idle_start_time_spec.reset();
    idle_end_time_spec.reset();
    last_sample_state_tx = true;
  }

  // If there's a dl_config_end, pull the end of the transmission back to that
  // point
  if (dl_config_end.has_value()) {
    dl_config_end_index = dl_config_end.value();
    dl_config_end_ts = uhd::time_spec_t::from_ticks(
        tx_md.ts + dl_config_end_index, srate_hz);
    dl_config_extend_end = true;
    if (is_1(dl_config_end_ts.value(), last_sample_state_tx,
          idle_start_time_spec, idle_end_time_spec)) {
      // If transmission starts after TDD config end, cancel it
      if (idle_start_time_spec.has_value()
          && dl_config_end_ts <= idle_start_time_spec.value()) {
        idle_start_time_spec.reset();
        idle_end_time_spec.reset();
        idle_end_index = data.get_nof_samples();
      } else {
        idle_end_time_spec = dl_config_end_ts.value();
        idle_end_index = dl_config_end_index;
        idle_extend_end = true;
      }
    }
  } // No else because if we're already running the start/end passes through

  update_last(last_sample_state_tx, idle_start_time_spec, idle_end_time_spec);
  update_last(last_tdd_config_state_tx, dl_config_start, dl_config_end);

  if (discontinuous_config) {
    start_time_spec = dl_config_start_ts;
    start_index = dl_config_start_index;
    end_time_spec = dl_config_end_ts;
    end_index = dl_config_end_index;

    if (dl_config_extend_end) {
      if (end_time_spec.has_value()) {
        end_time_spec.value() += end_extension_ts;
      }
      end_index += end_extension_count;
    }
  } else {
    start_time_spec = idle_start_time_spec;
    start_index = idle_start_index;
    end_time_spec = idle_end_time_spec;
    end_index = idle_end_index;

    if (idle_extend_end) {
      if (end_time_spec.has_value()) {
        end_time_spec.value() += end_extension_ts;
      }
      end_index += end_extension_count;
    }
  }

  bool has_tx_start = start_time_spec.has_value();
  bool has_tx_end   = end_time_spec.has_value();

  uhd::time_spec_t time_spec            =
      time_spec.from_ticks(tx_md.ts, srate_hz);
  bool             transmit;
  bool             start_of_tx          = false;
  bool             end_of_tx            = false;
  bool             force_start_of_burst = false;
  bool             force_end_of_burst   = false;
  bool             start_of_burst;
  bool             end_of_burst;

  if (has_tx_start && discontinuous_tx) {
    // Time actual transmission will start (regardless of when samples start)
    time_spec = start_time_spec.value();
  }

  // Update state.
  transmit = state_fsm.on_transmit(
          discontinuous_tx, time_spec, start_of_tx, end_of_tx,
          force_start_of_burst, force_end_of_burst,
          tx_md.is_empty, has_tx_end);
  start_of_burst = force_start_of_burst;
  end_of_burst = force_end_of_burst;

  if(discontinuous_tx) {
      uhd_metadata.start_of_burst |= start_of_tx;
      uhd_metadata.end_of_burst |= end_of_tx;
  }

  // For restarting a late/underflowed transmission
  uhd_metadata.start_of_burst |= start_of_burst;
  uhd_metadata.end_of_burst |= end_of_burst;

  if (start_of_burst) {
    // Is getting forced on for whatever exceptional reason.
    last_sample_state_tx = true;
  }
  if (end_of_burst) {
    // Is getting forced off for whatever exceptional reason.
    last_sample_state_tx = false;
  }

  // Determine actual transmission range.
  unsigned data_start = discontinuous_tx ? start_index : 0;
  unsigned data_nof_samples = discontinuous_tx ?
    end_index - start_index
    : data.get_nof_samples();

  if (gpio_tx_index.has_value()) {
    uint32_t pin_mask = 1 << gpio_tx_index.value();
    bool clear_time = false;

    std::optional<uhd::time_spec_t> gpio_start_time;
    std::optional<uhd::time_spec_t> gpio_end_time;

    // Determine start time (if there is one)
    if (force_start_of_burst) {
      gpio_start_time = time_spec;
    } else {
      if (gpio_tx_use_config) {
        if (dl_config_start_ts.has_value()) {
          gpio_start_time = dl_config_start_ts.value();
        }
      } else {
        if (idle_start_time_spec.has_value()) {
          gpio_start_time = idle_start_time_spec.value();
        }
      }
    }

    if (gpio_start_time.has_value()) {
      gpio_start_time.value() -= uhd::time_spec_t::from_ticks(
              gpio_tx_prelude_samples, srate_hz);
    }


    // Determine end time (if there is one)
    if (force_end_of_burst) {
      gpio_end_time = time_spec + uhd::time_spec_t::from_ticks(
              data_nof_samples, srate_hz);
    } else {
      if (gpio_tx_use_config) {
        if (dl_config_end_ts.has_value()) {
          gpio_end_time = dl_config_end_ts.value();
        }
      } else {
        if (idle_end_time_spec.has_value()) {
          gpio_end_time = idle_end_time_spec.value();
        }
      }
    }

    // Actually set the GPIO
    if (gpio_start_time.has_value()) {
      usrp->set_command_time(gpio_start_time.value());
      clear_time = true;
      usrp->set_gpio_attr("FP0", "OUT",
          gpio_tx_sense ? pin_mask : 0, pin_mask);
    }

    if (gpio_end_time.has_value()) {
      usrp->set_command_time(gpio_end_time.value());
      clear_time = true;
      usrp->set_gpio_attr("FP0", "OUT",
          gpio_tx_sense ? 0 : pin_mask, pin_mask);
    }
    if (clear_time) {
      usrp->clear_command_time();
    }
  }

  // Skip if no transmission is required.
  if (transmit) {
    if(uhd_metadata.start_of_burst) {
        uhd_metadata.has_time_spec = true;
        uhd_metadata.time_spec = time_spec
            + uhd::time_spec_t::from_ticks(sample_offset, srate_hz);
    }

    // Notify start of burst.
    if (uhd_metadata.start_of_burst) {
      radio_notification_handler::event_description event_description;
      event_description.stream_id  = stream_id;
      event_description.channel_id = 0;
      event_description.source     = radio_notification_handler::event_source::TRANSMIT;
      event_description.type       = radio_notification_handler::event_type::START_OF_BURST;
      event_description.timestamp.emplace(time_spec.to_ticks(srate_hz));
      notifier.on_radio_rt_event(event_description);

      // Transmit zeros before the actual transmission to absorb power ramping effects.
      if (discontinuous_tx) {
        // Compute the time in number of samples between the end of the last transmission and the start of the current
        // one.
        unsigned transmission_gap = (time_spec - last_tx_timespec).to_ticks(srate_hz);

        // Make sure that the power ramping padding starts at least 10 microseconds after the last transmission end.
        unsigned minimum_gap_before_power_ramping = srate_hz / 100000.0;
        unsigned nof_padding_samples              = 0;
        if (transmission_gap > minimum_gap_before_power_ramping) {
          nof_padding_samples = std::min(power_ramping_nof_samples, transmission_gap - minimum_gap_before_power_ramping);
        }

        if (nof_padding_samples > 0) {
          unsigned txd_padding_sps_total = 0;

          uhd::tx_metadata_t power_ramping_metadata;
          power_ramping_metadata.has_time_spec  = true;
          power_ramping_metadata.start_of_burst = true;
          power_ramping_metadata.end_of_burst   = false;
          power_ramping_metadata.time_spec = uhd_metadata.time_spec - time_spec.from_ticks(nof_padding_samples, srate_hz);

          // Modify the actual trasnmission metadata, since we have already started the burst with padding.
          uhd_metadata.start_of_burst = false;
          uhd_metadata.has_time_spec  = false;
          do {
            unsigned txd_samples = 0;

            baseband_gateway_buffer_reader_view tx_padding =
                baseband_gateway_buffer_reader_view(power_ramping_buffer.get_reader(), 0, nof_padding_samples);

            if (!transmit_block(txd_samples, tx_padding, txd_padding_sps_total, power_ramping_metadata)) {
              printf("Error: failed transmitting power ramping padding. %s.\n", get_error_message().c_str());
              return;
            }

            power_ramping_metadata.time_spec += txd_samples * srate_hz;
            txd_padding_sps_total += txd_samples;

          } while (txd_padding_sps_total < nof_padding_samples);
        }
      }
    }

    // Notify end of burst.
    if (uhd_metadata.end_of_burst) {
      radio_notification_handler::event_description event_description;
      event_description.stream_id  = stream_id;
      event_description.channel_id = 0;
      event_description.source     = radio_notification_handler::event_source::TRANSMIT;
      event_description.type       = radio_notification_handler::event_type::END_OF_BURST;
      event_description.timestamp.emplace(time_spec.to_ticks(srate_hz));
      notifier.on_radio_rt_event(event_description);
    }

    // Don't pass the transmission data to UHD if the transmit buffer is empty.
    baseband_gateway_buffer_reader_view tx_data =
        discontinuous_tx && tx_md.is_empty ? baseband_gateway_buffer_reader_view(data, 0, 0)
                                           : baseband_gateway_buffer_reader_view(data, data_start, data_nof_samples);

    unsigned nsamples = tx_data.get_nof_samples();

    // Transmit stream in multiple blocks.
    unsigned txd_samples_total = 0;
    do {
      unsigned txd_samples = 0;
      if (!transmit_block(txd_samples, tx_data, txd_samples_total, uhd_metadata)) {
        printf("Error: failed transmitting packet. %s.\n", get_error_message().c_str());
        return;
      }

      // Increment timespec.
      uhd_metadata.time_spec +=
        uhd::time_spec_t::from_ticks(txd_samples, srate_hz);

      // Increment the total amount of received samples.
      txd_samples_total += txd_samples;

    } while (txd_samples_total < nsamples);

    last_tx_timespec = time_spec + uhd::time_spec_t::from_ticks(txd_samples_total, srate_hz);
  }
}

void radio_uhd_tx_stream::stop()
{
  uhd::tx_metadata_t md;
  state_fsm.stop(md);

  // Send end-of-burst if it is in the middle of a burst.
  if (md.end_of_burst) {
    std::unique_lock<std::mutex> transmit_lock(stream_transmit_mutex);

    // Notify end of burst.
    radio_notification_handler::event_description event_description = {};
    event_description.stream_id                                     = stream_id;
    event_description.channel_id                                    = 0;
    event_description.source = radio_notification_handler::event_source::TRANSMIT;
    event_description.type   = radio_notification_handler::event_type::END_OF_BURST;
    notifier.on_radio_rt_event(event_description);

    // Flatten buffers.
    std::array<cf_t, 4>                          buffer;
    static_vector<void*, RADIO_MAX_NOF_CHANNELS> buffs_flat_ptr(nof_channels);
    for (unsigned channel = 0; channel != nof_channels; ++channel) {
      buffs_flat_ptr[channel] = (void*)buffer.data();
    }

    // Make UHD buffers.
    uhd::tx_streamer::buffs_type buffs_cpp(buffs_flat_ptr.data(), nof_channels);

    // Safe transmission. Ignore return.
    safe_execution([this, &buffs_cpp, &md]() {
      // Actual transmission. Ignore number of transmitted samples.
      stream->send(buffs_cpp, 0, md, TRANSMIT_TIMEOUT_S);
    });
  }
}

void radio_uhd_tx_stream::wait_stop()
{
  state_fsm.wait_stop();
}

unsigned radio_uhd_tx_stream::get_buffer_size() const
{
  return max_packet_size;
}
