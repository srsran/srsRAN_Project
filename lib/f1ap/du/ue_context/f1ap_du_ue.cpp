/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "f1ap_du_ue.h"
#include "du/procedures/f1ap_du_ue_context_modification_procedure.h"
#include "du/procedures/f1ap_du_ue_context_setup_procedure.h"
#include "srsran/f1ap/du/f1ap_du_ue_config.h"

using namespace srsran;
using namespace srsran::srs_du;

void f1ap_du_ue::handle_ue_context_modification_request(const asn1::f1ap::ue_context_mod_request_s& msg)
{
  du_handler.get_ue_handler(context.ue_index)
      .schedule_async_task(launch_async<f1ap_du_ue_context_modification_procedure>(msg, *this));
}
