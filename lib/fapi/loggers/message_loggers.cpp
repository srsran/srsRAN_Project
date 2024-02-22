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

#include "message_loggers.h"
#include "srsran/fapi/messages.h"
#include "srsran/support/format_utils.h"

using namespace srsran;
using namespace fapi;

void srsran::fapi::log_error_indication(const error_indication_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(
      buffer, "Error.indication slot={}.{} error_code={} msg_id={}", msg.sfn, msg.slot, msg.error_code, msg.message_id);
  if (msg.error_code == error_code_id::out_of_sync) {
    fmt::format_to(buffer, " expected_sfn={} expected_slot={}", msg.expected_sfn, msg.expected_slot);
  }

  logger.debug("{}", to_c_str(buffer));
}

/// Converts the given FAPI CRC SINR to dBs as per SCF-222 v4.0 section 3.4.8.
static float to_crc_ul_sinr(int sinr)
{
  return static_cast<float>(sinr) * 0.002F;
}

/// Converts the given FAPI CRC RSRP to dBs as per SCF-222 v4.0 section 3.4.8.
static float to_crc_ul_rsrp(unsigned rsrp)
{
  return static_cast<float>(rsrp - 1400) * 0.1F;
}

void srsran::fapi::log_crc_indication(const crc_indication_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "CRC.indication slot={}.{}", msg.sfn, msg.slot);

  for (const auto& pdu : msg.pdus) {
    fmt::format_to(
        buffer, "\n\t- CRC rnti={} harq_id={} tb_status={}", pdu.rnti, pdu.harq_id, pdu.tb_crc_status_ok ? "OK" : "KO");
    if (pdu.timing_advance_offset_ns != std::numeric_limits<decltype(pdu.timing_advance_offset_ns)>::min()) {
      fmt::format_to(buffer, " ta_s={}", pdu.timing_advance_offset_ns * 1e-9);
    }
    if (pdu.ul_sinr_metric != std::numeric_limits<decltype(pdu.ul_sinr_metric)>::min()) {
      fmt::format_to(buffer, " sinr={:.1f}", to_crc_ul_sinr(pdu.ul_sinr_metric));
    }
    if (pdu.rsrp != std::numeric_limits<decltype(pdu.rsrp)>::max()) {
      fmt::format_to(buffer, " rsrp={:.1f}", to_crc_ul_rsrp(pdu.rsrp));
    }
  }

  logger.debug("{}", to_c_str(buffer));
}

static void log_pdcch_pdu(const dl_pdcch_pdu& pdu, fmt::memory_buffer& buffer)
{
  fmt::format_to(buffer,
                 "\n\t- PDCCH bwp={}:{} symb={}:{} nof_dcis={}",
                 pdu.coreset_bwp_start,
                 pdu.coreset_bwp_size,
                 pdu.start_symbol_index,
                 pdu.duration_symbols,
                 pdu.dl_dci.size());
}

static void log_ssb_pdu(const dl_ssb_pdu& pdu, fmt::memory_buffer& buffer)
{
  fmt::format_to(buffer,
                 "\n\t- SSB pointA={} L_max={} pci={} k_SSB={}",
                 pdu.ssb_offset_pointA.to_uint(),
                 pdu.ssb_maintenance_v3.L_max,
                 pdu.phys_cell_id,
                 pdu.ssb_subcarrier_offset);
}

static void log_pdsch_pdu(const dl_pdsch_pdu& pdu, fmt::memory_buffer& buffer)
{
  fmt::format_to(buffer,
                 "\n\t- PDSCH rnti={} bwp={}:{} symb={}:{} CW: tbs={} mod={} rv_idx={}",
                 pdu.rnti,
                 pdu.bwp_start,
                 pdu.bwp_size,
                 pdu.start_symbol_index,
                 pdu.nr_of_symbols,
                 pdu.cws.front().tb_size,
                 pdu.cws.front().qam_mod_order,
                 pdu.cws.front().rv_index);
}

