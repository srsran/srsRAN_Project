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

#include "fapi_to_mac_data_msg_translator.h"
#include "srsran/fapi/messages.h"

using namespace srsran;
using namespace fapi_adaptor;

namespace {

class mac_cell_rach_handler_dummy : public mac_cell_rach_handler
{
public:
  void handle_rach_indication(const mac_rach_indication& rach_ind) override {}
};

class mac_pdu_handler_dummy : public mac_pdu_handler
{
public:
  void handle_rx_data_indication(mac_rx_data_indication pdu) override {}
};

class mac_cell_control_information_handler_dummy : public mac_cell_control_information_handler
{
public:
  void handle_crc(const mac_crc_indication_message& msg) override {}

  void handle_uci(const mac_uci_indication_message& msg) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the FAPI-to-MAC message translator as a placeholder for the
/// actual, cell-specific MAC RACH handler, which will be later set up through the \ref set_cell_rach_handler() method.
static mac_cell_rach_handler_dummy dummy_mac_rach_handler;

/// This dummy object is passed to the constructor of the FAPI-to-MAC message translator as a placeholder for the
/// actual, cell-specific MAC PDU handler, which will be later set up through the \ref set_cell_pdu_handler() method.
static mac_pdu_handler_dummy dummy_pdu_handler;

/// This dummy object is passed to the constructor of the FAPI-to-MAC message translator as a placeholder for the
/// actual, cell-specific MAC CRC handler, which will be later set up through the \ref set_cell_crc_handler() method.
static mac_cell_control_information_handler_dummy dummy_crc_handler;

/// Converts the given FAPI UCI SINR to dBs as per SCF-222 v4.0 section 3.4.9.
static float to_uci_ul_sinr(int sinr)
{
  return static_cast<float>(sinr) * 0.002F;
}

/// Converts the given FAPI UCI RSSI to dBs as per SCF-222 v4.0 section 3.4.9.
static float to_uci_ul_rssi(unsigned rssi)
{
  return static_cast<float>(rssi - 1280) * 0.1F;
}

/// Converts the given FAPI UCI RSRP to dBs as per SCF-222 v4.0 section 3.4.9.
static float to_uci_ul_rsrp(unsigned rsrp)
{
  return static_cast<float>(rsrp - 1400) * 0.1F;
}

/// Converts the given FAPI UCI SINR to dBs as per SCF-222 v4.0 section 3.4.9.
static optional<float> convert_fapi_to_mac_ul_sinr(int16_t fapi_ul_sinr)
{
  if (fapi_ul_sinr != std::numeric_limits<decltype(fapi_ul_sinr)>::min()) {
    return to_uci_ul_sinr(fapi_ul_sinr);
  }
  return nullopt;
}

fapi_to_mac_data_msg_translator::fapi_to_mac_data_msg_translator(subcarrier_spacing scs_) :
  scs(scs_), rach_handler(dummy_mac_rach_handler), pdu_handler(dummy_pdu_handler), crc_handler(dummy_crc_handler)
{
}

void fapi_to_mac_data_msg_translator::on_rx_data_indication(const fapi::rx_data_indication_message& msg)
{
  mac_rx_data_indication indication;
  indication.sl_rx      = slot_point(scs, msg.sfn, msg.slot);
  indication.cell_index = to_du_cell_index(0);
  for (const auto& fapi_pdu : msg.pdus) {
    // PDUs that were not successfully decoded have zero length.
    if (fapi_pdu.pdu_length == 0) {
      continue;
    }

    auto pdu_buffer = byte_buffer::create(span<const uint8_t>(fapi_pdu.data, fapi_pdu.pdu_length));
    if (pdu_buffer.is_error()) {
      srslog::fetch_basic_logger("FAPI").warning("Unable to allocate memory for MAC RX PDU");
      // Avoid new buffer allocations for the same FAPI PDU.
      break;
    }

    mac_rx_pdu& pdu = indication.pdus.emplace_back();
    pdu.harq_id     = fapi_pdu.harq_id;
    pdu.rnti        = fapi_pdu.rnti;
    pdu.pdu         = std::move(pdu_buffer.value());
  }

  // Only invoke the MAC when there are successfully decoded PDUs available.
  if (!indication.pdus.empty()) {
    pdu_handler.get().handle_rx_data_indication(indication);
  }
}

void fapi_to_mac_data_msg_translator::on_crc_indication(const fapi::crc_indication_message& msg)
{
  mac_crc_indication_message indication;
  indication.sl_rx = slot_point(scs, msg.sfn, msg.slot);

  for (const auto& fapi_pdu : msg.pdus) {
    mac_crc_pdu& pdu = indication.crcs.emplace_back();

    pdu.harq_id        = fapi_pdu.harq_id;
    pdu.rnti           = fapi_pdu.rnti;
    pdu.tb_crc_success = fapi_pdu.tb_crc_status_ok;
    pdu.ul_sinr_metric = convert_fapi_to_mac_ul_sinr(fapi_pdu.ul_sinr_metric);
    if (fapi_pdu.timing_advance_offset_ns != std::numeric_limits<decltype(fapi_pdu.timing_advance_offset_ns)>::min()) {
      pdu.time_advance_offset = phy_time_unit::from_seconds(fapi_pdu.timing_advance_offset_ns * 1e-9);
    }
  }

  crc_handler.get().handle_crc(indication);
}

/// Converts the given FAPI Timing Advance Offset in nanoseconds to Physical layer time unit.
static void convert_fapi_to_mac_ta_offset(optional<phy_time_unit>& mac_ta_offset, int16_t fapi_ta_offset_ns)
{
  if (fapi_ta_offset_ns != std::numeric_limits<decltype(fapi_ta_offset_ns)>::min()) {
    mac_ta_offset = phy_time_unit::from_seconds(static_cast<float>(fapi_ta_offset_ns) * 1e-9);
  }
}

/// Converts the given FAPI UCI RSSI to dBs as per SCF-222 v4.0 section 3.4.9.
static optional<float> convert_fapi_to_mac_rssi(uint16_t fapi_rssi)
{
  if (fapi_rssi != std::numeric_limits<decltype(fapi_rssi)>::max()) {
    return to_uci_ul_rssi(fapi_rssi);
  }
  return nullopt;
}

/// Converts the given FAPI UCI RSRP to dBs as per SCF-222 v4.0 section 3.4.9.
static optional<float> convert_fapi_to_mac_rsrp(uint16_t fapi_rsrp)
{
  if (fapi_rsrp != std::numeric_limits<decltype(fapi_rsrp)>::max()) {
    return to_uci_ul_rsrp(fapi_rsrp);
  }
  return nullopt;
}

/// Returns true if the UCI payload is valid given a FAPI detection status.
static bool is_fapi_uci_payload_valid(uci_pusch_or_pucch_f2_3_4_detection_status status)
{
  return (status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass ||
          status == uci_pusch_or_pucch_f2_3_4_detection_status::no_dtx);
}

static void convert_fapi_to_mac_pucch_f0_f1_uci_ind(mac_uci_pdu::pucch_f0_or_f1_type&     mac_pucch,
                                                    const fapi::uci_pucch_pdu_format_0_1& fapi_pucch)
{
  mac_pucch.ul_sinr = convert_fapi_to_mac_ul_sinr(fapi_pucch.ul_sinr_metric);
  mac_pucch.rssi    = convert_fapi_to_mac_rssi(fapi_pucch.rssi);
  mac_pucch.rsrp    = convert_fapi_to_mac_rsrp(fapi_pucch.rsrp);
  convert_fapi_to_mac_ta_offset(mac_pucch.time_advance_offset, fapi_pucch.timing_advance_offset_ns);

  // Fill SR.
  if (fapi_pucch.pdu_bitmap.test(fapi::uci_pucch_pdu_format_0_1::SR_BIT)) {
    mac_pucch.sr_info.emplace().detected = fapi_pucch.sr.sr_indication;
  }

  // Fill HARQ.
  if (fapi_pucch.pdu_bitmap.test(fapi::uci_pucch_pdu_format_0_1::HARQ_BIT)) {
    mac_pucch.harq_info.emplace().harqs.assign(fapi_pucch.harq.harq_values.begin(), fapi_pucch.harq.harq_values.end());
  }
}

static void convert_fapi_to_mac_pusch_uci_ind(mac_uci_pdu::pusch_type& mac_pusch, const fapi::uci_pusch_pdu& fapi_pusch)
{
  mac_pusch.ul_sinr = convert_fapi_to_mac_ul_sinr(fapi_pusch.ul_sinr_metric);
  mac_pusch.rssi    = convert_fapi_to_mac_rssi(fapi_pusch.rssi);
  mac_pusch.rsrp    = convert_fapi_to_mac_rsrp(fapi_pusch.rsrp);
  convert_fapi_to_mac_ta_offset(mac_pusch.time_advance_offset, fapi_pusch.timing_advance_offset_ns);

  // Fill HARQ.
  if (fapi_pusch.pdu_bitmap.test(fapi::uci_pusch_pdu::HARQ_BIT)) {
    if (fapi_pusch.harq.payload.empty()) {
      mac_pusch.harq_info.emplace(
          mac_uci_pdu::pusch_type::harq_information::create_undetected_harq_info(fapi_pusch.harq.expected_bit_length));
    } else {
      mac_pusch.harq_info.emplace(
          mac_uci_pdu::pusch_type::harq_information::create_detected_harq_info(fapi_pusch.harq.payload));
    }
  }

  // Fill CSI Part 1.
  if (fapi_pusch.pdu_bitmap.test(fapi::uci_pusch_pdu::CSI_PART1_BIT)) {
    mac_uci_pdu::pusch_type::csi_information& csi = mac_pusch.csi_part1_info.emplace();
    csi.is_valid                                  = is_fapi_uci_payload_valid(fapi_pusch.csi_part1.detection_status);
    csi.payload                                   = fapi_pusch.csi_part1.payload;
  }

  // Fill CSI Part 2.
  if (fapi_pusch.pdu_bitmap.test(fapi::uci_pusch_pdu::CSI_PART2_BIT)) {
    mac_uci_pdu::pusch_type::csi_information& csi = mac_pusch.csi_part2_info.emplace();
    csi.is_valid                                  = is_fapi_uci_payload_valid(fapi_pusch.csi_part2.detection_status);
    csi.payload                                   = fapi_pusch.csi_part2.payload;
  }
}

static void convert_fapi_to_mac_pucch_f2_f3_f4_uci_ind(mac_uci_pdu::pucch_f2_or_f3_or_f4_type& mac_pucch,
                                                       const fapi::uci_pucch_pdu_format_2_3_4& fapi_pucch)
{
  mac_pucch.ul_sinr = convert_fapi_to_mac_ul_sinr(fapi_pucch.ul_sinr_metric);
  mac_pucch.rssi    = convert_fapi_to_mac_rssi(fapi_pucch.rssi);
  mac_pucch.rsrp    = convert_fapi_to_mac_rsrp(fapi_pucch.rsrp);
  convert_fapi_to_mac_ta_offset(mac_pucch.time_advance_offset, fapi_pucch.timing_advance_offset_ns);

  // Fill SR.
  if (fapi_pucch.pdu_bitmap.test(fapi::uci_pucch_pdu_format_2_3_4::SR_BIT)) {
    mac_pucch.sr_info.emplace() = fapi_pucch.sr.sr_payload;
  }

  // Fill HARQ.
  if (fapi_pucch.pdu_bitmap.test(fapi::uci_pucch_pdu_format_2_3_4::HARQ_BIT)) {
    if (fapi_pucch.harq.payload.empty()) {
      mac_pucch.harq_info.emplace(mac_uci_pdu::pucch_f2_or_f3_or_f4_type::harq_information::create_undetected_harq_info(
          fapi_pucch.harq.expected_bit_length));
    } else {
      mac_pucch.harq_info.emplace(
          mac_uci_pdu::pucch_f2_or_f3_or_f4_type::harq_information::create_detected_harq_info(fapi_pucch.harq.payload));
    }
  }

  // Fill CSI Part 1.
  if (fapi_pucch.pdu_bitmap.test(fapi::uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT)) {
    mac_uci_pdu::pucch_f2_or_f3_or_f4_type::csi_information& csi = mac_pucch.csi_part1_info.emplace();
    csi.is_valid = is_fapi_uci_payload_valid(fapi_pucch.csi_part1.detection_status);
    csi.payload  = fapi_pucch.csi_part1.payload;
  }

  // Fill CSI Part 2.
  if (fapi_pucch.pdu_bitmap.test(fapi::uci_pucch_pdu_format_2_3_4::CSI_PART2_BIT)) {
    mac_uci_pdu::pucch_f2_or_f3_or_f4_type::csi_information& csi = mac_pucch.csi_part2_info.emplace();
    csi.is_valid = is_fapi_uci_payload_valid(fapi_pucch.csi_part2.detection_status);
    csi.payload  = fapi_pucch.csi_part2.payload;
  }
}

void fapi_to_mac_data_msg_translator::on_uci_indication(const fapi::uci_indication_message& msg)
{
  mac_uci_indication_message mac_msg;
  mac_msg.sl_rx = slot_point(scs, msg.sfn, msg.slot);
  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::uci_pdu_type::PUSCH: {
        mac_uci_pdu& mac_pdu = mac_msg.ucis.emplace_back();
        mac_pdu.rnti         = to_rnti(pdu.pusch_pdu.rnti);
        mac_uci_pdu::pusch_type pusch;
        convert_fapi_to_mac_pusch_uci_ind(pusch, pdu.pusch_pdu);
        mac_pdu.pdu = pusch;
        break;
      }
      case fapi::uci_pdu_type::PUCCH_format_0_1: {
        mac_uci_pdu& mac_pdu = mac_msg.ucis.emplace_back();
        mac_pdu.rnti         = to_rnti(pdu.pucch_pdu_f01.rnti);
        mac_uci_pdu::pucch_f0_or_f1_type pucch;
        convert_fapi_to_mac_pucch_f0_f1_uci_ind(pucch, pdu.pucch_pdu_f01);
        mac_pdu.pdu = pucch;
        break;
      }
      case fapi::uci_pdu_type::PUCCH_format_2_3_4: {
        mac_uci_pdu& mac_pdu = mac_msg.ucis.emplace_back();
        mac_pdu.rnti         = to_rnti(pdu.pucch_pdu_f234.rnti);
        mac_uci_pdu::pucch_f2_or_f3_or_f4_type pucch;
        convert_fapi_to_mac_pucch_f2_f3_f4_uci_ind(pucch, pdu.pucch_pdu_f234);
        mac_pdu.pdu = pucch;
        break;
      }
    }
  }

