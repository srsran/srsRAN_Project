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

#pragma once

#include "pusch_uci_decoder_notifier.h"
#include "srsran/adt/optional.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_notifier.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator_notifier.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "srsran/srsvec/copy.h"

namespace srsran {

namespace detail {

/// \brief PUSCH processor notifier adaptor internal callback.
///
/// Interfaces the different notifiers with the notifier adaptor.
class pusch_processor_notifier_callback
{
public:
  /// Default destructor.
  virtual ~pusch_processor_notifier_callback() = default;

  virtual void on_sch_data(const pusch_decoder_result& result) = 0;

  /// Notifies HARQ-ACK.
  virtual void on_harq_ack(const pusch_uci_field& field) = 0;

  /// Notifies CSI Part 1.
  virtual void on_csi_part1(const pusch_uci_field& field) = 0;

  /// Notifies CSI Part 2.
  virtual void on_csi_part2(const pusch_uci_field& field) = 0;
};

} // namespace detail

class pusch_processor_csi_part1_feedback
{
public:
  virtual ~pusch_processor_csi_part1_feedback() = default;

  virtual void on_csi_part1(const uci_payload_type& part1) = 0;
};

/// \brief Adapts the notifiers of each PUSCH decoder to the PUSCH processor notifier.
///
/// The UCI fields notifier getters set flags of their respective fields as pending.
class pusch_processor_notifier_adaptor : private detail::pusch_processor_notifier_callback
{
public:
  /// Creates a notifier adaptor from a PUSCH processor notifier.
  pusch_processor_notifier_adaptor() :
    pusch_demod_notifier(uci_payload.csi),
    sch_data_notifier(*this),
    harq_ack_notifier(*this),
    csi_part_1_notifier(*this),
    csi_part_2_notifier(*this)
  {
  }

  /// \brief Configures the notifier for a new transmission.
  /// \param[in] notifier_            PUSCH processor result notifier.
  /// \param[in] csi_part_1_feedback_ Uplink control information field CSI Part 1 feedback notifier.
  /// \param[in] csi                  Channel state information.
  /// \return A PUSCH processor notifier adaptor.
  void new_transmission(pusch_processor_result_notifier&    notifier_,
                        pusch_processor_csi_part1_feedback& csi_part_1_feedback_,
                        const channel_state_information&    csi)
  {
    notifier            = &notifier_;
    csi_part_1_feedback = &csi_part_1_feedback_;
    uci_payload.csi     = csi;

    uci_payload.harq_ack.clear();
    uci_payload.csi_part1.clear();
    uci_payload.csi_part2.clear();
  }

  /// Gets the PUSCH demodulator notifier.
  pusch_demodulator_notifier& get_demodulator_notifier() { return pusch_demod_notifier; }

  /// Gets the SCH data notifier.
  pusch_decoder_notifier& get_sch_data_notifier() { return sch_data_notifier; }

  /// Gets the HARQ-ACK notifier and sets the field as pending.
  pusch_uci_decoder_notifier& get_harq_ack_notifier()
  {
    srsran_assert(!pending_harq_ack, "HARQ-ACK has already been requested.");
    pending_harq_ack = true;
    return harq_ack_notifier;
  }

  /// Gets the CSI Part 1 notifier and sets the field as pending.
  pusch_uci_decoder_notifier& get_csi_part1_notifier()
  {
    srsran_assert(!pending_csi_part1, "CSI Part 1 has already been requested.");
    pending_csi_part1 = true;
    return csi_part_1_notifier;
  }

  /// Gets the CSI Part 2 notifier and sets the field as pending.
  pusch_uci_decoder_notifier& get_csi_part2_notifier()
  {
    srsran_assert(!pending_csi_part2, "CSI Part 2 has already been requested.");
    pending_csi_part2 = true;
    return csi_part_2_notifier;
  }

  /// Default destructor - verifies that the previous fields are valid.
  ~pusch_processor_notifier_adaptor() override
  {
    srsran_assert(!pending_harq_ack, "HARQ-ACK has not been notified.");
    srsran_assert(!pending_csi_part1, "CSI Part 1 has not been notified.");
    srsran_assert(!pending_csi_part2, "CSI Part 2 has not been notified.");
  }

private:
  /// Implements the PUSCH demodulator statistics notifier.
  class pusch_demodulator_notifier_impl : public pusch_demodulator_notifier
  {
  public:
    pusch_demodulator_notifier_impl(channel_state_information& csi_) : csi(csi_) {}

    // See interface for documentation.
    void on_provisional_stats(const demodulation_stats& stats) override { update_csi(stats); }

    // See interface for documentation.
    void on_end_stats(const demodulation_stats& stats) override { update_csi(stats); }

  private:
    void update_csi(const demodulation_stats& stats)
    {
      if (stats.sinr_dB.has_value()) {
        csi.set_sinr_dB(channel_state_information::sinr_type::post_equalization, stats.sinr_dB.value());
      }
      if (stats.evm.has_value()) {
        csi.set_evm(stats.evm.value());
        csi.set_sinr_dB(channel_state_information::sinr_type::evm, -20.0F * log10f(stats.evm.value()) - 3.7F);
      }
    }

    channel_state_information& csi;
  };

  /// Implements the SCH data notifier.
  class sch_data_notifier_impl : public pusch_decoder_notifier
  {
  public:
    sch_data_notifier_impl(detail::pusch_processor_notifier_callback& notifier_) : notifier(notifier_) {}