static void log_csi_rs_pdu(const dl_csi_rs_pdu& pdu, fmt::memory_buffer& buffer)
{
  if (pdu.type == csi_rs_type::CSI_RS_NZP) {
    fmt::format_to(buffer,
                   "\n\t- NZP-CSI-RS crbs={}:{} row={} symbL0={} symbL1={} scramb_id={}",
                   pdu.start_rb,
                   pdu.num_rbs,
                   pdu.row,
                   pdu.symb_L0,
                   pdu.symb_L1,
                   pdu.scramb_id);
    return;
  }

  if (pdu.type == csi_rs_type::CSI_RS_ZP) {
    fmt::format_to(buffer,
                   "\n\t- ZP-CSI-RS crbs={}:{} row={} symbL0={} symbL1={}",
                   pdu.start_rb,
                   pdu.num_rbs,
                   pdu.row,
                   pdu.symb_L0,
                   pdu.symb_L1);
    return;
  }
}

void srsran::fapi::log_dl_tti_request(const dl_tti_request_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(
      buffer, "DL_TTI.request slot={}.{}, is_last_message_in_slot={}", msg.sfn, msg.slot, msg.is_last_message_in_slot);

  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::dl_pdu_type::CSI_RS:
        log_csi_rs_pdu(pdu.csi_rs_pdu, buffer);
        break;
      case fapi::dl_pdu_type::PDCCH:
        log_pdcch_pdu(pdu.pdcch_pdu, buffer);
        break;
      case fapi::dl_pdu_type::PDSCH:
        log_pdsch_pdu(pdu.pdsch_pdu, buffer);
        break;
      case fapi::dl_pdu_type::SSB:
        log_ssb_pdu(pdu.ssb_pdu, buffer);
        break;
    }
  }

  logger.debug("{}", to_c_str(buffer));
}

/// Converts the given FAPI RACH occasion RSSI to dBs as per SCF-222 v4.0 section 3.4.11.
static float to_rach_rssi_dB(int fapi_rssi)
{
  return (fapi_rssi - 140000) * 0.001F;
}

/// Converts the given FAPI RACH preamble power to dBs as per SCF-222 v4.0 section 3.4.11.
static float to_rach_preamble_power_dB(int fapi_power)
{
  return static_cast<float>(fapi_power - 140000) * 0.001F;
}

/// Converts the given FAPI RACH preamble SNR to dBs as per SCF-222 v4.0 section 3.4.11.
static float to_rach_preamble_snr_dB(int fapi_snr)
{
  return (fapi_snr - 128) * 0.5F;
}

void srsran::fapi::log_rach_indication(const rach_indication_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "RACH.indication slot={}.{}", msg.sfn, msg.slot);

  for (const auto& pdu : msg.pdus) {
    fmt::format_to(buffer, "\n\t- PRACH symb_idx={} slot_idx={}", pdu.symbol_index, pdu.slot_index);
    if (pdu.avg_rssi != std::numeric_limits<decltype(pdu.avg_rssi)>::max()) {
      fmt::format_to(buffer, " rssi={:.1f}", to_rach_rssi_dB(pdu.avg_rssi));
    }
    fmt::format_to(buffer, " nof_preambles={}:", pdu.preambles.size());

    // Log the preambles.
    for (const auto& preamble : pdu.preambles) {
      fmt::format_to(buffer, "\n\t\t- PREAMBLE index={}", preamble.preamble_index);
      if (preamble.timing_advance_offset_ns !=
          std::numeric_limits<decltype(preamble.timing_advance_offset_ns)>::max()) {
        fmt::format_to(buffer, " ta_s={}", preamble.timing_advance_offset_ns * 1e-9);
      }
      if (preamble.preamble_pwr != std::numeric_limits<decltype(preamble.preamble_pwr)>::max()) {
        fmt::format_to(buffer, " pwr={:.1f}", to_rach_preamble_power_dB(preamble.preamble_pwr));
      }
      if (preamble.preamble_snr != std::numeric_limits<decltype(preamble.preamble_snr)>::max()) {
        fmt::format_to(buffer, " snr={:.1f}", to_rach_preamble_snr_dB(preamble.preamble_snr));
      }
    }
  }

  logger.debug("{}", to_c_str(buffer));
}

