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

#include "srsran/adt/span.h"
#include "srsran/du/du_test_config.h"
#include "srsran/du_high/du_high_executor_mapper.h"
#include "srsran/mac/mac.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/pcap/pcap.h"

namespace srsran {

struct mac_control_config {
  srslog::basic_logger&         logger;
  mac_ul_ccch_notifier&         event_notifier;
  du_high_ue_executor_mapper&   ue_exec_mapper;
  du_high_cell_executor_mapper& cell_exec_mapper;
  task_executor&                ctrl_exec;

  mac_control_config(mac_ul_ccch_notifier&         event_notifier_,
                     du_high_ue_executor_mapper&   ul_exec_,
                     du_high_cell_executor_mapper& dl_exec_,
                     task_executor&                ctrl_exec_) :
    logger(srslog::fetch_basic_logger("MAC", true)),
    event_notifier(event_notifier_),
    ue_exec_mapper(ul_exec_),
    cell_exec_mapper(dl_exec_),
    ctrl_exec(ctrl_exec_)
  {
  }
};

} // namespace srsran
