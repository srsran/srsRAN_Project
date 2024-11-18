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

static f1ap_message create_f1ap_reset_message()
{
  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_RESET);

  asn1::f1ap::reset_s& reset = msg.pdu.init_msg().value.reset();
  reset->cause.set_misc();
  reset->cause.misc().value = asn1::f1ap::cause_misc_opts::unspecified;

  auto& f1 = reset->reset_type.set_f1_interface();
  f1.value = asn1::f1ap::reset_all_opts::reset_all;

  return msg;
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