void srsran::fapi::log_rx_data_indication(const rx_data_indication_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "Rx_Data.indication slot={}.{}", msg.sfn, msg.slot);

  for (const auto& pdu : msg.pdus) {
    fmt::format_to(buffer, "\n\t- DATA rnti={} harq_id={} tbs={}", pdu.rnti, pdu.harq_id, pdu.pdu_length);
  }

  logger.debug("{}", to_c_str(buffer));
}

void srsran::fapi::log_tx_data_request(const tx_data_request_message& msg, srslog::basic_logger& logger)
{
  logger.debug("Tx_Data.request slot={}.{} nof_pdus={}", msg.sfn, msg.slot, msg.pdus.size());
}

/// Converts the given FAPI UCI SINR to dBs as per SCF-222 v4.0 section 3.4.9.
static float to_uci_ul_sinr(int sinr)
{
  return static_cast<float>(sinr) * 0.002F;
}

/// Converts the given FAPI UCI RSRP to dBs as per SCF-222 v4.0 section 3.4.9.
static float to_uci_ul_rsrp(unsigned rsrp)
{
  return static_cast<float>(rsrp - 1400) * 0.1F;
}

static void log_uci_pucch_f0_f1_pdu(const uci_pucch_pdu_format_0_1& pdu, fmt::memory_buffer& buffer)
{
  fmt::format_to(
      buffer, "\n\t- UCI PUCCH format 0/1 format={} rnti={}", static_cast<unsigned>(pdu.pucch_format), pdu.rnti);

  if (pdu.ul_sinr_metric != std::numeric_limits<decltype(pdu.ul_sinr_metric)>::min()) {
    fmt::format_to(buffer, " sinr={:.1f}", to_uci_ul_sinr(pdu.ul_sinr_metric));
  }
  if (pdu.timing_advance_offset_ns != std::numeric_limits<decltype(pdu.timing_advance_offset_ns)>::min()) {
    fmt::format_to(buffer, " ta_s={}", pdu.timing_advance_offset_ns * 1e-9);
  }
  if (pdu.rsrp != std::numeric_limits<decltype(pdu.rsrp)>::max()) {
    fmt::format_to(buffer, " rsrp={:.1f}", to_uci_ul_rsrp(pdu.rsrp));
  }

  if (pdu.pdu_bitmap.test(fapi::uci_pucch_pdu_format_0_1::HARQ_BIT)) {
    fmt::format_to(buffer, " HARQ: confidence={} harq_ack=", pdu.harq.harq_confidence_level);
    for (unsigned i = 0, e = pdu.harq.harq_values.size(), last = e - 1; i != e; ++i) {
      fmt::format_to(buffer, "{}", to_string(pdu.harq.harq_values[i]));
      if (i != last) {
        fmt::format_to(buffer, ",");
      }
    }
  }
  if (pdu.pdu_bitmap.test(fapi::uci_pucch_pdu_format_0_1::SR_BIT)) {
    fmt::format_to(buffer,
                   " SR: confidence={} sr={}",
                   pdu.sr.sr_confidence_level,
                   pdu.sr.sr_indication ? "detected" : "not detected");
  }
}

