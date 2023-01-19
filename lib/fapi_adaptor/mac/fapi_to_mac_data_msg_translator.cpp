/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "fapi_to_mac_data_msg_translator.h"
#include "srsgnb/fapi/messages.h"

using namespace srsgnb;
using namespace fapi_adaptor;

static constexpr unsigned NOF_BITS_PER_BYTE = 8U;

/// Maximum length of payload in bits of the PDU carried by UCI PUSCH PDU. As per SCF-222 v4.0 section 3.4.9.1.
constexpr size_t MAX_PAYLOAD_BITS_PER_UCI_ON_PUSCH = 1706;

/// Maximum length of payload in bytes of the PDU carried by UCI PUSCH PDU. As per SCF-222 v4.0 section 3.4.9.1.
constexpr size_t MAX_PAYLOAD_BYTES_PER_UCI_ON_PUSCH = 214;

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

fapi_to_mac_data_msg_translator::fapi_to_mac_data_msg_translator(subcarrier_spacing scs_) :
  rach_handler(dummy_mac_rach_handler), pdu_handler(dummy_pdu_handler), crc_handler(dummy_crc_handler), scs(scs_)
{
}

void fapi_to_mac_data_msg_translator::on_dl_tti_response(const fapi::dl_tti_response_message& msg) {}

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
    mac_rx_pdu& pdu = indication.pdus.emplace_back();
    pdu.harq_id     = fapi_pdu.harq_id;
    pdu.rnti        = fapi_pdu.rnti;
    pdu.pdu         = span<const uint8_t>(fapi_pdu.data, fapi_pdu.pdu_length);
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
  }

  crc_handler.get().handle_crc(indication);
}

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
static void convert_fapi_to_mac_ul_sinr(int16_t fapi_ul_sinr, float& mac_ulsinr)
{
  if (fapi_ul_sinr != std::numeric_limits<decltype(fapi_ul_sinr)>::min()) {
    mac_ulsinr = to_uci_ul_sinr(fapi_ul_sinr);
  }
}

/// Converts the given FAPI Timing Advance Offset in nanoseconds to Physical layer time unit.
static void convert_fapi_to_mac_ta_offset(int16_t fapi_ta_offset_ns, optional<phy_time_unit>& mac_ta_offset)
{
  if (fapi_ta_offset_ns != std::numeric_limits<decltype(fapi_ta_offset_ns)>::min()) {
    mac_ta_offset = phy_time_unit::from_seconds(static_cast<float>(fapi_ta_offset_ns) * 1e-9);
  }
}

/// Converts the given FAPI UCI RSSI to dBs as per SCF-222 v4.0 section 3.4.9.
static void convert_fapi_to_mac_rssi(uint16_t fapi_rssi, optional<float>& mac_rssi)
{
  if (fapi_rssi != std::numeric_limits<decltype(fapi_rssi)>::max()) {
    mac_rssi = to_uci_ul_rssi(fapi_rssi);
  }
}

/// Converts the given FAPI UCI RSRP to dBs as per SCF-222 v4.0 section 3.4.9.
static void convert_fapi_to_mac_rsrp(uint16_t fapi_rsrp, optional<float>& mac_rsrp)
{
  if (fapi_rsrp != std::numeric_limits<decltype(fapi_rsrp)>::max()) {
    mac_rsrp = to_uci_ul_rsrp(fapi_rsrp);
  }
}

static void convert_fapi_to_mac_pucch_f0_f1_uci_ind(mac_uci_pdu::pucch_f0_or_f1_type&     mac_pucch,
                                                    const fapi::uci_pucch_pdu_format_0_1& fapi_pucch)
{
  mac_pucch.is_f1 = fapi_pucch.pucch_format == fapi::uci_pucch_pdu_format_0_1::format_type::format_1;

  convert_fapi_to_mac_ul_sinr(fapi_pucch.ul_sinr_metric, mac_pucch.ul_sinr);
  convert_fapi_to_mac_ta_offset(fapi_pucch.timing_advance_offset_ns, mac_pucch.time_advance_offset);
  convert_fapi_to_mac_rssi(fapi_pucch.rssi, mac_pucch.rssi);
  convert_fapi_to_mac_rsrp(fapi_pucch.rsrp, mac_pucch.rsrp);

  // Fill SR.
  if (fapi_pucch.pdu_bitmap.test(fapi::uci_pucch_pdu_format_0_1::SR_BIT)) {
    mac_pucch.sr_info.emplace();
    mac_pucch.sr_info.value().sr_detected = fapi_pucch.sr.sr_indication;
  }

  // Fill HARQ.
  if (fapi_pucch.pdu_bitmap.test(fapi::uci_pucch_pdu_format_0_1::HARQ_BIT)) {
    mac_pucch.harq_info.emplace();
    mac_pucch.harq_info.value().harqs.assign(fapi_pucch.harq.harq_values.begin(), fapi_pucch.harq.harq_values.end());
  }
}