  crc_handler.get().handle_uci(mac_msg);
}

void fapi_to_mac_data_msg_translator::on_srs_indication(const fapi::srs_indication_message& msg) {}

/// Converts the given FAPI RACH occasion RSSI to dBs as per SCF-222 v4.0 section 3.4.11.
static float to_prach_rssi_dB(int fapi_rssi)
{
  return (fapi_rssi - 140000) * 0.001F;
}

/// Converts the given FAPI RACH preamble power to dBs as per SCF-222 v4.0 section 3.4.11.
static float to_prach_preamble_power_dB(int fapi_power)
{
  return static_cast<float>(fapi_power - 140000) * 0.001F;
}

/// Converts the given FAPI RACH preamble SNR to dBs as per SCF-222 v4.0 section 3.4.11.
static float to_prach_preamble_snr_dB(int fapi_snr)
{
  return (fapi_snr - 128) * 0.5F;
}

void fapi_to_mac_data_msg_translator::on_rach_indication(const fapi::rach_indication_message& msg)
{
  mac_rach_indication indication;
  indication.slot_rx = slot_point(scs, msg.sfn, msg.slot);
  for (const auto& pdu : msg.pdus) {
    srsran_assert(pdu.avg_rssi != std::numeric_limits<decltype(pdu.avg_rssi)>::max(), "Average RSSI field not set");

    mac_rach_indication::rach_occasion& occas = indication.occasions.emplace_back();
    occas.frequency_index                     = pdu.ra_index;
    occas.slot_index                          = pdu.slot_index;
    occas.start_symbol                        = pdu.symbol_index;
    occas.rssi_dB                             = to_prach_rssi_dB(pdu.avg_rssi);
    for (const auto& preamble : pdu.preambles) {
      srsran_assert(preamble.preamble_pwr != std::numeric_limits<decltype(preamble.preamble_pwr)>::max(),
                    "Preamble power field not set");
      srsran_assert(preamble.preamble_snr != std::numeric_limits<decltype(preamble.preamble_snr)>::max(),
                    "Preamble SNR field not set");

      mac_rach_indication::rach_preamble& mac_pream = occas.preambles.emplace_back();
      mac_pream.index                               = preamble.preamble_index;
      mac_pream.time_advance = phy_time_unit::from_seconds(preamble.timing_advance_offset_ns * 1e-9);
      mac_pream.power_dB     = to_prach_preamble_power_dB(preamble.preamble_pwr);
      mac_pream.snr_dB       = to_prach_preamble_snr_dB(preamble.preamble_snr);
    }
  }

  rach_handler.get().handle_rach_indication(indication);
}

void fapi_to_mac_data_msg_translator::set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler)
{
  rach_handler = std::ref(mac_rach_handler);
}

void fapi_to_mac_data_msg_translator::set_cell_pdu_handler(mac_pdu_handler& handler)
{
  pdu_handler = std::ref(handler);
}

void fapi_to_mac_data_msg_translator::set_cell_crc_handler(mac_cell_control_information_handler& handler)
{
  crc_handler = std::ref(handler);
}
