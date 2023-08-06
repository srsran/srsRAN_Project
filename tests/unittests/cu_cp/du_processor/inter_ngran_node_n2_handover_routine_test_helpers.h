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

#pragma once

#include "du_processor_routine_manager_test_helpers.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

class inter_ngran_node_n2_handover_routine_test : public du_processor_routine_manager_test
{
protected:
  void start_procedure(const cu_cp_inter_ngran_node_n2_handover_request& msg)
  {
    t = routine_mng->start_inter_ngran_node_n2_handover_routine(msg, ngap_control_notifier);
    t_launcher.emplace(t);
  }

  async_task<cu_cp_inter_ngran_node_n2_handover_response>                   t;
  optional<lazy_task_launcher<cu_cp_inter_ngran_node_n2_handover_response>> t_launcher;
};

} // namespace srs_cu_cp
} // namespace srsran
