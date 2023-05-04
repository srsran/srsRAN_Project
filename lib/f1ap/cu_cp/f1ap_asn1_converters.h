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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/up_transport_layer_info.h"
#include <string>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

/// \brief Convert F1AP Cause to \c cause_t type.
/// \param f1ap_cause The F1AP Cause.
/// \return The cause_t type.
inline cause_t f1ap_cause_to_cause(asn1::f1ap::cause_c f1ap_cause)
{
  cause_t cause;

  switch (f1ap_cause.type()) {
    case asn1::f1ap::cause_c::types_opts::radio_network:
      cause = cause_t::radio_network;
      return cause;
      break;
    case asn1::f1ap::cause_c::types_opts::transport:
      cause = cause_t::transport;
      return cause;
      break;
    case asn1::f1ap::cause_c::types_opts::protocol:
      cause = cause_t::protocol;
      return cause;
      break;
    case asn1::f1ap::cause_c::types_opts::misc:
      cause = cause_t::misc;
      return cause;
      break;
    default:
      cause = cause_t::nulltype;
      return cause;
  }
}

/// \brief Convert \c cause_t type to F1AP cause.
/// \param cause The cause_t type.
/// \return The F1AP cause.
inline asn1::f1ap::cause_c cause_to_f1ap_cause(cause_t cause)
{
  asn1::f1ap::cause_c f1ap_cause;

  switch (cause) {
    case cause_t::radio_network:
      f1ap_cause.set(asn1::f1ap::cause_c::types_opts::radio_network);
      break;
    case cause_t::transport:
      f1ap_cause.set(asn1::f1ap::cause_c::types_opts::transport);
      break;
    case cause_t::nas:
      // NAS doesn't exist as F1AP cause - use radio-network instead.
      f1ap_cause.set(asn1::f1ap::cause_c::types_opts::radio_network);
      break;
    case cause_t::protocol:
      f1ap_cause.set(asn1::f1ap::cause_c::types_opts::protocol);
      break;
    case cause_t::misc:
      f1ap_cause.set(asn1::f1ap::cause_c::types_opts::misc);
      break;
    default:
      report_fatal_error("Cannot convert cause {} to F1AP type", cause);
  }

  return f1ap_cause;
}

/// \brief Convert F1AP NRCGI to NR Cell Identity.
/// \param f1ap_cgi The F1AP NRCGI.
/// \return The NR Cell Identity.
inline nr_cell_id_t f1ap_nrcgi_to_nr_cell_identity(asn1::f1ap::nr_cgi_s& f1ap_cgi)
{
  return f1ap_cgi.nr_cell_id.to_number();
}

/// \brief Convert \c rlc_mode to F1AP ASN.1.
/// \param rlc_mod The common type RLC mode.
/// \return The ASN.1 RLC mode.
inline asn1::f1ap::rlc_mode_e rlc_mode_to_f1ap_asn1(const srsran::rlc_mode& rlc_mod)
{
  asn1::f1ap::rlc_mode_e asn1_rlc_mode;

  switch (rlc_mod) {
    case rlc_mode::am:
      asn1_rlc_mode.value = asn1::f1ap::rlc_mode_opts::rlc_am;
      break;
    case rlc_mode::um_bidir:
      asn1_rlc_mode.value = asn1::f1ap::rlc_mode_opts::rlc_um_bidirectional;
      break;
    case rlc_mode::um_unidir_dl:
      asn1_rlc_mode.value = asn1::f1ap::rlc_mode_opts::rlc_um_unidirectional_dl;
      break;
    case rlc_mode::um_unidir_ul:
      asn1_rlc_mode.value = asn1::f1ap::rlc_mode_opts::rlc_um_unidirectional_ul;
      break;
    case rlc_mode::tm:
      // TM not supported for DRBs
      report_fatal_error("Invalid RLC mode {}", rlc_mod);
      break;
  }

  return asn1_rlc_mode;
}

