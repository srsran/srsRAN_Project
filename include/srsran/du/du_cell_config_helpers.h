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

#include "du_cell_config.h"
#include "srsran/du/du_qos_config.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/five_qi.h"
#include "srsran/ran/nr_cgi_helpers.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"
#include "srsran/scheduler/config/cell_config_builder_params.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include <map>

// TODO: This file is temporary. Eventually we will receive cell configurations from the DU config file.

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
inline du_cell_config make_default_du_cell_config(const cell_config_builder_params_extended& params = {})
{
  du_cell_config cfg{};
  cfg.pci         = params.pci;
  cfg.tac         = 1;
  cfg.nr_cgi.plmn = "00101";
  cfg.nr_cgi.nci  = config_helpers::make_nr_cell_identity(411, 32, 1);

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
      cfg.dl_carrier.band, params.scs_common, params.scs_common, *params.coreset0_index, params.k_ssb->value());
  cfg.dmrs_typeA_pos = coreset0_desc.nof_symb_coreset == 3U ? dmrs_typeA_position::pos3 : dmrs_typeA_position::pos2;

  cfg.tdd_ul_dl_cfg_common = params.tdd_ul_dl_cfg_common;
  cfg.ue_ded_serv_cell_cfg = create_default_initial_ue_serving_cell_config(params);

  return cfg;
}

/// Generates default QoS configuration used by gNB DU. The default configuration should be valid.
/// Dependencies between RLC timers should be considered:
///   * t-Reassembly: How long it takes for the RLC to detect a lost PDU. If larger than the MAC SR, we may drop a
///                   PDU prematurely in the case UM, or we may send NACKs prematurely for the case of AM.
///
///   * t-StatusProhibit: This value dictates how often the RLC is allowed to send status reports. If this value is
///                       shorter than the MAC's SR, it may take longer than t-StatusProhibit to send a control PDU.
///
///   * t-PollRetransmission: This value should be slightly larger than t-StatusProhibit and also account for RTT.
///                           Moreover this value should be slightly larger than the SR of the MAC
///                           to avoid spurious RETX'es from late status reports. See t-StatusProhibit for details.
///
/// Note: These three timers will have implications in picking the PDCP's t-Reordering. See the generation of
///       t-Reordering default configuration for details.
///
/// Dependencies between F1-U timers should be considered:
///   * t-Notify: This value determines the maximum backoff time to aggregate notifications (towards CU_UP) of which
///               PDCP SDUs have been transmitted (RLC UM/AM) or delivered (RLC AM). Small values increase the number of
///               F1-U messages. Large values may trigger unnecessary discard notifications due to expiration of the
///               PDCP discard timer.
inline std::map<five_qi_t, du_qos_config> make_default_du_qos_config_list(int rlc_metrics_report)
{
  std::map<five_qi_t, du_qos_config> qos_list = {};
  {
    // 5QI=7
    du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                  = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.sn_field_length = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly    = 100;
    cfg.rlc.um.tx.queue_size      = 4096;
    cfg.rlc.metrics_period        = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify = 10;

    qos_list[uint_to_five_qi(7)] = cfg;
  }
  {
    // 5QI=9
    du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                    = rlc_mode::am;
    cfg.rlc.am.tx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.tx.t_poll_retx       = 20;
    cfg.rlc.am.tx.poll_pdu          = 16;
    cfg.rlc.am.tx.poll_byte         = -1;
    cfg.rlc.am.tx.max_retx_thresh   = 32;
    cfg.rlc.am.tx.max_window        = 0;
    cfg.rlc.am.tx.queue_size        = 4096;
    cfg.rlc.am.rx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.rx.t_reassembly      = 20;
    cfg.rlc.am.rx.t_status_prohibit = 10;
    cfg.rlc.metrics_period          = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify = 10;

    qos_list[uint_to_five_qi(9)] = cfg;
  }
  return qos_list;
}

} // namespace config_helpers
} // namespace srsran
