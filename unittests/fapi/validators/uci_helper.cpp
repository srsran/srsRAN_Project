/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "uci_helper.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

static std::mt19937 gen(0);

static unsigned generate_handle()
{
  std::uniform_int_distribution<unsigned> dist(0, 4e6);
  return dist(gen);
}

static rnti_t generate_rnti()
{
  std::uniform_int_distribution<unsigned> dist(1, 65535);
  return to_rnti(dist(gen));
}

static int generate_ul_sinr_metric()
{
  std::uniform_int_distribution<int> dist(-32768, 32767);
  return dist(gen);
}

static unsigned generate_timing_advance_offset()
{
  std::uniform_int_distribution<unsigned> dist(0, 100);
  unsigned                                value = dist(gen);

  return (value < 64U) ? value : 65535U;
}

static int generate_timing_advance_offset_in_ns()
{
  std::uniform_int_distribution<int> dist(-32768, 32767);
  int                                value = dist(gen);

  return (-16800 <= value && value <= 16800) ? value : -32768;
}

static unsigned generate_rssi_or_rsrp()
{
  std::uniform_int_distribution<unsigned> dist(0, 2500);
  unsigned                                value = dist(gen);

  return (value <= 1280U) ? value : 65535;
}

static uci_detection_status generate_detection_status()
{
  std::uniform_int_distribution<unsigned> dist(1, 5);
  return static_cast<uci_detection_status>(dist(gen));
}

static unsigned generate_bit_length()
{
  std::uniform_int_distribution<unsigned> dist(1, 1706);
  return dist(gen);
}

static uci_harq_pdu generate_harq_pdu()
{
  uci_harq_pdu pdu;

  pdu.detection_status = generate_detection_status();
  pdu.bit_length       = generate_bit_length();
  if (pdu.detection_status == uci_detection_status::crc_pass || pdu.detection_status == uci_detection_status::no_dtx ||
      pdu.detection_status == uci_detection_status::dtx_not_checked) {
    pdu.payload.resize(std::ceil(static_cast<float>(pdu.bit_length) / 8.F));
  }

  return pdu;
}

static uci_csi_part1 generate_csi_part1_pdu()
{
  uci_csi_part1 pdu;

  pdu.detection_status = generate_detection_status();
  pdu.bit_length       = generate_bit_length();
  if (pdu.detection_status == uci_detection_status::crc_pass || pdu.detection_status == uci_detection_status::no_dtx ||
      pdu.detection_status == uci_detection_status::dtx_not_checked) {
    pdu.payload.resize(std::ceil(static_cast<float>(pdu.bit_length) / 8.F));
  }

  return pdu;
}

static uci_csi_part2 generate_csi_part2_pdu()
{
  uci_csi_part2 pdu;

  pdu.detection_status = generate_detection_status();
  pdu.bit_length       = generate_bit_length();
  if (pdu.detection_status == uci_detection_status::crc_pass || pdu.detection_status == uci_detection_status::no_dtx ||
      pdu.detection_status == uci_detection_status::dtx_not_checked) {
    pdu.payload.resize(std::ceil(static_cast<float>(pdu.bit_length) / 8.F));
  }

  return pdu;
}

uci_pusch_pdu unittest::build_valid_uci_push_pdu()
{
  uci_pusch_pdu pdu;

  pdu.handle                   = generate_handle();
  pdu.rnti                     = generate_rnti();
  pdu.ul_sinr_metric           = static_cast<int16_t>(generate_ul_sinr_metric());
  pdu.timing_advance_offset    = generate_timing_advance_offset();
  pdu.timing_advance_offset_ns = static_cast<int16_t>(generate_timing_advance_offset_in_ns());
  pdu.rssi                     = generate_rssi_or_rsrp();
  pdu.rsrp                     = generate_rssi_or_rsrp();

  // Enable HARQ, CSI part 1 and CSI Part 2.
  pdu.pdu_bitmap = 14U;
  pdu.harq       = generate_harq_pdu();
  pdu.csi_part1  = generate_csi_part1_pdu();
  pdu.csi_part2  = generate_csi_part2_pdu();

  return pdu;
}

static sr_pdu_format_0_1 generate_sr_format01_pdu()
{
  sr_pdu_format_0_1 pdu;

  std::uniform_int_distribution<unsigned> dist(0, 3);
  pdu.sr_indication = std::min(1U, dist(gen));

  unsigned value          = dist(gen);
  pdu.sr_confidence_level = (value < 2U) ? value : 255U;

  return pdu;
}

