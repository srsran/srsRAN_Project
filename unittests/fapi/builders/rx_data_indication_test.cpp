/*
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

static void test_basic_parameters()
{
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);

  unsigned sfn            = sfn_dist(gen);
  unsigned slot           = slot_dist(gen);
  unsigned control_length = sfn_dist(gen);

  rx_data_indication_message         msg;
  rx_data_indication_message_builder builder(msg);

  builder.set_basic_parameters(sfn, slot, control_length);

  TESTASSERT_EQ(sfn, msg.sfn);
  TESTASSERT_EQ(slot, msg.slot);
  TESTASSERT_EQ(control_length, msg.control_length);
}

static void test_add_custom_pdu()
{
  std::uniform_int_distribution<unsigned> dist(1, 65535);
  std::uniform_int_distribution<unsigned> harq_dist(0, 15);

  static_vector<uint8_t, 18> data = {1, 2, 3, 4, 5};

  unsigned handle = dist(gen);
  rnti_t   rnti   = to_rnti(dist(gen));

  unsigned           rapid_value = dist(gen);
  optional<unsigned> rapid;
  if (rapid_value < 64U) {
    rapid.emplace(rapid_value);
  }

  unsigned harq = harq_dist(gen);

  rx_data_indication_message         msg;
  rx_data_indication_message_builder builder(msg);

  builder.add_custom_pdu(handle, rnti, rapid, harq, {data});

  const auto& pdu = msg.pdus.back();
  TESTASSERT_EQ(handle, pdu.handle);
  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(rapid ? static_cast<uint8_t>(rapid.value()) : 255U, pdu.rapid);
  TESTASSERT_EQ(harq, pdu.harq_id);
  TESTASSERT_EQ(rx_data_indication_pdu::pdu_tag_type::custom, pdu.pdu_tag);
  TESTASSERT_EQ(data.size(), pdu.pdu_length);
  TESTASSERT_EQ(data.data(), pdu.data);
}

static void test_rx_data_indication_builder_ok()
{
  test_basic_parameters();
  test_add_custom_pdu();
}

int main()
{
  test_rx_data_indication_builder_ok();

  fmt::print("Rx_Data.indication message builder -> Ok\n");
}
