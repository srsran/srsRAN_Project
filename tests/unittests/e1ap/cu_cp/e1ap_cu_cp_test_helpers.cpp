/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1ap_cu_cp_test_helpers.h"
#include "srsgnb/support/async/async_test_utils.h"

using namespace srsgnb;
using namespace srs_cu_cp;

e1ap_cu_cp_test::e1ap_cu_cp_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  e1ap_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  e1ap = create_e1(timers, e1_pdu_notifier, cu_up_processor_notifier, ctrl_worker);
}

e1ap_cu_cp_test::~e1ap_cu_cp_test()
{
  // flush logger after each test
  srslog::flush();
}

void e1ap_cu_cp_test::run_bearer_context_setup(ue_index_t ue_index, gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_bearer_context_setup_request req = generate_bearer_context_setup_request(ue_index);

  // Start procedure in CU-CP.
  async_task<e1ap_bearer_context_setup_response>         t = e1ap->handle_bearer_context_setup_request(req);
  lazy_task_launcher<e1ap_bearer_context_setup_response> t_launcher(t);

  ASSERT_FALSE(t.ready());

  test_ues.emplace(ue_index);
  test_ue& u         = test_ues[ue_index];
  u.ue_index         = ue_index;
  u.cu_cp_ue_e1ap_id = int_to_gnb_cu_cp_ue_e1ap_id(this->e1_pdu_notifier.last_e1_msg.pdu.init_msg()
                                                       .value.bearer_context_setup_request()
                                                       ->gnb_cu_cp_ue_e1ap_id.value);
  u.cu_up_ue_e1ap_id = cu_up_ue_e1ap_id;

  // Handle response from CU-UP.
  e1_message bearer_context_setup_response =
      generate_bearer_context_setup_response(u.cu_cp_ue_e1ap_id.value(), cu_up_ue_e1ap_id);
  test_logger.info("Injecting BearerContextSetupResponse");
  e1ap->handle_message(bearer_context_setup_response);

  srsgnb_assert(t.ready(), "The procedure should have completed by now");
}