static uci_harq_format_0_1 generate_harq_format01_pdu()
{
  uci_harq_format_0_1 pdu;

  std::uniform_int_distribution<unsigned> dist(0, 2);
  unsigned                                value = dist(gen);
  pdu.harq_confidence_level                     = (value < 2U) ? value : 255U;

  std::uniform_int_distribution<unsigned> nof_dist(1, 2);
  for (unsigned i = 0, e = nof_dist(gen); i != e; ++i) {
    pdu.harq_values.emplace_back(dist(gen));
  }

  return pdu;
}

uci_pucch_pdu_format_0_1 unittest::build_valid_uci_pucch_format01_pdu()
{
  uci_pucch_pdu_format_0_1 pdu;

  pdu.handle                   = generate_handle();
  pdu.rnti                     = generate_rnti();
  pdu.ul_sinr_metric           = static_cast<int16_t>(generate_ul_sinr_metric());
  pdu.timing_advance_offset    = generate_timing_advance_offset();
  pdu.timing_advance_offset_ns = static_cast<int16_t>(generate_timing_advance_offset_in_ns());
  pdu.rssi                     = generate_rssi_or_rsrp();
  pdu.rsrp                     = generate_rssi_or_rsrp();

  std::uniform_int_distribution<unsigned> dist(0, 1);
  pdu.pucch_format = static_cast<uci_pucch_pdu_format_0_1::format_type>(dist(gen));

  // Enable SR and HARQ for PUCCH format 0/1.
  pdu.pdu_bitmap = 3U;
  pdu.sr         = generate_sr_format01_pdu();
  pdu.harq       = generate_harq_format01_pdu();

  return pdu;
}

static sr_pdu_format_2_3_4 generate_sr_format234_pdu()
{
  sr_pdu_format_2_3_4 pdu;

  std::uniform_int_distribution<unsigned> dist(1, 4);
  pdu.sr_bitlen = dist(gen);
  pdu.sr_payload.resize(std::ceil(static_cast<float>(pdu.sr_bitlen) / 8.F));

  return pdu;
}

static uci_payload_pusch_pucch generate_uci_payload()
{
  uci_payload_pusch_pucch payload;

  payload.detection_status          = generate_detection_status();
  payload.expected_uci_payload_size = generate_bit_length();
  if (payload.detection_status == uci_detection_status::crc_pass ||
      payload.detection_status == uci_detection_status::no_dtx ||
      payload.detection_status == uci_detection_status::dtx_not_checked) {
    payload.payload.resize(std::ceil(static_cast<float>(payload.expected_uci_payload_size) / 8.F));
  }

  return payload;
}

uci_pucch_pdu_format_2_3_4 unittest::build_valid_uci_pucch_format234_pdu()
{
  uci_pucch_pdu_format_2_3_4 pdu;

  pdu.handle                   = generate_handle();
  pdu.rnti                     = generate_rnti();
  pdu.ul_sinr_metric           = static_cast<int16_t>(generate_ul_sinr_metric());
  pdu.timing_advance_offset    = generate_timing_advance_offset();
  pdu.timing_advance_offset_ns = static_cast<int16_t>(generate_timing_advance_offset_in_ns());
  pdu.rssi                     = generate_rssi_or_rsrp();
  pdu.rsrp                     = generate_rssi_or_rsrp();

  std::uniform_int_distribution<unsigned> dist(0, 2);
  pdu.pucch_format = static_cast<uci_pucch_pdu_format_2_3_4::format_type>(dist(gen));

  // Enable SR, HARQ, CSI part 1. CSI part 2, UCI payload part 1 and UCI payload part 2.
  pdu.pdu_bitmap = 15U;
  pdu.sr         = generate_sr_format234_pdu();
  pdu.harq       = generate_harq_pdu();
  pdu.csi_part1  = generate_csi_part1_pdu();
  pdu.csi_part2  = generate_csi_part2_pdu();
  pdu.uci_part1  = generate_uci_payload();
  pdu.uci_part2  = generate_uci_payload();

  return pdu;
}

uci_indication_message unittest::build_valid_uci_indication()
{
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);

  uci_indication_message msg;

  msg.sfn  = sfn_dist(gen);
  msg.slot = slot_dist(gen);

  // Add one PDU of each type.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type  = uci_pdu_type::PUSCH;
  msg.pdus.back().pusch_pdu = build_valid_uci_push_pdu();

  // Add one PDU of each type.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type      = uci_pdu_type::PUCCH_format_0_1;
  msg.pdus.back().pucch_pdu_f01 = build_valid_uci_pucch_format01_pdu();

  // Add one PDU of each type.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type       = uci_pdu_type::PUCCH_format_2_3_4;
  msg.pdus.back().pucch_pdu_f234 = build_valid_uci_pucch_format234_pdu();

  return msg;
}