static void log_uci_pucch_f234_pdu(const uci_pucch_pdu_format_2_3_4& pdu, fmt::memory_buffer& buffer)
{
  fmt::format_to(
      buffer, "\n\t- UCI PUCCH format 2/3/4 format={} rnti={}", static_cast<unsigned>(pdu.pucch_format) + 2, pdu.rnti);

  if (pdu.ul_sinr_metric != std::numeric_limits<decltype(pdu.ul_sinr_metric)>::min()) {
    fmt::format_to(buffer, " sinr={:.1f}", to_uci_ul_sinr(pdu.ul_sinr_metric));
  }
  if (pdu.timing_advance_offset_ns != std::numeric_limits<decltype(pdu.timing_advance_offset_ns)>::min()) {
    fmt::format_to(buffer, " ta_s={}", pdu.timing_advance_offset_ns * 1e-9);
  }
  if (pdu.rsrp != std::numeric_limits<decltype(pdu.rsrp)>::max()) {
    fmt::format_to(buffer, " rsrp={:.1f}", to_uci_ul_rsrp(pdu.rsrp));
  }

  if (pdu.pdu_bitmap.test(uci_pucch_pdu_format_2_3_4::SR_BIT)) {
    fmt::format_to(buffer, " SR: bit_len={}", pdu.sr.sr_bitlen);
  }
  if (pdu.pdu_bitmap.test(uci_pucch_pdu_format_2_3_4::HARQ_BIT)) {
    fmt::format_to(buffer, " HARQ: detection={} bit_len={}", pdu.harq.detection_status, pdu.harq.expected_bit_length);
  }
  if (pdu.pdu_bitmap.test(uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT)) {
    fmt::format_to(
        buffer, " CSI1: detection={} bit_len={}", pdu.csi_part1.detection_status, pdu.csi_part1.expected_bit_length);
  }
}

static void log_uci_pusch_pdu(const uci_pusch_pdu& pdu, fmt::memory_buffer& buffer)
{
  fmt::format_to(buffer, "\n\t- UCI PUSCH rnti={}", pdu.rnti);

  if (pdu.ul_sinr_metric != std::numeric_limits<decltype(pdu.ul_sinr_metric)>::min()) {
    fmt::format_to(buffer, " sinr={:.1f}", to_uci_ul_sinr(pdu.ul_sinr_metric));
  }
  if (pdu.timing_advance_offset_ns != std::numeric_limits<decltype(pdu.timing_advance_offset_ns)>::min()) {
    fmt::format_to(buffer, " ta_s={}", pdu.timing_advance_offset_ns * 1e-9);
  }
  if (pdu.rsrp != std::numeric_limits<decltype(pdu.rsrp)>::max()) {
    fmt::format_to(buffer, " rsrp={:.1f}", to_uci_ul_rsrp(pdu.rsrp));
  }

  if (pdu.pdu_bitmap.test(uci_pusch_pdu::HARQ_BIT)) {
    fmt::format_to(buffer, " HARQ: detection={} bit_len={}", pdu.harq.detection_status, pdu.harq.expected_bit_length);
  }
  if (pdu.pdu_bitmap.test(uci_pusch_pdu::CSI_PART1_BIT)) {
    fmt::format_to(
        buffer, " CSI1: detection={} bit_len={}", pdu.csi_part1.detection_status, pdu.csi_part1.expected_bit_length);
  }
}

void srsran::fapi::log_uci_indication(const uci_indication_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "UCI.indication slot={}.{}", msg.sfn, msg.slot);

  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::uci_pdu_type::PUSCH:
        log_uci_pusch_pdu(pdu.pusch_pdu, buffer);
        break;
      case fapi::uci_pdu_type::PUCCH_format_0_1:
        log_uci_pucch_f0_f1_pdu(pdu.pucch_pdu_f01, buffer);
        break;
      case fapi::uci_pdu_type::PUCCH_format_2_3_4:
        log_uci_pucch_f234_pdu(pdu.pucch_pdu_f234, buffer);
        break;
    }
  }

  logger.debug("{}", to_c_str(buffer));
}

static void log_prach_pdu(const ul_prach_pdu& pdu, fmt::memory_buffer& buffer)
{
  fmt::format_to(buffer,
                 "\n\t- PRACH pci={} format={} fd_ra={}:{} symb={} z_corr={} res_config_idx={}",
                 pdu.phys_cell_id,
                 to_string(pdu.prach_format),
                 pdu.index_fd_ra,
                 pdu.maintenance_v3.num_fd_ra,
                 pdu.prach_start_symbol,
                 pdu.num_cs,
                 pdu.maintenance_v3.prach_res_config_index);
}

