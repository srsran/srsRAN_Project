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

#include "srsran/du/du_cell_config.h"
#include "srsran/du/du_high/du_qos_config.h"
#include "srsran/mac/config/mac_config_helpers.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsran/ran/qos/five_qi.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"
#include "srsran/scheduler/config/cell_config_builder_params.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include <map>

namespace srsran {
namespace config_helpers {

inline scheduler_expert_config make_default_scheduler_expert_config()
{
  scheduler_expert_config cfg;
  cfg.ra.max_nof_msg3_harq_retxs = 4;
  cfg.ra.msg3_mcs_index          = 0;
  cfg.ra.rar_mcs_index           = 0;

  // As per TS 38.214, Section 5.1.3.1, only an MCS with modulation order 2 allowed for SIB1.
  cfg.si.sib1_mcs_index    = 5;
  cfg.si.sib1_retx_period  = sib1_rtx_periodicity::ms160;
  cfg.si.sib1_dci_aggr_lev = aggregation_level::n4;

  // As per TS 38.214, Section 5.1.3.1, only an MCS with modulation order 2 allowed for Paging.
  cfg.pg.paging_mcs_index    = 5;
  cfg.pg.paging_dci_aggr_lev = aggregation_level::n4;
  cfg.pg.max_paging_retries  = 2;

  cfg.ue.dl_mcs             = {0, 28};
  cfg.ue.initial_cqi        = 3;
  cfg.ue.pdsch_rv_sequence  = {0};
  cfg.ue.ul_mcs             = {0, 28};
  cfg.ue.pusch_rv_sequence  = {0};
  cfg.ue.max_nof_harq_retxs = 4;
  // The UE is not expected to decode a PDSCH scheduled with P-RNTI, RA-RNTI, SI-RNTI and Qm > 2 i.e. MCS index of 9 in
  // Table 5.1.3.1-1 of TS 38.214.
  // Note: A MCS index of 7 can handle Msg4 of size 458 bytes for PDSCH occupying symbols (2,..,14].
  // Note: A MCS index of 8 can handle Msg4 of size 325 bytes for PDSCH occupying symbols (3,..,14].
  cfg.ue.max_msg4_mcs                            = 9;
  cfg.ue.initial_ul_sinr                         = 5;
  cfg.ue.enable_csi_rs_pdsch_multiplexing        = true;
  cfg.ue.ta_measurement_slot_period              = 80;
  cfg.ue.ta_cmd_offset_threshold                 = 1;
  cfg.ue.ta_update_measurement_ul_sinr_threshold = 0.0F;

  cfg.log_broadcast_messages = true;
  cfg.metrics_report_period  = std::chrono::milliseconds{1000};

  return cfg;
}

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline srs_du::du_cell_config make_default_du_cell_config(const cell_config_builder_params_extended& params = {})
{
  srs_du::du_cell_config cfg{};
  cfg.pci            = params.pci;
  cfg.tac            = 1;
  cfg.nr_cgi.plmn_id = plmn_identity::test_value();
  cfg.nr_cgi.nci     = nr_cell_identity::create({411, 22}, 1).value();

  cfg.dl_carrier              = make_default_dl_carrier_configuration(params);
  cfg.ul_carrier              = make_default_ul_carrier_configuration(params);
  cfg.coreset0_idx            = *params.coreset0_index;
  cfg.searchspace0_idx        = params.search_space0_index;
  cfg.dl_cfg_common           = make_default_dl_config_common(params);
  cfg.ul_cfg_common           = make_default_ul_config_common(params);
  cfg.scs_common              = params.scs_common;
  cfg.ssb_cfg                 = make_default_ssb_config(params);
  cfg.cell_barred             = false;
  cfg.intra_freq_resel        = false;
  cfg.ue_timers_and_constants = make_default_ue_timers_and_constants_config();

  // The CORESET duration of 3 symbols is only permitted if dmrs-typeA-Position is set to 3. Refer TS 38.211, 7.3.2.2.
  const pdcch_type0_css_coreset_description coreset0_desc = pdcch_type0_css_coreset_get(
      cfg.dl_carrier.band, params.ssb_scs, params.scs_common, *params.coreset0_index, params.k_ssb->value());
  cfg.dmrs_typeA_pos = coreset0_desc.nof_symb_coreset == 3U ? dmrs_typeA_position::pos3 : dmrs_typeA_position::pos2;

  cfg.tdd_ul_dl_cfg_common = params.tdd_ul_dl_cfg_common;
  cfg.ue_ded_serv_cell_cfg = create_default_initial_ue_serving_cell_config(params);

  return cfg;
}

} // namespace config_helpers
} // namespace srsran
