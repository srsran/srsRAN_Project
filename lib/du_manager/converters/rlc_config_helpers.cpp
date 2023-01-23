/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rlc_config_helpers.h"

using namespace srsgnb;
using namespace srs_du;

rlc_mode srsgnb::srs_du::convert_asn1_f1ap_to_rlc_mode(asn1::f1ap::rlc_mode_e mode)
{
  switch (mode.value) {
    case asn1::f1ap::rlc_mode_opts::rlc_am:
      return rlc_mode::am;
      break;
    case asn1::f1ap::rlc_mode_opts::rlc_um_bidirectional:
      return rlc_mode::um_bidir;
      break;
    case asn1::f1ap::rlc_mode_opts::rlc_um_unidirectional_dl:
      return rlc_mode::um_unidir_dl;
      break;
    case asn1::f1ap::rlc_mode_opts::rlc_um_unidirectional_ul:
      return rlc_mode::um_unidir_ul;
      break;
    default:
      report_fatal_error("Invalid RLC mode {}", mode.value);
      break;
  }
  return {};
}

rlc_mode srsgnb::srs_du::convert_asn1_f1ap_to_rlc_mode(drb_rlc_mode mode)
{
  switch (mode) {
    case drb_rlc_mode::am:
      return rlc_mode::am;
    case drb_rlc_mode::um_bidir:
      return rlc_mode::um_bidir;
    case drb_rlc_mode::um_unidir_dl:
      return rlc_mode::um_unidir_dl;
    case drb_rlc_mode::um_unidir_ul:
      return rlc_mode::um_unidir_ul;
    default:
      report_fatal_error("Invalid RLC mode {}", mode);
      break;
  }
  return {};
}

rlc_config srsgnb::srs_du::make_default_srb_rlc_config()
{
  rlc_config cfg;
  cfg.mode                    = rlc_mode::am;
  cfg.am.tx.sn_field_length   = rlc_am_sn_size::size12bits;
  cfg.am.tx.t_poll_retx       = 45;
  cfg.am.tx.poll_pdu          = -1;
  cfg.am.tx.poll_byte         = -1;
  cfg.am.tx.max_retx_thresh   = 8;
  cfg.am.rx.sn_field_length   = rlc_am_sn_size::size12bits;
  cfg.am.rx.t_reassembly      = 35;
  cfg.am.rx.t_status_prohibit = 0;
  return cfg;
}

rlc_config srsgnb::srs_du::create_rlc_config(const f1ap_drb_to_setup& f1_drb_cfg)
{
  rlc_config cfg;
  cfg.mode = convert_asn1_f1ap_to_rlc_mode(f1_drb_cfg.mode);
  // TODO: Actual use of F1 parameters.
  if (cfg.mode == rlc_mode::am) {
    cfg.am.tx.sn_field_length   = rlc_am_sn_size::size12bits;
    cfg.am.tx.t_poll_retx       = 45;
    cfg.am.tx.poll_pdu          = -1;
    cfg.am.tx.poll_byte         = -1;
    cfg.am.tx.max_retx_thresh   = 8;
    cfg.am.rx.sn_field_length   = rlc_am_sn_size::size12bits;
    cfg.am.rx.t_reassembly      = 35;
    cfg.am.rx.t_status_prohibit = 0;
  } else {
    cfg.um.tx.sn_field_length = rlc_um_sn_size::size12bits;
    cfg.um.rx.sn_field_length = rlc_um_sn_size::size12bits;
    cfg.um.rx.t_reassembly    = 35;
  }
  return cfg;
}

template <typename Bearer>
static void fill_rlc_entity_creation_message_common(rlc_entity_creation_message&             msg,
                                                    du_ue_index_t                            ue_index,
                                                    du_cell_index_t                          pcell_index,
                                                    Bearer&                                  bearer,
                                                    const du_manager_params::service_params& du_services)
{
  msg.ue_index       = ue_index;
  msg.config         = bearer.rlc_cfg;
  msg.rx_upper_dn    = &bearer.connector.rlc_rx_sdu_notif;
  msg.tx_upper_dn    = &bearer.connector.rlc_tx_data_notif;
  msg.tx_upper_cn    = &bearer.connector.rlc_tx_ctrl_notif;
  msg.tx_lower_dn    = &bearer.connector.rlc_tx_buffer_state_notif;
  msg.timers         = &du_services.timers;
  msg.pcell_executor = &du_services.cell_execs.executor(pcell_index);
  msg.ue_executor    = &du_services.ue_execs.executor(ue_index);
}

// for SRBs
rlc_entity_creation_message
srsgnb::srs_du::make_rlc_entity_creation_message(du_ue_index_t                            ue_index,
                                                 du_cell_index_t                          pcell_index,
                                                 du_ue_srb&                               bearer,
                                                 const du_manager_params::service_params& du_services)
{
  rlc_entity_creation_message msg;
  fill_rlc_entity_creation_message_common(msg, ue_index, pcell_index, bearer, du_services);
  msg.rb_id = bearer.srb_id;
  return msg;
}

// for DRBs
rlc_entity_creation_message
srsgnb::srs_du::make_rlc_entity_creation_message(du_ue_index_t                            ue_index,
                                                 du_cell_index_t                          pcell_index,
                                                 du_ue_drb&                               bearer,
                                                 const du_manager_params::service_params& du_services)
{
  rlc_entity_creation_message msg;
  fill_rlc_entity_creation_message_common(msg, ue_index, pcell_index, bearer, du_services);
  msg.rb_id = bearer.drb_id;
  return msg;
}
