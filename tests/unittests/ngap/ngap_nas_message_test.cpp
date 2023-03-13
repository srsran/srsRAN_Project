/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class ngap_nas_message_routine_test : public ngap_test
{
protected:
  void start_procedure(const ue_index_t ue_index)
  {
    ASSERT_EQ(ngap->get_nof_ues(), 0);
    create_ue(ue_index);
  }

  void start_dl_nas_procedure(ue_index_t ue_index)
  {
    ASSERT_EQ(ngap->get_nof_ues(), 0);
    create_ue(ue_index);

    // Inject DL NAS transport message from AMF
    run_dl_nas_transport(ue_index);
  }

  bool was_dl_nas_transport_forwarded() const { return rrc_ue_notifier.last_nas_pdu.length() == nas_pdu_len; }

  bool was_dl_nas_transport_dropped() const { return rrc_ue_notifier.last_nas_pdu.length() == 0; }

  bool was_ul_nas_transport_forwarded() const
  {
    return msg_notifier.last_ngap_msg.pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ul_nas_transport;
  }
};

/// Initial UE message tests
TEST_F(ngap_nas_message_routine_test, when_initial_ue_message_is_received_then_ngap_ue_is_created)
{
  ASSERT_EQ(ngap->get_nof_ues(), 0);

  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max)));
  this->start_procedure(ue_index);

  // check that initial UE message is sent to AMF and that UE objects has been created
  ASSERT_EQ(msg_notifier.last_ngap_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(ngap->get_nof_ues(), 1);
}

/// Test DL NAS transport handling
TEST_F(ngap_nas_message_routine_test, when_ue_present_dl_nas_transport_is_forwarded)
{
  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max)));
  this->start_procedure(ue_index);

  auto& ue     = test_ues.at(ue_index);
  ue.amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));

  // Inject DL NAS transport message from AMF
  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);

  // Check that RRC notifier was called
  ASSERT_TRUE(was_dl_nas_transport_forwarded());
}

TEST_F(ngap_nas_message_routine_test, when_no_ue_present_dl_nas_transport_is_dropped)
{
  // Inject DL NAS transport message from AMF
  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(

      uint_to_amf_ue_id(
          test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max))),
      uint_to_ran_ue_id(
          test_rgen::uniform_int<uint64_t>(ran_ue_id_to_uint(ran_ue_id_t::min), ran_ue_id_to_uint(ran_ue_id_t::max))));
  ngap->handle_message(dl_nas_transport);

  // Check that no message has been sent to RRC
  ASSERT_TRUE(was_dl_nas_transport_dropped());
}

/// Test UL NAS transport handling
TEST_F(ngap_nas_message_routine_test, when_ue_present_and_amf_set_ul_nas_transport_is_forwared)
{
  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max)));
  this->start_dl_nas_procedure(ue_index);

  ngap_ul_nas_transport_message ul_nas_transport = generate_ul_nas_transport_message(ue_index);
  ngap->handle_ul_nas_transport_message(ul_nas_transport);

  // Check that AMF notifier was called with right type
  ASSERT_TRUE(was_ul_nas_transport_forwarded());
}
