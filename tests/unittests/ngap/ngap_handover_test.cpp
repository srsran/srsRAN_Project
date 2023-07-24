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
#include "srsran/ngap/ngap_handover.h"
#include "srsran/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Test successful handover preparation procedure
TEST_F(ngap_test, when_source_gnb_handover_preperation_triggered_then_ho_command_received)
{
  // Action 1: Launch HO preparation procedure
  test_logger.info("Launch source NGAP handover preparation procedure");
  ngap_handover_preparation_request                      request = {};
  async_task<ngap_handover_preparation_response>         t       = ngap->handle_handover_preparation_request(request);
  lazy_task_launcher<ngap_handover_preparation_response> t_launcher(t);
}
