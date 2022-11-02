/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi/message_loggers.h"
#include "srsgnb/fapi/messages.h"
#include "srsgnb/support/format_utils.h"

using namespace srsgnb;
using namespace fapi;

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

void srsgnb::fapi::log_crc_indication(const crc_indication_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "CRC.indication slot={}.{}", msg.sfn, msg.slot);

  for (const auto& pdu : msg.pdus) {
    fmt::format_to(buffer,
                   "\n\t- CRC rnti={:#x} harq_id={} tb_status={} ta_s={} sinr={} rsrp={}",
                   pdu.rnti,
                   pdu.harq_id,
                   pdu.tb_crc_status_ok ? "OK" : "KO",
                   (pdu.timing_advance_offset_ns != std::numeric_limits<decltype(pdu.timing_advance_offset_ns)>::min())
                       ? fmt::format("{}", pdu.timing_advance_offset_ns * 1e-9)
                       : "n/a",
                   (pdu.ul_sinr_metric != std::numeric_limits<decltype(pdu.ul_sinr_metric)>::min())
                       ? fmt::format("{}", to_crc_ul_sinr(pdu.ul_sinr_metric))
                       : "n/a",
                   (pdu.rsrp != std::numeric_limits<decltype(pdu.rsrp)>::max())
                       ? fmt::format("{}", to_crc_ul_rsrp(pdu.rsrp))
                       : "n/a");
  }

  logger.debug("{}", to_c_str(buffer));
}

static void log_pdcch_pdu(const dl_pdcch_pdu& pdu, fmt::memory_buffer& buffer)
{
  fmt::format_to(buffer,
                 "\n\t- PDCCH bwp={}:{} symb={}:{}",
                 pdu.coreset_bwp_start,
                 pdu.coreset_bwp_size,
                 pdu.start_symbol_index,
                 pdu.duration_symbols);
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
                 "\n\t- PDSCH rnti={:#x} bwp={}:{} symb={}:{} tbs={}",
                 pdu.rnti,
                 pdu.bwp_start,
                 pdu.bwp_size,
                 pdu.start_symbol_index,
                 pdu.nr_of_symbols,
                 pdu.cws.front().tb_size);
}

void srsgnb::fapi::log_dl_tti_request(const dl_tti_request_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "DL_TTI.request slot={}.{}", msg.sfn, msg.slot);

  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::dl_pdu_type::CSI_RS:
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
      default:
        srsgnb_assert(0, "DL_TTI.request PDU type value ([]) not recognized.", static_cast<unsigned>(pdu.pdu_type));
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

void srsgnb::fapi::log_rach_indication(const rach_indication_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "RACH.indication slot={}.{}", msg.sfn, msg.slot);
  for (const auto& pdu : msg.pdus) {
    fmt::format_to(buffer,
                   "\n\t- PRACH symb_idx={} rssi={} preambles={}:",
                   pdu.symbol_index,
                   (pdu.avg_rssi != std::numeric_limits<decltype(pdu.avg_rssi)>::max())
                       ? fmt::format("{}", to_rach_rssi_dB(pdu.avg_rssi))
                       : "n/a",
                   pdu.preambles.size());

    // Log the preambles.
    for (const auto& pream : pdu.preambles) {
      fmt::format_to(
          buffer,
          "\n\t\t- PREAMBLE index={} ta_s={} epre={} snr={}",
          pream.preamble_index,
          (pream.timing_advance_offset_ns != std::numeric_limits<decltype(pream.timing_advance_offset_ns)>::max())
              ? fmt::format("{}", pream.timing_advance_offset_ns * 1e-9)
              : "n/a",
          (pream.preamble_pwr != std::numeric_limits<decltype(pream.preamble_pwr)>::max())
              ? fmt::format("{}", to_rach_preamble_power_dB(pream.preamble_pwr))
              : "n/a",
          (pream.preamble_snr != std::numeric_limits<decltype(pream.preamble_snr)>::max())
              ? fmt::format("{}", to_rach_preamble_snr_dB(pream.preamble_snr))
              : "n/a");
    }
  }

  logger.debug("{}", to_c_str(buffer));
}

void srsgnb::fapi::log_rx_data_indication(const rx_data_indication_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "Rx_Data.indication slot={}.{}", msg.sfn, msg.slot);

  for (const auto& pdu : msg.pdus) {
    fmt::format_to(buffer, "\n\t- DATA rnti={:#x} harq_id={} tbs={}", pdu.rnti, pdu.harq_id, pdu.pdu_length);
  }

  logger.debug("{}", to_c_str(buffer));
}