static void log_pusch_pdu(const ul_pusch_pdu& pdu, fmt::memory_buffer& buffer)
{
  fmt::format_to(buffer,
                 "\n\t- PUSCH rnti={} bwp={}:{} symb={}:{} mod={}",
                 pdu.rnti,
                 pdu.bwp_start,
                 pdu.bwp_size,
                 pdu.start_symbol_index,
                 pdu.nr_of_symbols,
                 pdu.qam_mod_order);

  if (pdu.pdu_bitmap.test(fapi::ul_pusch_pdu::PUSCH_DATA_BIT)) {
    fmt::format_to(buffer,
                   " CW: tbs={} rv_idx={} harq_id={}",
                   pdu.pusch_data.tb_size,
                   pdu.pusch_data.rv_index,
                   pdu.pusch_data.harq_process_id);
  }

  if (pdu.pdu_bitmap.test(fapi::ul_pusch_pdu::PUSCH_UCI_BIT)) {
    fmt::format_to(buffer,
                   " UCI: harq_bit_len={} csi1_bit_len={}",
                   pdu.pusch_uci.harq_ack_bit_length,
                   pdu.pusch_uci.csi_part1_bit_length);
  }
}

static void log_pucch_pdu(const ul_pucch_pdu& pdu, fmt::memory_buffer& buffer)
{
  fmt::format_to(buffer,
                 "\n\t- PUCCH rnti={} bwp={}:{} format={} prb={}:{} prb2={} symb={}:{} harq_bit_len={} sr_bit_len={}",
                 pdu.rnti,
                 pdu.bwp_start,
                 pdu.bwp_size,
                 pdu.format_type,
                 pdu.prb_start,
                 pdu.prb_size,
                 pdu.second_hop_prb,
                 pdu.start_symbol_index,
                 pdu.nr_of_symbols,
                 pdu.bit_len_harq,
                 pdu.sr_bit_len);

  switch (pdu.format_type) {
    case pucch_format::FORMAT_2:
    case pucch_format::FORMAT_3:
    case pucch_format::FORMAT_4:
      fmt::format_to(buffer, " csi1_bit_len={}", pdu.csi_part1_bit_length);
      break;
    default:
      break;
  }
}

void srsran::fapi::log_ul_tti_request(const ul_tti_request_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "UL_TTI.request slot={}.{}", msg.sfn, msg.slot);

  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::ul_pdu_type::PRACH:
        log_prach_pdu(pdu.prach_pdu, buffer);
        break;
      case fapi::ul_pdu_type::PUCCH:
        log_pucch_pdu(pdu.pucch_pdu, buffer);
        break;
      case fapi::ul_pdu_type::PUSCH:
        log_pusch_pdu(pdu.pusch_pdu, buffer);
        break;
      case fapi::ul_pdu_type::SRS:
      default:
        srsran_assert(0, "UL_TTI.request PDU type value ({}) not recognized.", static_cast<unsigned>(pdu.pdu_type));
    }
  }

  logger.debug("{}", to_c_str(buffer));
}

void srsran::fapi::log_slot_indication(const slot_indication_message& msg, srslog::basic_logger& logger)
{
  logger.set_context(msg.sfn, msg.slot);
  logger.debug("Slot.indication");
}

void srsran::fapi::log_ul_dci_request(const ul_dci_request_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(
      buffer, "UL_DCI.request slot={}.{}, is_last_message_in_slot={}", msg.sfn, msg.slot, msg.is_last_message_in_slot);

  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::ul_dci_pdu_type::PDCCH:
        log_pdcch_pdu(pdu.pdu, buffer);
        break;
      default:
        srsran_assert(0, "UL_DCI.request PDU type value ({}) not recognized.", static_cast<unsigned>(pdu.pdu_type));
    }
  }

  logger.debug("{}", to_c_str(buffer));
}
