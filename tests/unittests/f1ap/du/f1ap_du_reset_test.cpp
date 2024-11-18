/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_test_helpers.h"
#include "lib/f1ap/du/procedures/f1ap_du_reset_procedure.h"
#include "srsran/asn1/f1ap/common.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

static f1ap_message create_f1ap_reset_message(
    const std::vector<std::pair<std::optional<gnb_du_ue_f1ap_id_t>, std::optional<gnb_cu_ue_f1ap_id_t>>>& ues_to_reset =
        {})
{
  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_RESET);

  asn1::f1ap::reset_s& reset = msg.pdu.init_msg().value.reset();
  reset->cause.set_misc();
  reset->cause.misc().value = asn1::f1ap::cause_misc_opts::unspecified;

  if (ues_to_reset.empty()) {
    auto& f1 = reset->reset_type.set_f1_interface();
    f1.value = asn1::f1ap::reset_all_opts::reset_all;
  } else {
    auto& lst = reset->reset_type.set_part_of_f1_interface();
    lst.resize(ues_to_reset.size());
    for (unsigned i = 0; i != ues_to_reset.size(); ++i) {
      lst[i].load_info_obj(ASN1_F1AP_ID_UE_ASSOCIATED_LC_F1_CONN_ITEM);
      auto& conn                     = lst[i].value().ue_associated_lc_f1_conn_item();
      conn.gnb_du_ue_f1ap_id_present = ues_to_reset[i].first.has_value();
      if (conn.gnb_du_ue_f1ap_id_present) {
        conn.gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_to_uint(ues_to_reset[i].first.value());
      }
      conn.gnb_cu_ue_f1ap_id_present = ues_to_reset[i].second.has_value();
      if (conn.gnb_cu_ue_f1ap_id_present) {
        conn.gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_to_uint(ues_to_reset[i].second.value());
      }
    }
  }

  return msg;
}

static bool is_f1_reset_ack(const f1ap_message& msg)
{
  if (msg.pdu.type().value != asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome) {
    return false;
  }
  if (msg.pdu.successful_outcome().value.type().value !=
      asn1::f1ap::f1ap_elem_procs_o::successful_outcome_c::types_opts::reset_ack) {
    return false;
  }
  return true;
}

static bool is_valid_f1_reset(const f1ap_message& req, const f1ap_message& msg)
{
  using namespace asn1::f1ap;

  if (not is_f1_reset_ack(msg)) {
    return false;
  }

  const auto& reset = req.pdu.init_msg().value.reset();

  const reset_ack_s& ack = msg.pdu.successful_outcome().value.reset_ack();
  if (ack->transaction_id != reset->transaction_id) {
    return false;
  }
  if (reset->reset_type.type().value == reset_type_c::types_opts::f1_interface) {
    if (ack->ue_associated_lc_f1_conn_list_res_ack_present) {
      return false;
    }
  } else if (reset->reset_type.type().value == reset_type_c::types_opts::part_of_f1_interface) {
    if (not ack->ue_associated_lc_f1_conn_list_res_ack_present) {
      return false;
    }
  }

  return true;
}

class f1_reset_test : public f1ap_du_test
{
public:
  f1_reset_test()
  {
    // Test Preamble.
    run_f1_setup_procedure();

    this->f1c_gw.clear_tx_pdus();
  }
};

TEST_F(f1_reset_test, when_f1_reset_is_received_but_no_ue_is_created_then_no_request_is_forwarded_to_du)
{
  this->f1ap->handle_message(create_f1ap_reset_message());

  ASSERT_FALSE(this->f1ap_du_cfg_handler.last_ues_to_reset.has_value());
}

TEST_F(f1_reset_test, when_f1_reset_is_received_then_reset_request_is_forwarded_to_du)
{
  du_ue_index_t test_ue_index = to_du_ue_index(0);
  this->run_f1ap_ue_create(test_ue_index);

  auto req = create_f1ap_reset_message();
  this->f1ap->handle_message(req);

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ues_to_reset.has_value());
  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ues_to_reset.value().empty());

  ASSERT_TRUE(is_valid_f1_reset(req, this->f1c_gw.last_tx_pdu()));
}

TEST_F(f1_reset_test, when_f1_reset_is_received_for_a_specific_ue_then_reset_request_is_forwarded_to_du_with_ue)
{
  du_ue_index_t test_ue_index = to_du_ue_index(0);
  this->run_f1ap_ue_create(test_ue_index);

  auto req = create_f1ap_reset_message({{gnb_du_ue_f1ap_id_t{0}, std::nullopt}});
  this->f1ap->handle_message(req);

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ues_to_reset.has_value());
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_ues_to_reset.value().size(), 1);
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_ues_to_reset.value()[0], test_ue_index);

  ASSERT_TRUE(is_valid_f1_reset(req, this->f1c_gw.last_tx_pdu()));
}