void srsgnb::fapi::log_tx_data_request(const tx_data_request_message& msg, srslog::basic_logger& logger)
{
  logger.debug("Tx_Data.request slot={}.{} nof_pdu={}", msg.sfn, msg.slot, msg.pdus.size());
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
  fmt::format_to(buffer,
                 "\n\t- UCI PUCCH format 0/1 format={} rnti={:#x} ta_ns={} sinr={} rsrp={}",
                 static_cast<unsigned>(pdu.pucch_format),
                 pdu.rnti,
                 (pdu.timing_advance_offset_ns != std::numeric_limits<decltype(pdu.timing_advance_offset_ns)>::min())
                     ? fmt::format("{}", pdu.timing_advance_offset_ns * 1e-9)
                     : "n/a",
                 (pdu.ul_sinr_metric != std::numeric_limits<decltype(pdu.ul_sinr_metric)>::min())
                     ? fmt::format("{}", to_uci_ul_sinr(pdu.ul_sinr_metric))
                     : "n/a",
                 (pdu.rsrp != std::numeric_limits<decltype(pdu.rsrp)>::max())
                     ? fmt::format("{}", to_uci_ul_rsrp(pdu.rsrp))
                     : "n/a");

  if (pdu.pdu_bitmap.test(fapi::uci_pucch_pdu_format_0_1::HARQ_BIT)) {
    fmt::format_to(buffer, " harq_ack=");
    for (unsigned i = 0, e = pdu.harq.harq_values.size(), last = e - 1; i != e; ++i) {
      fmt::format_to(buffer, "{}", to_string(pdu.harq.harq_values[i]));
      if (i != last) {
        fmt::format_to(buffer, ",");
      }
    }
  }

  if (pdu.pdu_bitmap.test(fapi::uci_pucch_pdu_format_0_1::SR_BIT)) {
    fmt::format_to(buffer, " sr={} ", pdu.sr.sr_indication ? "detected" : "not detected");
  }
}

void srsgnb::fapi::log_uci_indication(const uci_indication_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "UCI.indication slot={}.{}", msg.sfn, msg.slot);

  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::uci_pdu_type::PUCCH_format_0_1:
        log_uci_pucch_f0_f1_pdu(pdu.pucch_pdu_f01, buffer);
        break;
      default:
        continue;
    }
  }

  logger.debug("{}", to_c_str(buffer));
}

static void log_prach_pdu(const ul_prach_pdu& pdu, fmt::memory_buffer& buffer)
{
  fmt::format_to(buffer,
                 "\n\t- PRACH pci={} fd_ra={}:{} symb={} res_config_idx={} format={}",
                 pdu.phys_cell_id,
                 pdu.index_fd_ra,
                 pdu.maintenance_v3.num_fd_ra,
                 pdu.prach_start_symbol,
                 pdu.maintenance_v3.prach_res_config_index,
                 to_string(pdu.prach_format));
}

static void log_pusch_pdu(const ul_pusch_pdu& pdu, fmt::memory_buffer& buffer)
{
  fmt::format_to(buffer,
                 "\n\t- PUSCH rnti={:#x} bwp={}:{} symb={}:{} tbs={}",
                 pdu.rnti,
                 pdu.bwp_start,
                 pdu.bwp_size,
                 pdu.start_symbol_index,
                 pdu.nr_of_symbols,
                 pdu.pdu_bitmap.test(fapi::ul_pusch_pdu::PUSCH_DATA_BIT) ? fmt::format("{}", pdu.pusch_data.tb_size)
                                                                         : "n/a");
}

static void log_pucch_pdu(const ul_pucch_pdu& pdu, fmt::memory_buffer& buffer)
{
  // :TODO: log different properties for different formats.
  fmt::format_to(buffer,
                 "\n\t- PUCCH rnti={:#x} format={} prb={} prb2={} symb={}:{} harq_bits={} sr_bits={}",
                 pdu.rnti,
                 pdu.format_type,
                 pdu.prb_start,
                 pdu.second_hop_prb,
                 pdu.start_symbol_index,
                 pdu.nr_of_symbols,
                 pdu.bit_len_harq,
                 pdu.sr_bit_len);
}

void srsgnb::fapi::log_ul_tti_request(const ul_tti_request_message& msg, srslog::basic_logger& logger)
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
        srsgnb_assert(0, "UL_TTI.request PDU type value ([]) not recognized.", static_cast<unsigned>(pdu.pdu_type));
    }
  }

  logger.debug("{}", to_c_str(buffer));
}

void srsgnb::fapi::log_slot_indication(const slot_indication_message& msg, srslog::basic_logger& logger)
{
  logger.debug("Slot.indication slot={}.{}", msg.sfn, msg.slot);
}

void srsgnb::fapi::log_ul_dci_request(const ul_dci_request_message& msg, srslog::basic_logger& logger)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "UL_DCI.request slot={}.{}", msg.sfn, msg.slot);

  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::ul_dci_pdu_type::PDCCH:
        log_pdcch_pdu(pdu.pdu, buffer);
        break;
      default:
        srsgnb_assert(0, "UL_DCI.request PDU type value ([]) not recognized.", static_cast<unsigned>(pdu.pdu_type));
    }
  }

  logger.debug("{}", to_c_str(buffer));
}
