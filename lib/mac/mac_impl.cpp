/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_impl.h"
#include "mac_sched/srsran_scheduler_adapter.h"

namespace srsran {

mac_impl::mac_impl(const mac_config& params) :
  rlf_handler(params.mac_cfg.max_consecutive_dl_kos, params.mac_cfg.max_consecutive_ul_kos),
  mac_sched(std::make_unique<srsran_scheduler_adapter>(params, rnti_table, rlf_handler)),
  dl_unit(mac_dl_config{params.ue_exec_mapper,
                        params.cell_exec_mapper,
                        params.ctrl_exec,
                        params.phy_notifier,
                        params.pcap,
                        rlf_handler},
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
