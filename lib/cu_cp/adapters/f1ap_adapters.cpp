/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_adapters.h"
#include "../cu_cp_controller/cu_cp_controller.h"

using namespace srsran;
using namespace srs_cu_cp;

void f1ap_cu_cp_adapter::on_du_remove_request_received(du_index_t du_index)
{
  srsran_assert(controller != nullptr, "adapter is disconnected");
  controller->get_f1c_handler().handle_du_remove_request(du_index);
}
