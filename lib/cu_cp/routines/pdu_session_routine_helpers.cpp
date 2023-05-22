/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdu_session_routine_helpers.h"

void srsran::srs_cu_cp::fill_e1ap_drb_pdcp_config(e1ap_pdcp_config& e1ap_pdcp_cfg, const pdcp_config& cu_cp_pdcp_cfg)
{
  e1ap_pdcp_cfg.pdcp_sn_size_ul = cu_cp_pdcp_cfg.tx.sn_size;
  e1ap_pdcp_cfg.pdcp_sn_size_dl = cu_cp_pdcp_cfg.rx.sn_size;
  e1ap_pdcp_cfg.rlc_mod         = cu_cp_pdcp_cfg.rlc_mode;
  if (cu_cp_pdcp_cfg.tx.discard_timer != pdcp_discard_timer::not_configured) {
    e1ap_pdcp_cfg.discard_timer = cu_cp_pdcp_cfg.tx.discard_timer;
  }
  if (cu_cp_pdcp_cfg.rx.t_reordering != pdcp_t_reordering::infinity) {
    e1ap_pdcp_cfg.t_reordering_timer = cu_cp_pdcp_cfg.rx.t_reordering;
  }
}

void srsran::srs_cu_cp::fill_e1ap_qos_flow_param_item(e1ap_qos_flow_qos_param_item&      e1ap_qos_item,
                                                      srslog::basic_logger&              logger,
                                                      const qos_flow_setup_request_item& request_item)
{
  e1ap_qos_item.qos_flow_id = request_item.qos_flow_id;

  if (request_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.has_value()) {
    non_dyn_5qi_descriptor_t non_dyn_5qi;
    non_dyn_5qi.five_qi = request_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.value().five_qi;

    // TODO: Add optional values

    e1ap_qos_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi = non_dyn_5qi;
  } else {
    // logger.warning("qos_flow_id={}: dynamic 5QI not fully supported.", e1ap_qos_item.qos_flow_id);
    //  TODO: Add dynamic 5qi
  }

  e1ap_qos_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.prio_level =
      request_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp;
  e1ap_qos_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_cap =
      request_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap;
  e1ap_qos_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_vulnerability =
      request_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability;
}