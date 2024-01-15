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

#include "srsran/fapi/message_builders.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi;

TEST(uci_indication_builder, valid_pusch_pdu_metrics_passes)
{
  for (unsigned i = 0, e = 2; i != e; ++i) {
    uci_pusch_pdu         pdu;
    uci_pusch_pdu_builder builder(pdu);

    optional<float>    ul_sinr_metric;
    optional<unsigned> timing_advance_offset;
    optional<int>      timing_advance_offset_ns;
    optional<float>    rssi;
    optional<float>    rsrp;
    bool               rsrp_use_dB = i;

    if (i) {
      ul_sinr_metric.emplace(-30.1F);
      timing_advance_offset.emplace(10);
      timing_advance_offset_ns.emplace(-100);
      rssi.emplace(-59.3F);
      rsrp.emplace(-36.2F);
    }

    builder.set_metrics_parameters(
        ul_sinr_metric, timing_advance_offset, timing_advance_offset_ns, rssi, rsrp, rsrp_use_dB);

    ASSERT_EQ((ul_sinr_metric) ? static_cast<int>(ul_sinr_metric.value() * 500.F) : -32768, pdu.ul_sinr_metric);
    ASSERT_EQ((timing_advance_offset) ? timing_advance_offset.value() : 65535U, pdu.timing_advance_offset);
    ASSERT_EQ((timing_advance_offset_ns) ? timing_advance_offset_ns.value() : -32768, pdu.timing_advance_offset_ns);
    ASSERT_EQ((rssi) ? static_cast<unsigned>((rssi.value() + 128.F) * 10.F) : 65535U, pdu.rssi);
    ASSERT_EQ((rsrp) ? static_cast<unsigned>((rsrp.value() + ((rsrp_use_dB) ? 140.F : 128.F)) * 10.F) : 65535U,
              pdu.rsrp);
  }
}

TEST(uci_indication_builder, valid_pusch_pdu_harq_passes)
{
  uci_pusch_pdu         pdu;
  uci_pusch_pdu_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status       status = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(5);
  unsigned                                         bit_length = 1023;
  bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS> payload(bit_length);

  builder.set_harq_parameters(status, payload.size(), payload);

  const auto& harq = pdu.harq;
  ASSERT_TRUE(pdu.pdu_bitmap[uci_pusch_pdu::HARQ_BIT]);
  ASSERT_EQ(status, harq.detection_status);
  ASSERT_EQ(bit_length, harq.expected_bit_length);
  ASSERT_EQ(payload, harq.payload);
}

TEST(uci_indication_builder, valid_pusch_pdu_csi_part1_passes)
{
  uci_pusch_pdu         pdu;
  uci_pusch_pdu_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status     = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(1);
  unsigned                                   bit_length = 129;
  bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload(bit_length);

  builder.set_csi_part1_parameters(status, payload.size(), payload);

  const auto& csi = pdu.csi_part1;
  ASSERT_TRUE(pdu.pdu_bitmap[uci_pusch_pdu::CSI_PART1_BIT]);
  ASSERT_EQ(status, csi.detection_status);
  ASSERT_EQ(bit_length, csi.expected_bit_length);
  ASSERT_EQ(payload, csi.payload);
}

TEST(uci_indication_builder, valid_pusch_pdu_csi_part2_passes)
{
  uci_pusch_pdu         pdu;
  uci_pusch_pdu_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status     = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(3);
  unsigned                                   bit_length = 98;
  bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload(bit_length);

  builder.set_csi_part2_parameters(status, payload.size(), payload);

  const auto& csi = pdu.csi_part2;
  ASSERT_TRUE(pdu.pdu_bitmap[uci_pusch_pdu::CSI_PART2_BIT]);
  ASSERT_EQ(status, csi.detection_status);
  ASSERT_EQ(bit_length, csi.expected_bit_length);
  ASSERT_EQ(payload, csi.payload);
}

