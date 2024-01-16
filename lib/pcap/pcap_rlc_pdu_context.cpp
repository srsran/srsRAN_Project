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

#include "srsran/pcap/rlc_pcap.h"

using namespace srsran;

pcap_rlc_pdu_context::pcap_rlc_pdu_context(du_ue_index_t ue_index, rb_id_t rb_id, const rlc_tx_am_config& cfg)
{
  rlc_mode  = PCAP_RLC_AM_MODE;
  direction = PCAP_RLC_DIRECTION_DOWNLINK;
  set_sequence_number_length(cfg.sn_field_length);
  set_bearer_info(rb_id);
  ueid = ue_index + 1;
}

pcap_rlc_pdu_context::pcap_rlc_pdu_context(du_ue_index_t ue_index, rb_id_t rb_id, const rlc_rx_am_config& cfg)
{
  rlc_mode  = PCAP_RLC_AM_MODE;
  direction = PCAP_RLC_DIRECTION_UPLINK;
  set_sequence_number_length(cfg.sn_field_length);
  set_bearer_info(rb_id);
  ueid = ue_index + 1;
}

pcap_rlc_pdu_context::pcap_rlc_pdu_context(du_ue_index_t ue_index, rb_id_t rb_id, const rlc_tx_um_config& cfg)
{
  rlc_mode  = PCAP_RLC_UM_MODE;
  direction = PCAP_RLC_DIRECTION_DOWNLINK;
  set_sequence_number_length(cfg.sn_field_length);
  set_bearer_info(rb_id);
  ueid = ue_index + 1;
}

pcap_rlc_pdu_context::pcap_rlc_pdu_context(du_ue_index_t ue_index, rb_id_t rb_id, const rlc_rx_um_config& cfg)
{
  rlc_mode  = PCAP_RLC_UM_MODE;
  direction = PCAP_RLC_DIRECTION_UPLINK;
  set_sequence_number_length(cfg.sn_field_length);
  set_bearer_info(rb_id);
  ueid = ue_index + 1;
}

pcap_rlc_pdu_context::pcap_rlc_pdu_context(du_ue_index_t ue_index, rb_id_t rb_id, bool is_uplink)
{
  rlc_mode               = PCAP_RLC_TM_MODE;
  direction              = is_uplink ? PCAP_RLC_DIRECTION_UPLINK : PCAP_RLC_DIRECTION_DOWNLINK;
  sequence_number_length = PCAP_RLC_TM_SN_LENGTH_0_BITS;
  set_bearer_info(rb_id);
  ueid = ue_index + 1;
}

void pcap_rlc_pdu_context::set_sequence_number_length(rlc_am_sn_size sn_field_length)
{
  switch (sn_field_length) {
    case rlc_am_sn_size::size12bits:
      sequence_number_length = PCAP_RLC_AM_SN_LENGTH_12_BITS;
      break;
    case rlc_am_sn_size::size18bits:
      sequence_number_length = PCAP_RLC_AM_SN_LENGTH_18_BITS;
      break;
  }
}

void pcap_rlc_pdu_context::set_sequence_number_length(rlc_um_sn_size sn_field_length)
{
  switch (sn_field_length) {
    case rlc_um_sn_size::size6bits:
      sequence_number_length = PCAP_RLC_UM_SN_LENGTH_6_BITS;
      break;
    case rlc_um_sn_size::size12bits:
      sequence_number_length = PCAP_RLC_UM_SN_LENGTH_12_BITS;
      break;
  }
}

void pcap_rlc_pdu_context::set_bearer_info(rb_id_t rb_id)
{
  if (rb_id.is_drb()) {
    bearer_type = PCAP_RLC_BEARER_TYPE_DRB;
    bearer_id   = drb_id_to_uint(rb_id.get_drb_id());
  } else if (rb_id.is_srb()) {
    switch (rb_id.get_srb_id()) {
      case srb_id_t::srb0:
        bearer_type = PCAP_RLC_BEARER_TYPE_CCCH;
        break;
      default:
        bearer_type = PCAP_RLC_BEARER_TYPE_SRB;
    }
    bearer_id = srb_id_to_uint(rb_id.get_srb_id());
  }
}