/// \brief Convert \c qos_characteristics_t to F1AP ASN.1.
/// \param qos_characteristics The common type QoS characteristics.
/// \return The ASN.1 QoS characteristics.
inline asn1::f1ap::qos_characteristics_c
qos_characteristics_to_f1ap_asn1(const qos_characteristics_t& qos_characteristics)
{
  asn1::f1ap::qos_characteristics_c asn1_qos_characteristics;

  if (qos_characteristics.dyn_5qi.has_value()) {
    auto& asn1_dyn_5qi                          = asn1_qos_characteristics.set_dyn_5qi();
    asn1_dyn_5qi.qos_prio_level                 = qos_characteristics.dyn_5qi.value().qos_prio_level;
    asn1_dyn_5qi.packet_delay_budget            = qos_characteristics.dyn_5qi.value().packet_delay_budget;
    asn1_dyn_5qi.packet_error_rate.per_scalar   = qos_characteristics.dyn_5qi.value().packet_error_rate.per_scalar;
    asn1_dyn_5qi.packet_error_rate.per_exponent = qos_characteristics.dyn_5qi.value().packet_error_rate.per_exponent;

    if (qos_characteristics.dyn_5qi.value().five_qi.has_value()) {
      asn1_dyn_5qi.five_qi_present = true;
      asn1_dyn_5qi.five_qi         = five_qi_to_uint(qos_characteristics.dyn_5qi.value().five_qi.value());
    }

    if (qos_characteristics.dyn_5qi.value().delay_crit.has_value()) {
      asn1_dyn_5qi.delay_crit_present = true;
      asn1::string_to_enum(asn1_dyn_5qi.delay_crit, qos_characteristics.dyn_5qi.value().delay_crit.value());
    }

    if (qos_characteristics.dyn_5qi.value().averaging_win.has_value()) {
      asn1_dyn_5qi.averaging_win_present = true;
      asn1_dyn_5qi.averaging_win         = qos_characteristics.dyn_5qi.value().averaging_win.value();
    }

    if (qos_characteristics.dyn_5qi.value().max_data_burst_volume.has_value()) {
      asn1_dyn_5qi.max_data_burst_volume_present = true;
      asn1_dyn_5qi.max_data_burst_volume         = qos_characteristics.dyn_5qi.value().max_data_burst_volume.value();
    }

  } else if (qos_characteristics.non_dyn_5qi.has_value()) {
    auto& asn1_non_dyn_5qi = asn1_qos_characteristics.set_non_dyn_5qi();

    asn1_non_dyn_5qi.five_qi = five_qi_to_uint(qos_characteristics.non_dyn_5qi.value().five_qi);

    if (qos_characteristics.non_dyn_5qi.value().qos_prio_level.has_value()) {
      asn1_non_dyn_5qi.qos_prio_level_present = true;
      asn1_non_dyn_5qi.qos_prio_level         = qos_characteristics.non_dyn_5qi.value().qos_prio_level.value();
    }

    if (qos_characteristics.non_dyn_5qi.value().averaging_win.has_value()) {
      asn1_non_dyn_5qi.averaging_win_present = true;
      asn1_non_dyn_5qi.averaging_win         = qos_characteristics.non_dyn_5qi.value().averaging_win.value();
    }

    if (qos_characteristics.non_dyn_5qi.value().max_data_burst_volume.has_value()) {
      asn1_non_dyn_5qi.max_data_burst_volume_present = true;
      asn1_non_dyn_5qi.max_data_burst_volume = qos_characteristics.non_dyn_5qi.value().max_data_burst_volume.value();
    }
  } else {
    report_fatal_error("Invalid QoS characteristics. Either dynamic or non-dynamic 5qi must be set");
  }

  return asn1_qos_characteristics;
}