TEST(uci_indication_builder, valid_pucch_f01_pdu_metrics_passes)
{
  for (unsigned i = 0, e = 2; i != e; ++i) {
    uci_pucch_pdu_format_0_1         pdu;
    uci_pucch_pdu_format_0_1_builder builder(pdu);

    optional<float>    ul_sinr_metric;
    optional<unsigned> timing_advance_offset;
    optional<int>      timing_advance_offset_ns;
    optional<float>    rssi;
    optional<float>    rsrp;
    bool               rsrp_use_dB = i;

    if (i) {
      ul_sinr_metric.emplace(-30.1F);
      timing_advance_offset.emplace(10);
      timing_advance_offset_ns.emplace(-100);
      rssi.emplace(-59.3F);
      rsrp.emplace(-36.2F);
    }

    builder.set_metrics_parameters(
        ul_sinr_metric, timing_advance_offset, timing_advance_offset_ns, rssi, rsrp, rsrp_use_dB);

    ASSERT_EQ((ul_sinr_metric) ? static_cast<int>(ul_sinr_metric.value() * 500.F) : -32768, pdu.ul_sinr_metric);
    ASSERT_EQ((timing_advance_offset) ? timing_advance_offset.value() : 65535U, pdu.timing_advance_offset);
    ASSERT_EQ((timing_advance_offset_ns) ? timing_advance_offset_ns.value() : -32768, pdu.timing_advance_offset_ns);
    ASSERT_EQ((rssi) ? static_cast<unsigned>((rssi.value() + 128.F) * 10.F) : 65535U, pdu.rssi);
    ASSERT_EQ((rsrp) ? static_cast<unsigned>((rsrp.value() + ((rsrp_use_dB) ? 140.F : 128.F)) * 10.F) : 65535U,
              pdu.rsrp);
  }
}

TEST(uci_indication_builder, valid_pucch_f01_pdu_sr_passes)
{
  for (unsigned i = 0, e = 2; i != e; ++i) {
    uci_pucch_pdu_format_0_1         pdu;
    uci_pucch_pdu_format_0_1_builder builder(pdu);

    optional<unsigned> confidence;
    if (i) {
      confidence.emplace(i);
    }

    static_vector<uint8_t, uci_harq_format_0_1::MAX_NUM_HARQ> payload = {2, 3};

    builder.set_sr_parameters(i, confidence);

    const auto& sr_pdu = pdu.sr;
    ASSERT_TRUE(pdu.pdu_bitmap[uci_pucch_pdu_format_0_1::SR_BIT]);
    ASSERT_EQ((confidence) ? confidence.value() : 255U, sr_pdu.sr_confidence_level);
    ASSERT_EQ(i, sr_pdu.sr_indication);
  }
}

