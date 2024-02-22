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

#include "mac_impl.h"
#include "mac_sched/srsran_scheduler_adapter.h"

namespace srsran {

mac_impl::mac_impl(const mac_config& params) :
  rnti_table(params.mac_cfg.initial_crnti),
  mac_sched(std::make_unique<srsran_scheduler_adapter>(params, rnti_table)),
  dl_unit(
      mac_dl_config{params.ue_exec_mapper, params.cell_exec_mapper, params.ctrl_exec, params.phy_notifier, params.pcap},
      *mac_sched,
      rnti_table),
  ul_unit(mac_ul_config{params.ctrl_exec,
                        params.ue_exec_mapper,
                        params.ul_ccch_notifier,
                        *mac_sched,
                        rnti_table,
                        params.pcap}),
  ctrl_unit(
      mac_control_config{params.ul_ccch_notifier, params.ue_exec_mapper, params.cell_exec_mapper, params.ctrl_exec},
      ul_unit,
      dl_unit,
      rnti_table,
      *mac_sched)
{
}

} // namespace srsran