inline asn1::f1ap::qos_info_c qos_info_to_f1ap_asn1(const cu_cp_drb_info& qos_info)
{
  asn1::f1ap::qos_info_c asn1_qos_info;

  // qos info
  asn1_qos_info.set_choice_ext();
  auto& choice_ext = asn1_qos_info.choice_ext();
  choice_ext.load_info_obj(ASN1_F1AP_ID_DRB_INFO);

  auto& asn1_drb_info = choice_ext.value().drb_info();

  // drb qos
  // qos characteristics
  asn1_drb_info.drb_qos.qos_characteristics = qos_characteristics_to_f1ap_asn1(qos_info.drb_qos.qos_characteristics);

  // alloc and retention prio
  asn1_drb_info.drb_qos.ngra_nalloc_retention_prio.prio_level =
      qos_info.drb_qos.alloc_and_retention_prio.prio_level_arp;
  asn1::string_to_enum(asn1_drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_cap,
                       qos_info.drb_qos.alloc_and_retention_prio.pre_emption_cap);
  asn1::string_to_enum(asn1_drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_vulnerability,
                       qos_info.drb_qos.alloc_and_retention_prio.pre_emption_vulnerability);

  // gbr qos info
  if (qos_info.drb_qos.gbr_qos_info.has_value()) {
    asn1_drb_info.drb_qos.gbr_qos_flow_info_present = true;
    asn1_drb_info.drb_qos.gbr_qos_flow_info.max_flow_bit_rate_dl =
        qos_info.drb_qos.gbr_qos_info.value().max_flow_bit_rate_dl;
    asn1_drb_info.drb_qos.gbr_qos_flow_info.max_flow_bit_rate_ul =
        qos_info.drb_qos.gbr_qos_info.value().max_flow_bit_rate_ul;
    asn1_drb_info.drb_qos.gbr_qos_flow_info.guaranteed_flow_bit_rate_dl =
        qos_info.drb_qos.gbr_qos_info.value().guaranteed_flow_bit_rate_dl;
    asn1_drb_info.drb_qos.gbr_qos_flow_info.guaranteed_flow_bit_rate_ul =
        qos_info.drb_qos.gbr_qos_info.value().guaranteed_flow_bit_rate_ul;

    if (qos_info.drb_qos.gbr_qos_info.value().max_packet_loss_rate_dl.has_value()) {
      asn1_drb_info.drb_qos.gbr_qos_flow_info.max_packet_loss_rate_dl_present = true;
      asn1_drb_info.drb_qos.gbr_qos_flow_info.max_packet_loss_rate_dl =
          qos_info.drb_qos.gbr_qos_info.value().max_packet_loss_rate_dl.value();
    }

    if (qos_info.drb_qos.gbr_qos_info.value().max_packet_loss_rate_ul.has_value()) {
      asn1_drb_info.drb_qos.gbr_qos_flow_info.max_packet_loss_rate_ul_present = true;
      asn1_drb_info.drb_qos.gbr_qos_flow_info.max_packet_loss_rate_ul =
          qos_info.drb_qos.gbr_qos_info.value().max_packet_loss_rate_ul.value();
    }
  }

  // reflective qos attribute
  if (qos_info.drb_qos.reflective_qos_attribute.has_value()) {
    asn1_drb_info.drb_qos.reflective_qos_attribute_present = true;
    asn1::string_to_enum(asn1_drb_info.drb_qos.reflective_qos_attribute,
                         qos_info.drb_qos.reflective_qos_attribute.value());
  }

  // s nssai
  asn1_drb_info.snssai.sst.from_number(qos_info.s_nssai.sst);
  if (qos_info.s_nssai.sd.has_value()) {
    asn1_drb_info.snssai.sd_present = true;
    asn1_drb_info.snssai.sd.from_number(qos_info.s_nssai.sd.value());
  }

  // notif ctrl
  if (qos_info.notif_ctrl.has_value()) {
    asn1_drb_info.notif_ctrl_present = true;
    asn1::string_to_enum(asn1_drb_info.notif_ctrl, qos_info.notif_ctrl.value());
  }

  // flows mapped to drb list
  for (const auto& qos_flow : qos_info.flows_mapped_to_drb_list) {
    asn1::f1ap::flows_mapped_to_drb_item_s asn1_flow;
    asn1_flow.qos_flow_id               = qos_flow_id_to_uint(qos_flow.qos_flow_id);
    asn1_flow.qos_flow_level_qos_params = asn1_drb_info.drb_qos;

    asn1_drb_info.flows_mapped_to_drb_list.push_back(asn1_flow);
  }

  return asn1_qos_info;
}

} // namespace srs_cu_cp
} // namespace srsran
