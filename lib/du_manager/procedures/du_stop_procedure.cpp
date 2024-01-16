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

#include "du_stop_procedure.h"
#include "srsran/support/async/async_timer.h"
#include "srsran/support/async/execute_on.h"

using namespace srsran;
using namespace srs_du;

du_stop_procedure::du_stop_procedure(du_ue_manager& ue_mng_) :
  ue_mng(ue_mng_), proc_logger(srslog::fetch_basic_logger("DU-MNG"), "DU Stop")
{
}

void du_stop_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  // TODO: Call F1 Removal Request if not yet called.

  // Forcefully interrupt all UE activity.
  CORO_AWAIT(ue_mng.stop());

  proc_logger.log_proc_completed();

  CORO_RETURN();
}
