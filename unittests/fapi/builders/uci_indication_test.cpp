/*
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

using namespace srsgnb;
using namespace fapi;

static std::mt19937 gen(0);

static void test_uci_pusch_pdu_metrics()
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

    TESTASSERT_EQ((ul_sinr_metric) ? static_cast<int>(ul_sinr_metric.value() * 500.F) : -32768, pdu.ul_sinr_metric);
    TESTASSERT_EQ((timing_advance_offset) ? timing_advance_offset.value() : 65535U, pdu.timing_advance_offset);
    TESTASSERT_EQ((timing_advance_offset_ns) ? timing_advance_offset_ns.value() : -32768, pdu.timing_advance_offset_ns);
    TESTASSERT_EQ((rssi) ? static_cast<unsigned>((rssi.value() + 128.F) * 10.F) : 65535U, pdu.rssi);
    TESTASSERT_EQ((rsrp) ? static_cast<unsigned>((rsrp.value() + ((rsrp_use_dB) ? 140.F : 128.F)) * 10.F) : 65535U,
                  pdu.rsrp);
  }
}

static void test_uci_pusch_pdu_harq()
{
  std::uniform_int_distribution<unsigned> detection_dist(1, 5);
  std::uniform_int_distribution<unsigned> length_dist(1, 1706);

  uci_pusch_pdu         pdu;
  uci_pusch_pdu_builder builder(pdu);

  uci_detection_status status     = static_cast<uci_detection_status>(detection_dist(gen));
  unsigned             bit_length = length_dist(gen);
  static_vector<uint8_t, uci_harq_pdu::MAX_UCI_HARQ_LEN> payload = {2, 3, 4, 5};

  builder.set_harq_parameters(status, bit_length, {payload});

  const auto& harq = pdu.harq;
  TESTASSERT((pdu.pdu_bitmap >> uci_pusch_pdu::HARQ_BIT) & 1U);
  TESTASSERT_EQ(status, harq.detection_status);
  TESTASSERT_EQ(bit_length, harq.bit_length);
  TESTASSERT(payload == harq.payload);
}

static void test_uci_pusch_pdu_csi_part1()
{
  std::uniform_int_distribution<unsigned> detection_dist(1, 5);
  std::uniform_int_distribution<unsigned> length_dist(1, 1706);

  uci_pusch_pdu         pdu;
  uci_pusch_pdu_builder builder(pdu);

  uci_detection_status status     = static_cast<uci_detection_status>(detection_dist(gen));
  unsigned             bit_length = length_dist(gen);
  static_vector<uint8_t, uci_csi_part1::MAX_CSI_PART1_LEN> payload = {2, 3, 4, 5};

  builder.set_csi_part1_parameters(status, bit_length, {payload});

  const auto& csi = pdu.csi_part1;
  TESTASSERT((pdu.pdu_bitmap >> uci_pusch_pdu::CSI_PART1_BIT) & 1U);
  TESTASSERT_EQ(status, csi.detection_status);
  TESTASSERT_EQ(bit_length, csi.bit_length);
  TESTASSERT(payload == csi.payload);
}

static void test_uci_pusch_pdu_csi_part2()
{
  std::uniform_int_distribution<unsigned> detection_dist(1, 5);
  std::uniform_int_distribution<unsigned> length_dist(1, 1706);

  uci_pusch_pdu         pdu;
  uci_pusch_pdu_builder builder(pdu);

  uci_detection_status status     = static_cast<uci_detection_status>(detection_dist(gen));
  unsigned             bit_length = length_dist(gen);
  static_vector<uint8_t, uci_csi_part2::MAX_CSI_PART2_LEN> payload = {2, 3, 4, 5};

  builder.set_csi_part2_parameters(status, bit_length, {payload});

  const auto& csi = pdu.csi_part2;
  TESTASSERT((pdu.pdu_bitmap >> uci_pusch_pdu::CSI_PART2_BIT) & 1U);
  TESTASSERT_EQ(status, csi.detection_status);
  TESTASSERT_EQ(bit_length, csi.bit_length);
  TESTASSERT(payload == csi.payload);
}

static void test_uci_pusch_pdu()
{
  test_uci_pusch_pdu_metrics();
  fmt::print("UCI.indication PUSCH PDU metrics parameters -> OK\n");
  test_uci_pusch_pdu_harq();
  fmt::print("UCI.indication PUSCH PDU HARQ parameters -> OK\n");
  test_uci_pusch_pdu_csi_part1();
  fmt::print("UCI.indication PUSCH PDU CSI part 1 parameters -> OK\n");
  test_uci_pusch_pdu_csi_part2();
  fmt::print("UCI.indication PUSCH PDU CSI part 2 parameters -> OK\n");
}

static void test_uci_pucch_format01_pdu_metrics()
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

    TESTASSERT_EQ((ul_sinr_metric) ? static_cast<int>(ul_sinr_metric.value() * 500.F) : -32768, pdu.ul_sinr_metric);
    TESTASSERT_EQ((timing_advance_offset) ? timing_advance_offset.value() : 65535U, pdu.timing_advance_offset);
    TESTASSERT_EQ((timing_advance_offset_ns) ? timing_advance_offset_ns.value() : -32768, pdu.timing_advance_offset_ns);
    TESTASSERT_EQ((rssi) ? static_cast<unsigned>((rssi.value() + 128.F) * 10.F) : 65535U, pdu.rssi);
    TESTASSERT_EQ((rsrp) ? static_cast<unsigned>((rsrp.value() + ((rsrp_use_dB) ? 140.F : 128.F)) * 10.F) : 65535U,
                  pdu.rsrp);
  }
}

static void test_uci_pucch_format01_pdu_sr()
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
    TESTASSERT((pdu.pdu_bitmap >> uci_pucch_pdu_format_0_1::SR_BIT) & 1U);
    TESTASSERT_EQ((confidence) ? confidence.value() : 255U, sr_pdu.sr_confidence_level);
    TESTASSERT_EQ(i, sr_pdu.sr_indication);
  }
}

static void test_uci_pucch_format01_pdu_harq()
{
  for (unsigned i = 0, e = 2; i != e; ++i) {
    uci_pucch_pdu_format_0_1         pdu;
    uci_pucch_pdu_format_0_1_builder builder(pdu);

    optional<unsigned> confidence;
    if (i) {
      confidence.emplace(i);
    }

    static_vector<uint8_t, uci_harq_format_0_1::MAX_NUM_HARQ> payload = {2, 3};

    builder.set_harq_parameters(confidence, {payload});

    const auto& harq = pdu.harq;
    TESTASSERT((pdu.pdu_bitmap >> uci_pucch_pdu_format_0_1::HARQ_BIT) & 1U);
    TESTASSERT_EQ((confidence) ? confidence.value() : 255U, harq.harq_confidence_level);
    TESTASSERT(payload == harq.harq_values);
  }
}

static void test_uci_pucch_format01_pdu()
{
  test_uci_pucch_format01_pdu_metrics();
  fmt::print("UCI.indication PUCCH format 0 or 1 metrics parameters -> OK\n");
  test_uci_pucch_format01_pdu_sr();
  fmt::print("UCI.indication PUCCH format 0 or 1 PDU SR parameters -> OK\n");
  test_uci_pucch_format01_pdu_harq();
  fmt::print("UCI.indication PUCCH format 0 or 1 PDU HARQ parameters -> OK\n");
}

static void test_uci_pucch_format234_pdu_metrics()
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

    TESTASSERT_EQ((ul_sinr_metric) ? static_cast<int>(ul_sinr_metric.value() * 500.F) : -32768, pdu.ul_sinr_metric);
    TESTASSERT_EQ((timing_advance_offset) ? timing_advance_offset.value() : 65535U, pdu.timing_advance_offset);
    TESTASSERT_EQ((timing_advance_offset_ns) ? timing_advance_offset_ns.value() : -32768, pdu.timing_advance_offset_ns);
    TESTASSERT_EQ((rssi) ? static_cast<unsigned>((rssi.value() + 128.F) * 10.F) : 65535U, pdu.rssi);
    TESTASSERT_EQ((rsrp) ? static_cast<unsigned>((rsrp.value() + ((rsrp_use_dB) ? 140.F : 128.F)) * 10.F) : 65535U,
                  pdu.rsrp);
  }
}

static void test_uci_pucch_format234_pdu_sr()
{
  uci_pucch_pdu_format_2_3_4                                       pdu;
  uci_pucch_pdu_format_2_3_4_builder                               builder(pdu);
  std::uniform_int_distribution<unsigned>                          bit_len_dist(1, 4);
  unsigned                                                         bit_len = bit_len_dist(gen);
  static_vector<uint8_t, sr_pdu_format_2_3_4::MAX_SR_PAYLOAD_SIZE> payload = {2};

  builder.set_sr_parameters(bit_len, {payload});

  const auto& sr_pdu = pdu.sr;
  TESTASSERT((pdu.pdu_bitmap >> uci_pucch_pdu_format_2_3_4::SR_BIT) & 1U);
  TESTASSERT_EQ(bit_len, sr_pdu.sr_bitlen);
  TESTASSERT(payload == sr_pdu.sr_payload);
}

static void test_uci_pucch_format234_pdu_harq()
{
  std::uniform_int_distribution<unsigned> detection_dist(1, 5);
  std::uniform_int_distribution<unsigned> length_dist(1, 1706);

  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_detection_status status     = static_cast<uci_detection_status>(detection_dist(gen));
  unsigned             bit_length = length_dist(gen);
  static_vector<uint8_t, uci_harq_pdu::MAX_UCI_HARQ_LEN> payload = {2, 3, 4, 5};

  builder.set_harq_parameters(status, bit_length, {payload});

  const auto& harq = pdu.harq;
  TESTASSERT((pdu.pdu_bitmap >> uci_pucch_pdu_format_2_3_4::HARQ_BIT) & 1U);
  TESTASSERT_EQ(status, harq.detection_status);
  TESTASSERT_EQ(bit_length, harq.bit_length);
  TESTASSERT(payload == harq.payload);
}

static void test_uci_pucch_format234_pdu_csi_part1()
{
  std::uniform_int_distribution<unsigned> detection_dist(1, 5);
  std::uniform_int_distribution<unsigned> length_dist(1, 1706);

  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_detection_status status     = static_cast<uci_detection_status>(detection_dist(gen));
  unsigned             bit_length = length_dist(gen);
  static_vector<uint8_t, uci_csi_part1::MAX_CSI_PART1_LEN> payload = {2, 3, 4, 5};

  builder.set_csi_part1_parameters(status, bit_length, {payload});

  const auto& csi = pdu.csi_part1;
  TESTASSERT((pdu.pdu_bitmap >> uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT) & 1U);
  TESTASSERT_EQ(status, csi.detection_status);
  TESTASSERT_EQ(bit_length, csi.bit_length);
  TESTASSERT(payload == csi.payload);
}

static void test_uci_pucch_format234_pdu_csi_part2()
{
  std::uniform_int_distribution<unsigned> detection_dist(1, 5);
  std::uniform_int_distribution<unsigned> length_dist(1, 1706);

  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_detection_status status     = static_cast<uci_detection_status>(detection_dist(gen));
  unsigned             bit_length = length_dist(gen);
  static_vector<uint8_t, uci_csi_part2::MAX_CSI_PART2_LEN> payload = {2, 3, 4, 5};

  builder.set_csi_part2_parameters(status, bit_length, {payload});

  const auto& csi = pdu.csi_part2;
  TESTASSERT((pdu.pdu_bitmap >> uci_pucch_pdu_format_2_3_4::CSI_PART2_BIT) & 1U);
  TESTASSERT_EQ(status, csi.detection_status);
  TESTASSERT_EQ(bit_length, csi.bit_length);
  TESTASSERT(payload == csi.payload);
}

static void test_uci_pucch_format234_pdu_uci_payload_part1()
{
  std::uniform_int_distribution<unsigned> detection_dist(1, 5);
  std::uniform_int_distribution<unsigned> length_dist(1, 1706);

  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_detection_status status     = static_cast<uci_detection_status>(detection_dist(gen));
  unsigned             bit_length = length_dist(gen);
  static_vector<uint8_t, uci_payload_pusch_pucch::MAX_UCI_PAYLOAD_LEN> payload = {2, 3, 4, 5};

  // Builder won't allow to add an UCI PDU if it's not present a CSI PDU.
  builder.set_csi_part1_parameters(status, bit_length, {});
  builder.set_uci_part1_payload(status, bit_length, {payload});

  const auto& uci = pdu.uci_part1;
  TESTASSERT((pdu.pdu_bitmap >> uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT) & 1U);
  TESTASSERT_EQ(status, uci.detection_status);
  TESTASSERT_EQ(bit_length, uci.expected_uci_payload_size);
  TESTASSERT(payload == uci.payload);
}

static void test_uci_pucch_format234_pdu_uci_payload_part2()
{
  std::uniform_int_distribution<unsigned> detection_dist(1, 5);
  std::uniform_int_distribution<unsigned> length_dist(1, 1706);

  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_detection_status status     = static_cast<uci_detection_status>(detection_dist(gen));
  unsigned             bit_length = length_dist(gen);
  static_vector<uint8_t, uci_payload_pusch_pucch::MAX_UCI_PAYLOAD_LEN> payload = {2, 3, 4, 5};

  // Builder won't allow to add an UCI PDU if it's not present a CSI PDU.
  builder.set_csi_part2_parameters(status, bit_length, {});
  builder.set_uci_part2_payload(status, bit_length, {payload});

  const auto& uci = pdu.uci_part2;
  TESTASSERT((pdu.pdu_bitmap >> uci_pucch_pdu_format_2_3_4::CSI_PART2_BIT) & 1U);
  TESTASSERT_EQ(status, uci.detection_status);
  TESTASSERT_EQ(bit_length, uci.expected_uci_payload_size);
  TESTASSERT(payload == uci.payload);
}

static void test_uci_pucch_format_234_pdu()
{
  test_uci_pucch_format234_pdu_metrics();
  fmt::print("UCI.indication PUCCH format 2,3 or 4 metrics parameters -> OK\n");
  test_uci_pucch_format234_pdu_sr();
  fmt::print("UCI.indication PUCCH format 2,3 or 4 PDU SR parameters -> OK\n");
  test_uci_pucch_format234_pdu_harq();
  fmt::print("UCI.indication PUCCH format 2,3 or 4 PDU HARQ parameters -> OK\n");
  test_uci_pucch_format234_pdu_csi_part1();
  fmt::print("UCI.indication PUCCH format 2,3 or 4 PDU CSI part 1 parameters -> OK\n");
  test_uci_pucch_format234_pdu_csi_part2();
  fmt::print("UCI.indication PUCCH format 2,3 or 4 PDU CSI part 2 parameters -> OK\n");
  test_uci_pucch_format234_pdu_uci_payload_part1();
  fmt::print("UCI.indication PUCCH format 2,3 or 4 PDU UCI payload part 1 parameters -> OK\n");
  test_uci_pucch_format234_pdu_uci_payload_part2();
  fmt::print("UCI.indication PUCCH format 2,3 or 4 PDU UCI payload part 2 parameters -> OK\n");
}

static void test_uci_basic_parameters()
{
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);

  uci_indication_message         msg;
  uci_indication_message_builder builder(msg);

  unsigned sfn  = sfn_dist(gen);
  unsigned slot = slot_dist(gen);

  builder.set_basic_parameters(sfn, slot);

  TESTASSERT_EQ(sfn, msg.sfn);
  TESTASSERT_EQ(slot, msg.slot);
}

static void test_uci_add_pusch_pdu()
{
  std::uniform_int_distribution<unsigned> handle_dist(0, 1023);
  std::uniform_int_distribution<unsigned> rnti_dist(1, 65535);

  uci_indication_message         msg;
  uci_indication_message_builder builder(msg);

  unsigned handle = handle_dist(gen);
  rnti_t   rnti   = to_rnti(rnti_dist(gen));

  builder.add_pusch_pdu(handle, rnti);

  TESTASSERT_EQ(1, msg.pdus.size());
  TESTASSERT_EQ(uci_pdu_type::PUSCH, msg.pdus.back().pdu_type);

  const auto& pdu = msg.pdus.back().pusch_pdu;
  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(handle, pdu.handle);
}

static void test_uci_add_pucch_format_0_1_pdu()
{
  std::uniform_int_distribution<unsigned> handle_dist(0, 1023);
  std::uniform_int_distribution<unsigned> rnti_dist(1, 65535);
  std::uniform_int_distribution<unsigned> format_dist(0, 1);

  uci_indication_message         msg;
  uci_indication_message_builder builder(msg);

  unsigned                              handle = handle_dist(gen);
  rnti_t                                rnti   = to_rnti(rnti_dist(gen));
  uci_pucch_pdu_format_0_1::format_type format = static_cast<uci_pucch_pdu_format_0_1::format_type>(format_dist(gen));

  builder.add_uci_pucch_pdu_format_0_1_pdu(handle, rnti, format);

  TESTASSERT_EQ(1, msg.pdus.size());
  TESTASSERT_EQ(uci_pdu_type::PUCCH_format_0_1, msg.pdus.back().pdu_type);

  const auto& pdu = msg.pdus.back().pucch_pdu_f01;
  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(handle, pdu.handle);
  TESTASSERT_EQ(format, pdu.pucch_format);
}

static void test_uci_add_pucch_format_2_3_4_pdu()
{
  std::uniform_int_distribution<unsigned> handle_dist(0, 1023);
  std::uniform_int_distribution<unsigned> rnti_dist(1, 65535);
  std::uniform_int_distribution<unsigned> format_dist(0, 2);

  uci_indication_message         msg;
  uci_indication_message_builder builder(msg);

  unsigned                                handle = handle_dist(gen);
  rnti_t                                  rnti   = to_rnti(rnti_dist(gen));
  uci_pucch_pdu_format_2_3_4::format_type format =
      static_cast<uci_pucch_pdu_format_2_3_4::format_type>(format_dist(gen));

  builder.add_uci_pucch_pdu_format_2_3_4_pdu(handle, rnti, format);

  TESTASSERT_EQ(1, msg.pdus.size());
  TESTASSERT_EQ(uci_pdu_type::PUCCH_format_2_3, msg.pdus.back().pdu_type);

  const auto& pdu = msg.pdus.back().pucch_pdu_f234;
  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(handle, pdu.handle);
  TESTASSERT_EQ(format, pdu.pucch_format);
}

static void test_uci_indication_ok()
{
  test_uci_basic_parameters();
  fmt::print("UCI.indication basic parameters -> OK\n");
  test_uci_add_pusch_pdu();
  fmt::print("UCI.indication add PUSCH PDU -> OK\n");
  test_uci_add_pucch_format_0_1_pdu();
  fmt::print("UCI.indication add PUCCH format 0 or 1 PDU -> OK\n");
  test_uci_add_pucch_format_2_3_4_pdu();
  fmt::print("UCI.indication add PUCCH format 2,3 or 4 PDU -> OK\n");

  test_uci_pusch_pdu();
  test_uci_pucch_format01_pdu();
  test_uci_pucch_format_234_pdu();
}

int main()
{
  test_uci_indication_ok();
  fmt::print("UCI.indication builder -> OK\n");
}