TEST(uci_indication_builder, valid_pucch_f01_pdu_harq_passes)
{
  for (unsigned i = 0, e = 2; i != e; ++i) {
    uci_pucch_pdu_format_0_1         pdu;
    uci_pucch_pdu_format_0_1_builder builder(pdu);

    optional<unsigned> confidence;
    if (i) {
      confidence.emplace(i);
    }

    static_vector<uci_pucch_f0_or_f1_harq_values, uci_harq_format_0_1::MAX_NUM_HARQ> payload = {
        uci_pucch_f0_or_f1_harq_values::ack, uci_pucch_f0_or_f1_harq_values::ack};

    builder.set_harq_parameters(confidence, {payload});

    const auto& harq = pdu.harq;
    ASSERT_TRUE(pdu.pdu_bitmap[uci_pucch_pdu_format_0_1::HARQ_BIT]);
    ASSERT_EQ((confidence) ? confidence.value() : 255U, harq.harq_confidence_level);
    ASSERT_EQ(payload, harq.harq_values);
  }
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_metrics_passes)
{
  for (unsigned i = 0, e = 2; i != e; ++i) {
    uci_pucch_pdu_format_2_3_4         pdu;
    uci_pucch_pdu_format_2_3_4_builder builder(pdu);

    optional<float>    ul_sinr_metric;
    optional<unsigned> timing_advance_offset;
    optional<int>      timing_advance_offset_ns;
    optional<float>    rssi;
    optional<float>    rsrp;
    bool               rsrp_use_dB = i;

    if (i) {
      ul_sinr_metric.emplace(-30.1F);
      timing_advance_offset.emplace(10);
      timing_advance_offset_ns.emplace(-100);
      rssi.emplace(-59.3F);
      rsrp.emplace(-36.2F);
    }

    builder.set_metrics_parameters(
        ul_sinr_metric, timing_advance_offset, timing_advance_offset_ns, rssi, rsrp, rsrp_use_dB);
    ASSERT_EQ((ul_sinr_metric) ? static_cast<int>(ul_sinr_metric.value() * 500.F) : -32768, pdu.ul_sinr_metric);
    ASSERT_EQ((timing_advance_offset) ? timing_advance_offset.value() : 65535U, pdu.timing_advance_offset);
    ASSERT_EQ((timing_advance_offset_ns) ? timing_advance_offset_ns.value() : -32768, pdu.timing_advance_offset_ns);
    ASSERT_EQ((rssi) ? static_cast<unsigned>((rssi.value() + 128.F) * 10.F) : 65535U, pdu.rssi);
    ASSERT_EQ((rsrp) ? static_cast<unsigned>((rsrp.value() + ((rsrp_use_dB) ? 140.F : 128.F)) * 10.F) : 65535U,
              pdu.rsrp);
  }
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_sr_passes)
{
  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  unsigned                                                      bit_len = 3;
  bounded_bitset<sr_pdu_format_2_3_4::MAX_SR_PAYLOAD_SIZE_BITS> payload(2);

  builder.set_sr_parameters(bit_len, payload);

  const auto& sr_pdu = pdu.sr;
  ASSERT_TRUE(pdu.pdu_bitmap[uci_pucch_pdu_format_2_3_4::SR_BIT]);
  ASSERT_EQ(bit_len, sr_pdu.sr_bitlen);
  ASSERT_EQ(payload, sr_pdu.sr_payload);
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_harq_passes)
{
  std::uniform_int_distribution<unsigned> detection_dist(1, 5);
  std::uniform_int_distribution<unsigned> length_dist(1, 1706);

  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status       status = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(4);
  unsigned                                         bit_length = 40;
  bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS> payload(bit_length);

  builder.set_harq_parameters(status, payload.size(), payload);

  const auto& harq = pdu.harq;
  ASSERT_TRUE(pdu.pdu_bitmap[uci_pucch_pdu_format_2_3_4::HARQ_BIT]);
  ASSERT_EQ(status, harq.detection_status);
  ASSERT_EQ(bit_length, harq.expected_bit_length);
  ASSERT_EQ(payload, harq.payload);
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_csi_part1_passes)
{
  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status     = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(3);
  unsigned                                   bit_length = 28;
  bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload(bit_length);

  builder.set_csi_part1_parameters(status, payload.size(), payload);

  const auto& csi = pdu.csi_part1;
  ASSERT_TRUE(pdu.pdu_bitmap[uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT]);
  ASSERT_EQ(status, csi.detection_status);
  ASSERT_EQ(bit_length, csi.expected_bit_length);
  ASSERT_EQ(payload, csi.payload);
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_csi_part2_passes)
{
  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status     = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(2);
  unsigned                                   bit_length = 15;
  bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload(bit_length);

  builder.set_csi_part2_parameters(status, payload.size(), payload);

  const auto& csi = pdu.csi_part2;
  ASSERT_TRUE(pdu.pdu_bitmap[uci_pucch_pdu_format_2_3_4::CSI_PART2_BIT]);
  ASSERT_EQ(status, csi.detection_status);
  ASSERT_EQ(bit_length, csi.expected_bit_length);
  ASSERT_EQ(payload, csi.payload);
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_csi_payoad_part1_passes)
{
  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status     = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(2);
  unsigned                                   bit_length = 3;
  bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload(bit_length);

  // Builder won't allow to add an UCI PDU if it's not present a CSI PDU.
  builder.set_csi_part1_parameters(status, bit_length, payload);

  ASSERT_EQ(bit_length, pdu.csi_part1.expected_bit_length);
  ASSERT_EQ(status, pdu.csi_part1.detection_status);
  ASSERT_EQ(payload, pdu.csi_part1.payload);
  ASSERT_TRUE(pdu.pdu_bitmap[uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT]);
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_csi_payoad_part2_passes)
{
  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status     = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(3);
  unsigned                                   bit_length = 4;
  bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload(bit_length);

  // Builder won't allow to add an UCI PDU if it's not present a CSI PDU.
  builder.set_csi_part2_parameters(status, bit_length, payload);

  ASSERT_EQ(bit_length, pdu.csi_part2.expected_bit_length);
  ASSERT_EQ(status, pdu.csi_part2.detection_status);
  ASSERT_EQ(payload, pdu.csi_part2.payload);
  ASSERT_TRUE(pdu.pdu_bitmap[uci_pucch_pdu_format_2_3_4::CSI_PART2_BIT]);
}