    // See interface for documentation.
    void on_sch_data(const pusch_decoder_result& result) override { notifier.on_sch_data(result); }

  private:
    detail::pusch_processor_notifier_callback& notifier;
  };

  /// Implements the HARQ-ACK notifier.
  class harq_ack_notifier_impl : public pusch_uci_decoder_notifier
  {
  public:
    harq_ack_notifier_impl(detail::pusch_processor_notifier_callback& callback_) : callback(callback_) {}

    // See interface for documentation.
    void on_uci_decoded(span<const uint8_t> message, const uci_status& status) override
    {
      pusch_uci_field field;
      field.payload = uci_payload_type(message.begin(), message.end());
      field.status  = status;
      callback.on_harq_ack(field);
    }

  private:
    detail::pusch_processor_notifier_callback& callback;
  };

  /// Implements the CSI Part 1 notifier.
  class csi_part1_notifier_impl : public pusch_uci_decoder_notifier
  {
  public:
    csi_part1_notifier_impl(detail::pusch_processor_notifier_callback& callback_) : callback(callback_) {}

    // See interface for documentation.
    void on_uci_decoded(span<const uint8_t> message, const uci_status& status) override
    {
      pusch_uci_field field;
      field.payload = uci_payload_type(message.begin(), message.end());
      field.status  = status;
      callback.on_csi_part1(field);
    }

  private:
    detail::pusch_processor_notifier_callback& callback;
  };

  /// Implements the CSI Part 2 notifier.
  class csi_part2_notifier_impl : public pusch_uci_decoder_notifier
  {
  public:
    csi_part2_notifier_impl(detail::pusch_processor_notifier_callback& callback_) : callback(callback_) {}

    // See interface for documentation.
    void on_uci_decoded(span<const uint8_t> message, const uci_status& status) override
    {
      pusch_uci_field field;
      field.payload = uci_payload_type(message.begin(), message.end());
      field.status  = status;
      callback.on_csi_part2(field);
    }

  private:
    detail::pusch_processor_notifier_callback& callback;
  };

  void on_sch_data(const pusch_decoder_result& result) override
  {
    pusch_processor_result_data result_data;
    result_data.data = result;
    result_data.csi  = uci_payload.csi;
    notifier->on_sch(result_data);
  }

  // See detail::pusch_processor_notifier_callback for documentation.
  void on_harq_ack(const pusch_uci_field& field) override
  {
    // Check HARQ ACK is pending.
    srsran_assert(pending_harq_ack, "HARQ ACK is not pending.");

    // Set HARQ ACK field.
    uci_payload.harq_ack = field;

    // HARQ ACK is no longer pending.
    pending_harq_ack = false;

    // Check if the rest of fields are pending and notify the results.
    check_and_notify_uci();
  }

  // See detail::pusch_processor_notifier_callback for documentation.
  void on_csi_part1(const pusch_uci_field& field) override
  {
    // Check CSI Part 1 is pending.
    srsran_assert(pending_csi_part1, "CSI Part 1 is not pending.");

    if (field.status == uci_status::valid) {
      srsran_assert(csi_part_1_feedback != nullptr, "Invalid CSI Part 1 feedback.");
      csi_part_1_feedback->on_csi_part1(field.payload);
    }

    // Set CSI Part 1 field.
    uci_payload.csi_part1 = field;

    // CSI Part 1 is no longer pending.
    pending_csi_part1 = false;

    // Check if the rest of fields are pending and notify the results.
    check_and_notify_uci();
  }

  // See detail::pusch_processor_notifier_callback for documentation.
  void on_csi_part2(const pusch_uci_field& field) override
  {
    // Check CSI Part 2 is pending.
    srsran_assert(pending_csi_part2, "CSI Part 2 is not pending.");

    // Set CSI Part 2 field.
    uci_payload.csi_part2 = field;

    // CSI Part 2 is no longer pending.
    pending_csi_part2 = false;

    // Check if the rest of fields are pending and notify the results.
    check_and_notify_uci();
  }

  /// Check if the no UCI fields are pending and notify the results.
  void check_and_notify_uci()
  {
    srsran_assert(notifier != nullptr, "Invalid notifier.");
    if (!pending_harq_ack && !pending_csi_part1 && !pending_csi_part2) {
      notifier->on_uci(uci_payload);
    }
  }

  /// Set to true if the HARQ-ACK was requested but it has not notified back.
  bool pending_harq_ack = false;
  /// Set to true if the CSI Part 1 was requested but it has not notified back.
  bool pending_csi_part1 = false;
  /// Set to true if the CSI Part 2 was requested but it has not notified back.
  bool pending_csi_part2 = false;
  /// Stored UCI payload.
  pusch_processor_result_control uci_payload;
  /// Reference to PUSCH processor notifier.
  pusch_processor_result_notifier* notifier;
  /// CSI Part 1 feedback.
  pusch_processor_csi_part1_feedback* csi_part_1_feedback;
  /// Channel state information notifier.
  pusch_demodulator_notifier_impl pusch_demod_notifier;
  /// SCH data notifier.
  sch_data_notifier_impl sch_data_notifier;
  /// HARQ-ACK notifier.
  harq_ack_notifier_impl harq_ack_notifier;
  /// CSI Part 1 notifier.
  csi_part1_notifier_impl csi_part_1_notifier;
  /// CSI Part 2 notifier.
  csi_part2_notifier_impl csi_part_2_notifier;
};

} // namespace srsran
