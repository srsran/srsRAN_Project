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
