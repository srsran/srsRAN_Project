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

#pragma once

#include "../du_ue/du_ue_manager.h"
#include "procedure_logger.h"
#include "srsran/du_manager/du_manager_params.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

/// \brief Procedure used to interrupt all the DU activity.
class du_stop_procedure
{
public:
  explicit du_stop_procedure(du_ue_manager& ue_mng);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  du_ue_manager& ue_mng;

  du_procedure_logger proc_logger;
};

} // namespace srs_du
} // namespace srsran