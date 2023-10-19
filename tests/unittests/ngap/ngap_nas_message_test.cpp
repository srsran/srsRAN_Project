/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ngap_test_helpers.h"
#include "srsran/ran/cause.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class ngap_nas_message_routine_test : public ngap_test
{
protected:
  ue_index_t start_procedure() { return create_ue(); }

  ue_index_t start_dl_nas_procedure()
  {
    ue_index_t ue_index = create_ue();

    // Inject DL NAS transport message from AMF
    run_dl_nas_transport(ue_index);

    return ue_index;
  }

  bool was_dl_nas_transport_forwarded(const test_ue& ue) const
  {
    return ue.rrc_ue_notifier.last_nas_pdu.length() == nas_pdu_len;
  }

  bool was_dl_nas_transport_dropped(const test_ue& ue) const { return ue.rrc_ue_notifier.last_nas_pdu.empty(); }

  bool was_ul_nas_transport_forwarded() const
  {
    return msg_notifier.last_ngap_msg.pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ul_nas_transport;
  }

  bool was_error_indication_sent() const
  {
    return msg_notifier.last_ngap_msg.pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::error_ind;
  }
};

/// Initial UE message tests
TEST_F(ngap_nas_message_routine_test, when_initial_ue_message_is_received_then_ngap_ue_is_created)
{
  ASSERT_EQ(ngap->get_nof_ues(), 0);

  // Test preamble
  this->start_procedure();

  // check that initial UE message is sent to AMF and that UE objects has been created
  ASSERT_EQ(msg_notifier.last_ngap_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(msg_notifier.last_ngap_msg.pdu.init_msg().value.type(),
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
  ASSERT_EQ(ngap->get_nof_ues(), 1);
}

/// Initial UE message tests
TEST_F(ngap_nas_message_routine_test, when_initial_context_setup_request_is_not_received_then_ue_is_released)
{
  ASSERT_EQ(ngap->get_nof_ues(), 0);

  // Test preamble
  this->start_procedure();

  // check that initial UE message is sent to AMF and that UE objects has been created
  ASSERT_EQ(msg_notifier.last_ngap_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(msg_notifier.last_ngap_msg.pdu.init_msg().value.type(),
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
  ASSERT_EQ(ngap->get_nof_ues(), 1);

  // tick timers
  // Status: NGAP does not receive new Initial Context Setup Request until ue_context_setup_timer has ended.
  for (unsigned msec_elapsed = 0; msec_elapsed < cfg.ue_context_setup_timeout_s.count() * 1000; ++msec_elapsed) {
    this->tick();
  }

  // check that UE release was requested
  ASSERT_NE(du_processor_notifier->last_command.ue_index, ue_index_t::invalid);
  ASSERT_EQ(du_processor_notifier->last_command.cause, cause_t{cause_nas_t::unspecified});
}

/// Test DL NAS transport handling
TEST_F(ngap_nas_message_routine_test, when_ue_present_dl_nas_transport_is_forwarded)
{
  // Test preamble
  ue_index_t ue_index = this->start_procedure();

  auto& ue     = test_ues.at(ue_index);
  ue.amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));

  // Inject DL NAS transport message from AMF
  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);

  // Check that RRC notifier was called
  ASSERT_TRUE(was_dl_nas_transport_forwarded(ue));
}

TEST_F(ngap_nas_message_routine_test, when_no_ue_present_dl_nas_transport_is_dropped_and_error_indication_is_sent)
{
  // Inject DL NAS transport message from AMF
  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(
      uint_to_amf_ue_id(
          test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max))),
      uint_to_ran_ue_id(
          test_rgen::uniform_int<uint64_t>(ran_ue_id_to_uint(ran_ue_id_t::min), ran_ue_id_to_uint(ran_ue_id_t::max))));
  ngap->handle_message(dl_nas_transport);

  // Check that Error Indication has been sent to AMF
  ASSERT_TRUE(was_error_indication_sent());
}

/// Test UL NAS transport handling
TEST_F(ngap_nas_message_routine_test, when_ue_present_and_amf_set_ul_nas_transport_is_forwared)
{
  // Test preamble
  ue_index_t ue_index = this->start_dl_nas_procedure();

  cu_cp_ul_nas_transport ul_nas_transport = generate_ul_nas_transport_message(ue_index);
  ngap->handle_ul_nas_transport_message(ul_nas_transport);

  // Check that AMF notifier was called with right type
  ASSERT_TRUE(was_ul_nas_transport_forwarded());
}

/// Test AMF UE ID handling
TEST_F(ngap_nas_message_routine_test, when_amf_ue_id_is_max_size_then_its_not_cropped)
{
  // Test preamble
  ue_index_t ue_index = this->start_procedure();

  auto& ue     = test_ues.at(ue_index);
  ue.amf_ue_id = amf_ue_id_t::max;

  // Inject DL NAS transport message from AMF
  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);

  // Check that RRC notifier was called
  ASSERT_TRUE(was_dl_nas_transport_forwarded(ue));

  cu_cp_ul_nas_transport ul_nas_transport = generate_ul_nas_transport_message(ue_index);
  ngap->handle_ul_nas_transport_message(ul_nas_transport);

  // Check that AMF notifier was called with right type
  ASSERT_TRUE(was_ul_nas_transport_forwarded());

  ASSERT_EQ(msg_notifier.last_ngap_msg.pdu.init_msg().value.ul_nas_transport()->amf_ue_ngap_id,
            amf_ue_id_to_uint(amf_ue_id_t::max));
}
