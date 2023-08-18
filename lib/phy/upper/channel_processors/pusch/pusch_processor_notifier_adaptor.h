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
#include "pusch_uci_decoder_notifier.h"
#include "srsran/adt/optional.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_notifier.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "srsran/srsvec/copy.h"

namespace srsran {

namespace detail {

/// \brief PUSCH processor notifier adaptor internal UCI callback.
///
/// Interfaces the different UCI notifiers with the notifier adaptor.
class pusch_processor_notifier_uci_callback
{
public:
  /// Default destructor.
  virtual ~pusch_processor_notifier_uci_callback() = default;

  /// Notifies HARQ-ACK.
  virtual void on_harq_ack(const pusch_uci_field& field) = 0;

  /// Notifies CSI Part 1.
  virtual void on_csi_part1(const pusch_uci_field& field) = 0;

  /// Notifies CSI Part 2.
  virtual void on_csi_part2(const pusch_uci_field& field) = 0;
};

} // namespace detail

/// \brief Adapts the PUSCH decoders notifiers to the PUSCH processor notifier.
///
/// The UCI fields notifier getters set flags of their respective fields as pending.
class pusch_processor_notifier_adaptor : private detail::pusch_processor_notifier_uci_callback
{
public:
  /// Creates a notifier adaptor from a PUSCH processor notifier.
  pusch_processor_notifier_adaptor(pusch_processor_result_notifier& notifier_) :
    notifier(notifier_),
    sch_data_notifier(notifier),
    harq_ack_notifier(*this),
    csi_part_1_notifier(*this),
    csi_part_2_notifier(*this)
  {
    uci_payload.harq_ack.clear();
    uci_payload.csi_part1.clear();
    uci_payload.csi_part2.clear();
  }

  /// Gets the SCH data notifier.
  pusch_decoder_notifier& get_sch_data_notifier() { return sch_data_notifier; }

  /// Gets the HARQ-ACK notifier and sets the field as pending.
  pusch_uci_decoder_notifier& get_harq_ack_notifier()
  {
    srsran_assert(!pending_csi_part1, "HARQ-ACK has already been requested.");
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

  ~pusch_processor_notifier_adaptor()
  {
    srsran_assert(!pending_harq_ack, "HARQ-ACK has not been notified.");
    srsran_assert(!pending_csi_part1, "CSI Part 1 has not been notified.");
    srsran_assert(!pending_csi_part2, "CSI Part 2 has not been notified.");
  }

private:
  /// Implements the SCH data notifier.
  class sch_data_notifier_impl : public pusch_decoder_notifier
  {
  public:
    sch_data_notifier_impl(pusch_processor_result_notifier& notifier_) : notifier(notifier_) {}

    void on_sch_data(const pusch_decoder_result& result) override
    {
      pusch_processor_result_data result_data;
      result_data.data = result;
      notifier.on_sch(result_data);
    }

  private:
    pusch_processor_result_notifier& notifier;
  };

  /// Implements the HARQ-ACK notifier.
  class harq_ack_notifier_impl : public pusch_uci_decoder_notifier
  {
  public:
    harq_ack_notifier_impl(detail::pusch_processor_notifier_uci_callback& callback_) : callback(callback_) {}

    // See interface for documentation.
    void on_uci_decoded(span<const uint8_t> message, const uci_status& status) override
    {
      pusch_uci_field field;
      field.payload.resize(message.size());
      srsvec::copy(field.payload, message);
      field.status = status;
      callback.on_harq_ack(field);
    }

  private:
    detail::pusch_processor_notifier_uci_callback& callback;
  };

  /// Implements the CSI Part 1 notifier.
  class csi_part1_notifier_impl : public pusch_uci_decoder_notifier
  {
  public:
    csi_part1_notifier_impl(detail::pusch_processor_notifier_uci_callback& callback_) : callback(callback_) {}

    // See interface for documentation.
    void on_uci_decoded(span<const uint8_t> message, const uci_status& status) override
    {
      pusch_uci_field field;
      field.payload.resize(message.size());
      srsvec::copy(field.payload, message);
      field.status = status;
      callback.on_csi_part1(field);
    }

  private:
    detail::pusch_processor_notifier_uci_callback& callback;
  };

  /// Implements the CSI Part 2 notifier.
  class csi_part2_notifier_impl : public pusch_uci_decoder_notifier
  {
  public:
    csi_part2_notifier_impl(detail::pusch_processor_notifier_uci_callback& callback_) : callback(callback_) {}

    // See interface for documentation.
    void on_uci_decoded(span<const uint8_t> message, const uci_status& status) override
    {
      pusch_uci_field field;
      field.payload.resize(message.size());
      srsvec::copy(field.payload, message);
      field.status = status;
      callback.on_csi_part2(field);
    }

  private:
    detail::pusch_processor_notifier_uci_callback& callback;
  };

  // See detail::pusch_processor_notifier_uci_callback for documentation.
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

  // See detail::pusch_processor_notifier_uci_callback for documentation.
  void on_csi_part1(const pusch_uci_field& field) override
  {
    // Check CSI Part 1 is pending.
    srsran_assert(pending_csi_part1, "CSI Part 1 is not pending.");

    // Set CSI Part 1 field.
    uci_payload.csi_part1 = field;

    // CSI Part 1 is no longer pending.
    pending_csi_part1 = false;

    // Check if the rest of fields are pending and notify the results.
    check_and_notify_uci();
  }

  // See detail::pusch_processor_notifier_uci_callback for documentation.
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
    if (!pending_harq_ack && !pending_csi_part1 && !pending_csi_part2) {
      notifier.on_uci(uci_payload);
    }
  }

  /// Reference to PUSCH processor notifier.
  pusch_processor_result_notifier& notifier;
  /// SCH data notifier.
  sch_data_notifier_impl sch_data_notifier;
  /// HARQ-ACK notifier.
  harq_ack_notifier_impl harq_ack_notifier;
  /// CSI Part 1 notifier.
  csi_part1_notifier_impl csi_part_1_notifier;
  /// CSI Part 2 notifier.
  csi_part2_notifier_impl csi_part_2_notifier;
  /// Set to true if the HARQ-ACK was requested but it has not notified back.
  bool pending_harq_ack = false;
  /// Set to true if the CSI Part 1 was requested but it has not notified back.
  bool pending_csi_part1 = false;
  /// Set to true if the CSI Part 2 was requested but it has not notified back.
  bool pending_csi_part2 = false;
  /// Stored UCI payload.
  pusch_processor_result_control uci_payload;
};

} // namespace srsran
