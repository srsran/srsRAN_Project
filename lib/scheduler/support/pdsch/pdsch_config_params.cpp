/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "pdsch_config_params.h"
#include "../dmrs_helpers.h"

using namespace srsran;
using namespace sched_helper;

pdsch_config_params
sched_helper::get_pdsch_config_f1_0_tc_rnti(const cell_configuration&                    cell_cfg_common,
                                            const pdsch_time_domain_resource_allocation& pdsch_td_cfg)
{
  static constexpr pdsch_mcs_table mcs_table = pdsch_mcs_table::qam64;
  // As per TS 38.214, Section 5.1.3.2, TB scaling field can be different to 0 only for DCI 1_0 with P-RNTI, or RA-RNTI.
  static constexpr unsigned tb_scaling_field = 0;
  // As per TS 38.214, Section 5.1.3.2, nof_oh_prb by \c xOverhead, defined in \c PDSCH-ServingCellConfig, TS 38.331; it
  // is in the dedicated resources, configured the after DCI Format 1-0 TC-RNTI is used. Hence, nof_oh_prb is here set
  // as 0.
  static constexpr unsigned nof_oh_prb = 0;
  static constexpr unsigned nof_layers = 1;

  pdsch_config_params pdsch;

  pdsch.dmrs = make_dmrs_info_common(pdsch_td_cfg, cell_cfg_common.pci, cell_cfg_common.dmrs_typeA_pos);

  pdsch.nof_oh_prb                   = nof_oh_prb;
  pdsch.symbols                      = pdsch_td_cfg.symbols;
  pdsch.mcs_table                    = mcs_table;
  pdsch.tb_scaling_field             = tb_scaling_field;
  pdsch.nof_layers                   = nof_layers;
  pdsch.max_nof_cws_scheduled_by_dci = 1;

  return pdsch;
}

pdsch_config_params
sched_helper::get_pdsch_config_f1_0_c_rnti(const cell_configuration&                    cell_cfg_common,
                                           const pdsch_time_domain_resource_allocation& pdsch_td_cfg)
{
  // As per TS 38.214, Section 5.1.3.2, TB scaling filed can be different to 0 only for DCI 1_0 with P-RNTI, or RA-RNTI.
  static constexpr unsigned        tb_scaling_field = 0;
  static constexpr unsigned        nof_layers       = 1;
  static constexpr pdsch_mcs_table mcs_table        = pdsch_mcs_table::qam64;

  pdsch_config_params pdsch;

  pdsch.dmrs = make_dmrs_info_common(pdsch_td_cfg, cell_cfg_common.pci, cell_cfg_common.dmrs_typeA_pos);
  // NOTE: x_overhead::not_set is mapped to 0.
  pdsch.nof_oh_prb                   = static_cast<unsigned>(0);
  pdsch.symbols                      = pdsch_td_cfg.symbols;
  pdsch.mcs_table                    = mcs_table;
  pdsch.tb_scaling_field             = tb_scaling_field;
  pdsch.nof_layers                   = nof_layers;
  pdsch.max_nof_cws_scheduled_by_dci = 1;

  return pdsch;
}

pdsch_config_params
sched_helper::get_pdsch_config_f1_0_c_rnti(const cell_configuration&                    cell_cfg_common,
                                           const pdsch_serving_cell_config*             pdsch_serv_cfg,
                                           const pdsch_time_domain_resource_allocation& pdsch_td_cfg)
{
  pdsch_config_params params = get_pdsch_config_f1_0_c_rnti(cell_cfg_common, pdsch_td_cfg);
  if (pdsch_serv_cfg != nullptr) {
    // According to TS 38.214, Section 5.1.3.2, nof_oh_prb is set equal to xOverhead, when set; else nof_oh_prb = 0.
    params.nof_oh_prb = static_cast<unsigned>(pdsch_serv_cfg->x_ov_head);
  }
  return params;
}

pdsch_config_params
sched_helper::get_pdsch_config_f1_1_c_rnti(const cell_configuration&                    cell_cfg_common,
                                           const pdsch_config&                          pdsch_cfg,
                                           const pdsch_serving_cell_config*             pdsch_serv_cfg,
                                           const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                           unsigned                                     nof_layers)
{
  // As per TS 38.214, Section 5.1.3.2, TB scaling filed can be different to 0 only for DCI 1_0 with P-RNTI, or RA-RNTI.
  static constexpr unsigned tb_scaling_field = 0;

  // TODO: Update the value based on nof. CWs enabled.
  static const bool are_both_cws_enabled = false;

  pdsch_config_params pdsch;

  pdsch.dmrs = make_dmrs_info_dedicated(pdsch_td_cfg,
                                        cell_cfg_common.pci,
                                        cell_cfg_common.dmrs_typeA_pos,
                                        pdsch_cfg.pdsch_mapping_type_a_dmrs.value(),
                                        nof_layers,
                                        cell_cfg_common.dl_carrier.nof_ant,
                                        are_both_cws_enabled);

  // According to TS 38.214, Section 5.1.3.2, nof_oh_prb is set equal to xOverhead, when set; else nof_oh_prb = 0.
  // NOTE: x_overhead::not_set is mapped to 0.
  pdsch.nof_oh_prb                   = pdsch_serv_cfg != nullptr ? static_cast<unsigned>(pdsch_serv_cfg->x_ov_head)
                                                                 : static_cast<unsigned>(x_overhead::not_set);
  pdsch.symbols                      = pdsch_td_cfg.symbols;
  pdsch.mcs_table                    = pdsch_cfg.mcs_table;
  pdsch.tb_scaling_field             = tb_scaling_field;
  pdsch.nof_layers                   = nof_layers;
  pdsch.max_nof_cws_scheduled_by_dci = pdsch_cfg.is_max_cw_sched_by_dci_is_two ? 2 : 1;

  return pdsch;
}