TEST(uci_indication_builder, valid_basic_parameters_passes)
{
  uci_indication_message         msg;
  uci_indication_message_builder builder(msg);

  unsigned sfn  = 13;
  unsigned slot = 14;

  builder.set_basic_parameters(sfn, slot);

  ASSERT_EQ(sfn, msg.sfn);
  ASSERT_EQ(slot, msg.slot);
}

TEST(uci_indication_builder, add_pusch_pdu_passes)
{
  uci_indication_message         msg;
  uci_indication_message_builder builder(msg);

  unsigned handle = 8;
  rnti_t   rnti   = to_rnti(9);

  builder.add_pusch_pdu(handle, rnti);

  ASSERT_EQ(1, msg.pdus.size());
  ASSERT_EQ(uci_pdu_type::PUSCH, msg.pdus.back().pdu_type);

  const auto& pdu = msg.pdus.back().pusch_pdu;
  ASSERT_EQ(to_value(rnti), pdu.rnti);
  ASSERT_EQ(handle, pdu.handle);
}

TEST(uci_indication_builder, add_pucch_f01_passes)
{
  uci_indication_message         msg;
  uci_indication_message_builder builder(msg);

  unsigned     handle = 4;
  rnti_t       rnti   = to_rnti(5);
  pucch_format format = static_cast<pucch_format>(0);

  builder.add_format_0_1_pucch_pdu(handle, rnti, format);

  ASSERT_EQ(1, msg.pdus.size());
  ASSERT_EQ(uci_pdu_type::PUCCH_format_0_1, msg.pdus.back().pdu_type);

  const auto& pdu = msg.pdus.back().pucch_pdu_f01;
  ASSERT_EQ(to_value(rnti), pdu.rnti);
  ASSERT_EQ(handle, pdu.handle);
  ASSERT_EQ((format == srsran::pucch_format::FORMAT_0) ? uci_pucch_pdu_format_0_1::format_type::format_0
                                                       : uci_pucch_pdu_format_0_1::format_type::format_1,
            pdu.pucch_format);
}

TEST(uci_indication_builder, add_pucch_f234_passes)
{
  uci_indication_message         msg;
  uci_indication_message_builder builder(msg);

  unsigned     handle = 3;
  rnti_t       rnti   = to_rnti(4);
  pucch_format format = static_cast<pucch_format>(3);

  builder.add_format_2_3_4_pucch_pdu(handle, rnti, format);

  ASSERT_EQ(1, msg.pdus.size());
  ASSERT_EQ(uci_pdu_type::PUCCH_format_2_3_4, msg.pdus.back().pdu_type);

  const auto& pdu = msg.pdus.back().pucch_pdu_f234;
  ASSERT_EQ(to_value(rnti), pdu.rnti);
  ASSERT_EQ(handle, pdu.handle);
  ASSERT_EQ((static_cast<unsigned>(format) - 2U), static_cast<unsigned>(pdu.pucch_format));
}