/// Converts the given FAPI UCI on PUSCH payload in array of bytes to bitset representation.
static void
convert_fapi_uci_pusch_payload_to_mac(uint16_t                                                          payload_bits,
                                      const static_vector<uint8_t, MAX_PAYLOAD_BYTES_PER_UCI_ON_PUSCH>& payload,
                                      bounded_bitset<MAX_PAYLOAD_BITS_PER_UCI_ON_PUSCH>&                out)
{
  // Resized to 0 since size is incremented upon appending. And, to also keep track of exact number of bits.
  out.resize(0);
  // Lowest order information bit [position 0] is mapped to the most significant bit. As per SCF-222 v4.0
  // section 3.4.9.1.
  for (int j = static_cast<int>(std::ceil(payload_bits / NOF_BITS_PER_BYTE)) - 1; j >= 0; --j) {
    for (int k = NOF_BITS_PER_BYTE - 1; k >= 0; --k) {
      // FirstBitIsLeftmost is false.
      out.push_back(((payload[j] >> k) & 1U) != 0U);
    }
  }
}

static void convert_fapi_to_mac_pusch_uci_ind(mac_uci_pdu::pusch_type& mac_pusch, const fapi::uci_pusch_pdu& fapi_pusch)
{
  convert_fapi_to_mac_ul_sinr(fapi_pusch.ul_sinr_metric, mac_pusch.ul_sinr);
  convert_fapi_to_mac_ta_offset(fapi_pusch.timing_advance_offset_ns, mac_pusch.time_advance_offset);
  convert_fapi_to_mac_rssi(fapi_pusch.rssi, mac_pusch.rssi);
  convert_fapi_to_mac_rsrp(fapi_pusch.rsrp, mac_pusch.rsrp);

  // Fill HARQ.
  if (fapi_pusch.pdu_bitmap.test(fapi::uci_pusch_pdu::HARQ_BIT)) {
    mac_pusch.harq_info.emplace();
    mac_pusch.harq_info.value().harq_status = fapi_pusch.harq.detection_status;
    convert_fapi_uci_pusch_payload_to_mac(
        fapi_pusch.harq.bit_length, fapi_pusch.harq.payload, mac_pusch.harq_info.value().payload);
  }

  // Fill CSI Part 1.
  if (fapi_pusch.pdu_bitmap.test(fapi::uci_pusch_pdu::CSI_PART1_BIT)) {
    mac_pusch.csi_part1_info.emplace();
    mac_pusch.csi_part1_info.value().csi_status = fapi_pusch.csi_part1.detection_status;
    convert_fapi_uci_pusch_payload_to_mac(
        fapi_pusch.csi_part1.bit_length, fapi_pusch.csi_part1.payload, mac_pusch.csi_part1_info.value().payload);
  }

  // Fill CSI Part 2.
  if (fapi_pusch.pdu_bitmap.test(fapi::uci_pusch_pdu::CSI_PART2_BIT)) {
    mac_pusch.csi_part2_info.emplace();
    mac_pusch.csi_part2_info.value().csi_status = fapi_pusch.csi_part2.detection_status;
    convert_fapi_uci_pusch_payload_to_mac(
        fapi_pusch.csi_part2.bit_length, fapi_pusch.csi_part2.payload, mac_pusch.csi_part2_info.value().payload);
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
        mac_pdu.type         = mac_uci_pdu::pdu_type::pusch;
        mac_pdu.rnti         = to_rnti(pdu.pusch_pdu.rnti);
        convert_fapi_to_mac_pusch_uci_ind(mac_pdu.pusch, pdu.pusch_pdu);
      } break;
      case fapi::uci_pdu_type::PUCCH_format_0_1: {
        mac_uci_pdu& mac_pdu = mac_msg.ucis.emplace_back();
        mac_pdu.type         = mac_uci_pdu::pdu_type::pucch_f0_or_f1;
        mac_pdu.rnti         = to_rnti(pdu.pucch_pdu_f01.rnti);
        convert_fapi_to_mac_pucch_f0_f1_uci_ind(mac_pdu.pucch_f0_or_f1, pdu.pucch_pdu_f01);
      } break;
      case fapi::uci_pdu_type::PUCCH_format_2_3_4:
        break;
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
    srsgnb_assert(pdu.avg_rssi != std::numeric_limits<uint16_t>::max(), "Average RSSI field not set");

    mac_rach_indication::rach_occasion& occas = indication.occasions.emplace_back();
    occas.frequency_index                     = pdu.ra_index;
    occas.slot_index                          = pdu.slot_index;
    occas.start_symbol                        = pdu.symbol_index;
    occas.rssi_dB                             = to_prach_rssi_dB(pdu.avg_rssi);
    for (const auto& preamble : pdu.preambles) {
      srsgnb_assert(preamble.preamble_pwr != std::numeric_limits<uint32_t>::max(), "Preamble power field not set");
      srsgnb_assert(preamble.preamble_snr != std::numeric_limits<uint8_t>::max(), "Preamble SNR field not set");

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
