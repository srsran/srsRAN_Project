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

#include "phy_to_fapi_results_event_translator.h"
#include "srsran/fapi/message_builders.h"
#include "srsran/fapi/message_validators.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/math_utils.h"

using namespace srsran;
using namespace fapi_adaptor;

namespace {

class slot_data_message_notifier_dummy : public fapi::slot_data_message_notifier
{
public:
  void on_rx_data_indication(const fapi::rx_data_indication_message& msg) override {}
  void on_crc_indication(const fapi::crc_indication_message& msg) override {}
  void on_uci_indication(const fapi::uci_indication_message& msg) override {}
  void on_srs_indication(const fapi::srs_indication_message& msg) override {}
  void on_rach_indication(const fapi::rach_indication_message& msg) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the PHY-to-FAPI data event translator as a placeholder for the
/// actual data-specific notifier, which will be later set up through the \ref set_slot_data_message_notifier() method.
static slot_data_message_notifier_dummy dummy_data_notifier;

phy_to_fapi_results_event_translator::phy_to_fapi_results_event_translator(srslog::basic_logger& logger_) :
  logger(logger_), data_notifier(dummy_data_notifier)
{
}

void phy_to_fapi_results_event_translator::on_new_prach_results(const ul_prach_results& result)
{
  if (result.result.preambles.empty()) {
    return;
  }

  slot_point slot = result.context.slot;

  // Avoid generating a PRACH indication when all detected preambles have a negative TA value as they must be discarded.
  if (std::all_of(
          result.result.preambles.begin(),
          result.result.preambles.end(),
          [](const prach_detection_result::preamble_indication& ind) { return ind.time_advance.to_seconds() < 0.0; })) {
    logger.warning(
        "All detected PRACH preambles have a negative TA value in slot={}, no PRACH.ind message will be generated",
        slot);
    return;
  }

  fapi::rach_indication_message         msg;
  fapi::rach_indication_message_builder builder(msg);

  builder.set_basic_parameters(slot.sfn(), slot.slot_index());

  // NOTE: Currently not managing handle.
  static constexpr unsigned handle = 0U;
  // NOTE: Currently not supporting PRACH multiplexed in frequency domain.
  static constexpr unsigned fd_ra_index = 0U;
  // NOTE: Clamp values defined in SCF-222 v4.0 Section 3.4.11 Table RACH.indication message body.
  static constexpr float            MIN_AVG_RSSI_VALUE = -140.F;
  static constexpr float            MAX_AVG_RSSI_VALUE = 30.F;
  fapi::rach_indication_pdu_builder builder_pdu =
      builder.add_pdu(handle,
                      result.context.start_symbol,
                      slot.slot_index(),
                      fd_ra_index,
                      clamp(result.result.rssi_dB, MIN_AVG_RSSI_VALUE, MAX_AVG_RSSI_VALUE),
                      {},
                      {});

  for (const auto& preamble : result.result.preambles) {
    // NOTE: Clamp values defined in SCF-222 v4.0 Section 3.4.11 Table RACH.indication message body.
    static constexpr float MIN_PREAMBLE_POWER_VALUE = -140.F;
    static constexpr float MAX_PREAMBLE_POWER_VALUE = 30.F;
    static constexpr float MIN_PREAMBLE_SNR_VALUE   = -64.F;
    static constexpr float MAX_PREAMBLE_SNR_VALUE   = 63.F;

    double TA_ns = preamble.time_advance.to_seconds() * 1e9;
    // Ignore preambles with a negative TA value.
    if (TA_ns < 0.0) {
      logger.warning("Detected PRACH preamble in slot={} has a negative TA value of {}ns, skipping it", slot, TA_ns);
      continue;
    }

    builder_pdu.add_preamble(
        preamble.preamble_index,
        {},
        TA_ns,
        clamp(convert_power_to_dB(preamble.detection_metric), MIN_PREAMBLE_POWER_VALUE, MAX_PREAMBLE_POWER_VALUE),
        clamp(convert_power_to_dB(preamble.detection_metric), MIN_PREAMBLE_SNR_VALUE, MAX_PREAMBLE_SNR_VALUE));
  }

  error_type<fapi::validator_report> validation_result = validate_rach_indication(msg);
  if (!validation_result) {
    log_validator_report(validation_result.error(), logger);
    return;
  }

  data_notifier.get().on_rach_indication(msg);
}

void phy_to_fapi_results_event_translator::on_new_pusch_results_control(const ul_pusch_results_control& result)
{
  notify_pusch_uci_indication(result);
}

void phy_to_fapi_results_event_translator::on_new_pusch_results_data(const ul_pusch_results_data& result)
{
  notify_crc_indication(result);
  notify_rx_data_indication(result);
}

/// Converts and returns the given UCI status to FAPI UCI STATUS.
static uci_pusch_or_pucch_f2_3_4_detection_status to_fapi_uci_detection_status(uci_status status)
{
  switch (status) {
    case uci_status::invalid:
      return uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure;
    case uci_status::valid:
      return uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
    case uci_status::unknown:
    default:
      return uci_pusch_or_pucch_f2_3_4_detection_status::dtx;
  }
}

void phy_to_fapi_results_event_translator::notify_pusch_uci_indication(const ul_pusch_results_control& result)
{
  fapi::uci_indication_message         msg;
  fapi::uci_indication_message_builder builder(msg);

  builder.set_basic_parameters(result.slot.sfn(), result.slot.slot_index());

  // Do not manage handle.
  static constexpr unsigned   handle      = 0;
  fapi::uci_pusch_pdu_builder builder_pdu = builder.add_pusch_pdu(handle, result.rnti);

  const channel_state_information& csi_info = result.csi;

  // NOTE: Clamp values defined in SCF-222 v4.0 Section 3.4.9.1 Table UCI PUSCH PDU.
  static constexpr float MIN_UL_SINR_VALUE = -65.534;
  static constexpr float MAX_UL_SINR_VALUE = 65.534;

  builder_pdu.set_metrics_parameters(clamp(csi_info.get_sinr_dB(), MIN_UL_SINR_VALUE, MAX_UL_SINR_VALUE),
                                     {},
                                     optional<int>(result.csi.get_time_alignment().to_seconds() * 1e9),
                                     {},
                                     {});

  // Add the HARQ section.
  if (result.harq_ack.has_value()) {
    const pusch_uci_field&                     harq   = result.harq_ack.value();
    uci_pusch_or_pucch_f2_3_4_detection_status status = to_fapi_uci_detection_status(harq.status);
    builder_pdu.set_harq_parameters(status,
                                    harq.payload.size(),
                                    (status == uci_pusch_or_pucch_f2_3_4_detection_status::dtx ||
                                     status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure)
                                        ? bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>()
                                        : harq.payload);
  }

  // Add the CSI1 section.
  if (result.csi1.has_value()) {
    const pusch_uci_field&                     csi1   = result.csi1.value();
    uci_pusch_or_pucch_f2_3_4_detection_status status = to_fapi_uci_detection_status(csi1.status);
    builder_pdu.set_csi_part1_parameters(status,
                                         csi1.payload.size(),
                                         (status == uci_pusch_or_pucch_f2_3_4_detection_status::dtx ||
                                          status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure)
                                             ? bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>()
                                             : csi1.payload);
  }

  // Add the CSI2 section.
  if (result.csi2.has_value()) {
    const pusch_uci_field&                     csi2   = result.csi2.value();
    uci_pusch_or_pucch_f2_3_4_detection_status status = to_fapi_uci_detection_status(csi2.status);
    builder_pdu.set_csi_part2_parameters(status,
                                         csi2.payload.size(),
                                         (status == uci_pusch_or_pucch_f2_3_4_detection_status::dtx ||
                                          status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure)
                                             ? bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>()
                                             : csi2.payload);
  }

  error_type<fapi::validator_report> validation_result = validate_uci_indication(msg);
  if (!validation_result) {
    log_validator_report(validation_result.error(), logger);
    return;
  }

  data_notifier.get().on_uci_indication(msg);
}

void phy_to_fapi_results_event_translator::notify_crc_indication(const ul_pusch_results_data& result)
{
  fapi::crc_indication_message         msg;
  fapi::crc_indication_message_builder builder(msg);

  builder.set_basic_parameters(result.slot.sfn(), result.slot.slot_index());

  // Handle is not supported for now.
  unsigned handle = 0;
  // CB CRC status is not supported for now.
  unsigned num_cb = 0;

  // NOTE: Clamp values defined in SCF-222 v4.0 Section 3.4.8 Table CRC.indication message body.
  static constexpr float MIN_UL_SINR_VALUE = -65.534;
  static constexpr float MAX_UL_SINR_VALUE = 65.534;

  builder.add_pdu(handle,
                  result.rnti,
                  optional<uint8_t>(),
                  result.harq_id,
                  result.decoder_result.tb_crc_ok,
                  num_cb,
                  {},
                  clamp(result.csi.get_sinr_dB(), MIN_UL_SINR_VALUE, MAX_UL_SINR_VALUE),
                  {},
                  optional<int>(result.csi.get_time_alignment().to_seconds() * 1e9),
                  {},
                  {});

  error_type<fapi::validator_report> validation_result = validate_crc_indication(msg);
  if (!validation_result) {
    log_validator_report(validation_result.error(), logger);
    return;
  }

  data_notifier.get().on_crc_indication(msg);
}

void phy_to_fapi_results_event_translator::notify_rx_data_indication(const ul_pusch_results_data& result)
{
  fapi::rx_data_indication_message         msg;
  fapi::rx_data_indication_message_builder builder(msg);

  // Uplink CP/UP plane separation is not supported for now.
  unsigned control_length = 0;
  builder.set_basic_parameters(result.slot.sfn(), result.slot.slot_index(), control_length);

  // Handle is not supported for now.
  unsigned handle = 0;
  builder.add_custom_pdu(handle, result.rnti, optional<unsigned>(), result.harq_id, result.payload);

  error_type<fapi::validator_report> validation_result = validate_rx_data_indication(msg);
  if (!validation_result) {
    log_validator_report(validation_result.error(), logger);
    return;
  }

  data_notifier.get().on_rx_data_indication(msg);
}

/// Fills the SR parameters for PUCCH Format 0 or Format 1 using the given builder and result.
static void fill_format_0_1_sr(fapi::uci_pucch_pdu_format_0_1_builder& builder, const ul_pucch_results& result)
{
  srsran_assert(result.context.context_f0_f1.has_value(), "Context for PUCCH Format 0 or Format 1 is empty");

  const ul_pucch_f0_f1_context& context = result.context.context_f0_f1.value();
  // Do nothing when there is no SR opportunity.
  if (!context.is_sr_opportunity) {
    return;
  }

  // Set the SR detection status based on the UCI status.
  const pucch_uci_message& msg = result.processor_result.message;
  builder.set_sr_parameters(msg.get_status() == uci_status::valid, {});
}

/// Fills the HARQ parameters for PUCCH Format 0 or Format 1 using the given builder and message.
static void fill_format_0_1_harq(fapi::uci_pucch_pdu_format_0_1_builder& builder, const pucch_uci_message& message)
{
  unsigned nof_harq_bits = message.get_expected_nof_harq_ack_bits();
  if (nof_harq_bits == 0) {
    return;
  }

  // Initialize with DTX.
  static_vector<uci_pucch_f0_or_f1_harq_values, fapi::uci_harq_format_0_1::MAX_NUM_HARQ> harq(
      nof_harq_bits, uci_pucch_f0_or_f1_harq_values::dtx);

  // Write the contents when the uci status is valid.
  if (message.get_status() == uci_status::valid) {
    for (unsigned i = 0; i != nof_harq_bits; ++i) {
      harq[i] = (message.get_harq_ack_bits()[i] == 1U) ? uci_pucch_f0_or_f1_harq_values::ack
                                                       : uci_pucch_f0_or_f1_harq_values::nack;
    }
  }

  // Write the parameters using the builder.
  builder.set_harq_parameters({}, harq);
}

/// Adds a PUCCH Format 0 or Format 1 PDU to the given builder using the data provided by result.
static void add_format_0_1_pucch_pdu(fapi::uci_indication_message_builder& builder, const ul_pucch_results& result)
{
  // Do not use the handle for now.
  static const unsigned                  handle  = 0;
  const ul_pucch_context&                context = result.context;
  fapi::uci_pucch_pdu_format_0_1_builder builder_format01 =
      builder.add_format_0_1_pucch_pdu(handle, context.rnti, context.format);

  const channel_state_information& csi_info = result.processor_result.csi;

  // NOTE: Clamp values defined in SCF-222 v4.0 Section 3.4.9.2 Table UCI PUCCH format 0 or 1 PDU.
  static constexpr float MIN_UL_SINR_VALUE = -65.534;
  static constexpr float MAX_UL_SINR_VALUE = 65.534;

  builder_format01.set_metrics_parameters(
      clamp(csi_info.get_sinr_dB(), MIN_UL_SINR_VALUE, MAX_UL_SINR_VALUE),
      {},
      optional<int>(result.processor_result.csi.get_time_alignment().to_seconds() * 1e9),
      {},
      {});

  // Fill SR parameters.
  fill_format_0_1_sr(builder_format01, result);

  // Fill HARQ parameters.
  fill_format_0_1_harq(builder_format01, result.processor_result.message);
}

/// Fills the HARQ parameters for PUCCH Format 2/3/4 using the given builder and message.
static void fill_format_2_3_4_harq(fapi::uci_pucch_pdu_format_2_3_4_builder& builder, const pucch_uci_message& message)
{
  units::bits harq_len = units::bits(message.get_expected_nof_harq_ack_bits());
  if (harq_len.value() == 0) {
    return;
  }

  uci_pusch_or_pucch_f2_3_4_detection_status status = to_fapi_uci_detection_status(message.get_status());

  // Write an empty payload on detection failure.
  if (status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure ||
      status == uci_pusch_or_pucch_f2_3_4_detection_status::dtx) {
    builder.set_harq_parameters(status, harq_len.value(), {});
    return;
  }

  builder.set_harq_parameters(status,
                              harq_len.value(),
                              bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>(message.get_harq_ack_bits().begin(),
                                                                               message.get_harq_ack_bits().end()));
}

/// Fills the SR parameters for PUCCH Format 2/3/4 using the given builder and message.
static void fill_format_2_3_4_sr(fapi::uci_pucch_pdu_format_2_3_4_builder& builder, const pucch_uci_message& message)
{
  if (message.get_status() != uci_status::valid) {
    return;
  }

  units::bits sr_len = units::bits(message.get_expected_nof_sr_bits());

  if (sr_len.value() == 0) {
    return;
  }

  builder.set_sr_parameters(sr_len.value(),
                            bounded_bitset<fapi::sr_pdu_format_2_3_4::MAX_SR_PAYLOAD_SIZE_BITS>(
                                message.get_sr_bits().begin(), message.get_sr_bits().end()));
}

/// Fills the CSI Part 1 parameters for PUCCH Format 2/3/4 using the given builder and message.
static void fill_format_2_3_4_csi_part1(fapi::uci_pucch_pdu_format_2_3_4_builder& builder,
                                        const pucch_uci_message&                  message)
{
  units::bits csi_len = units::bits(message.get_expected_nof_csi_part1_bits());
  if (csi_len.value() == 0) {
    return;
  }

  uci_pusch_or_pucch_f2_3_4_detection_status status = to_fapi_uci_detection_status(message.get_status());

  // Write an empty payload on detection failure.
  if (status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure ||
      status == uci_pusch_or_pucch_f2_3_4_detection_status::dtx) {
    builder.set_csi_part1_parameters(status, csi_len.value(), {});
    return;
  }

  builder.set_csi_part1_parameters(status,
                                   csi_len.value(),
                                   bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>(
                                       message.get_csi_part1_bits().begin(), message.get_csi_part1_bits().end()));
}

/// Adds a PUCCH Format 2 PDU to the given builder using the data provided by result.
static void add_format_2_pucch_pdu(fapi::uci_indication_message_builder& builder, const ul_pucch_results& result)
{
  // Do not use the handle for now.
  static const unsigned                    handle = 0;
  fapi::uci_pucch_pdu_format_2_3_4_builder builder_format234 =
      builder.add_format_2_3_4_pucch_pdu(handle, result.context.rnti, result.context.format);

  const channel_state_information& csi_info = result.processor_result.csi;

  // NOTE: Clamp values defined in SCF-222 v4.0 Section 3.4.9.3 Table UCI PUCCH format 2, 3 or 4 PDU.
  static constexpr float MIN_UL_SINR_VALUE = -65.534;
  static constexpr float MAX_UL_SINR_VALUE = 65.534;

  builder_format234.set_metrics_parameters(
      clamp(csi_info.get_sinr_dB(), MIN_UL_SINR_VALUE, MAX_UL_SINR_VALUE),
      {},
      optional<int>(result.processor_result.csi.get_time_alignment().to_seconds() * 1e9),
      {},
      {});

  // Fill SR parameters.
  fill_format_2_3_4_sr(builder_format234, result.processor_result.message);

  // Fill HARQ parameters.
  fill_format_2_3_4_harq(builder_format234, result.processor_result.message);

  // Fill CSI Part 1 parameters.
  fill_format_2_3_4_csi_part1(builder_format234, result.processor_result.message);
}

void phy_to_fapi_results_event_translator::on_new_pucch_results(const ul_pucch_results& result)
{
  fapi::uci_indication_message         msg;
  fapi::uci_indication_message_builder builder(msg);

  const ul_pucch_context& context = result.context;
  builder.set_basic_parameters(context.slot.sfn(), context.slot.slot_index());

  switch (context.format) {
    case pucch_format::FORMAT_0:
    case pucch_format::FORMAT_1:
      add_format_0_1_pucch_pdu(builder, result);
      break;
    case pucch_format::FORMAT_2:
      add_format_2_pucch_pdu(builder, result);
      break;
    default:
      srsran_assert(0, "Unexpected PUCCH format {}", context.format);
  }

  error_type<fapi::validator_report> validation_result = validate_uci_indication(msg);
  if (!validation_result) {
    log_validator_report(validation_result.error(), logger);
    return;
  }

  data_notifier.get().on_uci_indication(msg);
}
