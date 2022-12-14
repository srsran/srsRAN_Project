/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_bearer.h"

using namespace srsgnb;
using namespace srs_du;

void du_bearer_adapter::connect(du_ue_index_t                       ue_index,
                                srb_id_t                            srb_id,
                                f1c_bearer&                         f1_bearer,
                                rlc_entity&                         rlc_bearer,
                                mac_ue_control_information_handler& mac_ue_info_handler)
{
  // > Connect RLC Rx SDU -> F1-C Tx SDU handler.
  rlc_rx_f1c_sdu_notif.connect(f1_bearer);

  // > Connect F1-C Rx SDU -> RLC Tx SDU.
  f1c_rx_sdu_notif.connect(*rlc_bearer.get_tx_upper_layer_data_interface());

  // > Connect RLC BSR update notifier -> MAC Control Info Handler.
  rlc_tx_buffer_state_notif.connect(ue_index, srb_id_to_lcid(srb_id), mac_ue_info_handler);

  // > Connect MAC Rx SDU notifier -> RLC Rx PDU.
  mac_rx_notif.connect(*rlc_bearer.get_rx_lower_layer_interface());

  // > Connect MAC Tx SDU builder -> RLC Tx PDU builder.
  mac_tx_notif.connect(*rlc_bearer.get_tx_lower_layer_interface());
}

void du_bearer_adapter::connect(du_ue_index_t                       ue_index,
                                drb_id_t                            drb_id,
                                lcid_t                              lcid,
                                f1u_bearer&                         f1_bearer,
                                rlc_entity&                         rlc_bearer,
                                mac_ue_control_information_handler& mac_ue_info_handler)
{
  // > Connect RLC Rx SDU -> F1-U Tx SDU handler.
  rlc_rx_f1u_sdu_notif.connect(f1_bearer.get_tx_sdu_handler());

  // > Connect F1-C Rx SDU -> RLC Tx SDU.
  f1u_rx_sdu_notif.connect(*rlc_bearer.get_tx_upper_layer_data_interface());

  // > Connect RLC BSR update notifier -> MAC Control Info Handler.
  rlc_tx_buffer_state_notif.connect(ue_index, lcid, mac_ue_info_handler);

  // > Connect MAC Rx SDU notifier -> RLC Rx PDU.
  mac_rx_notif.connect(*rlc_bearer.get_rx_lower_layer_interface());

  // > Connect MAC Tx SDU builder -> RLC Tx PDU builder.
  mac_tx_notif.connect(*rlc_bearer.get_tx_lower_layer_interface());
}
