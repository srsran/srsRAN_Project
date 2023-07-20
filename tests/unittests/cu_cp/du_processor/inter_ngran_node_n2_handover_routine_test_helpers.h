/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
