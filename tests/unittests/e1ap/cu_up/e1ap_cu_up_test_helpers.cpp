/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1ap_cu_up_test_helpers.h"

using namespace srsran;
using namespace srs_cu_up;

e1ap_cu_up_test::e1ap_cu_up_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  e1ap_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  e1ap = create_e1ap(msg_notifier, cu_up_notifier, cu_up_worker);
}

e1ap_cu_up_test::~e1ap_cu_up_test()
{
  // flush logger after each test
  srslog::flush();
}

void e1ap_cu_up_test::setup_bearer(unsigned int cu_cp_ue_e1ap_id)
{
  // Generate BearerContextSetupRequest
  e1ap_message bearer_context_setup = generate_bearer_context_setup_request(cu_cp_ue_e1ap_id);

  e1ap->handle_message(bearer_context_setup);
}
