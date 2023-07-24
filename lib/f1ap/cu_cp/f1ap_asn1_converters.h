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

#include "srsran/adt/optional.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_ue_context_update.h"
#include "srsran/f1ap/cu_cp/f1ap_interface_management_types.h"
#include "srsran/ran/nr_cgi.h"
#include <string>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

/// \brief Convert F1AP ASN.1 Cause to \c cause_t type.
/// \param f1ap_cause The F1AP Cause.
/// \return The cause_t type.
inline cause_t f1ap_asn1_to_cause(asn1::f1ap::cause_c f1ap_cause)
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
inline asn1::f1ap::cause_c cause_to_f1ap_asn1(cause_t cause)
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
inline nr_cell_id_t f1ap_asn1_to_nr_cell_identity(asn1::f1ap::nr_cgi_s& f1ap_cgi)
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
    auto& asn1_dyn_5qi               = asn1_qos_characteristics.set_dyn_5qi();
    asn1_dyn_5qi.qos_prio_level      = qos_prio_level_to_uint(qos_characteristics.dyn_5qi.value().qos_prio_level);
    asn1_dyn_5qi.packet_delay_budget = qos_characteristics.dyn_5qi.value().packet_delay_budget;
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
      asn1_non_dyn_5qi.qos_prio_level =
          qos_prio_level_to_uint(qos_characteristics.non_dyn_5qi.value().qos_prio_level.value());
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

/// \brief Convert \c f1ap_notif_ctrl to F1AP ASN.1.
/// \param[in] notif_ctrl The common type notif ctrl.
/// \return The ASN.1 notif ctrl.
inline asn1::f1ap::notif_ctrl_e f1ap_notif_ctrl_to_asn1(const f1ap_notif_ctrl& notif_ctrl)
{
  asn1::f1ap::notif_ctrl_e asn1_notif_ctrl;

  switch (notif_ctrl) {
    case f1ap_notif_ctrl::active:
      asn1_notif_ctrl = asn1::f1ap::notif_ctrl_opts::options::active;
      break;
    case f1ap_notif_ctrl::not_active:
      asn1_notif_ctrl = asn1::f1ap::notif_ctrl_opts::options::not_active;
      break;
  }

  return asn1_notif_ctrl;
}

/// \brief Convert QoS info to F1AP ASN.1.
/// \param[in] qos_info The common type qos info.
/// \return The ASN.1 qos info.
inline asn1::f1ap::qos_info_c f1ap_qos_info_to_asn1(const f1ap_drb_info& qos_info)
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

  // assert valid conversion result
  srsran_assert(asn1_drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_cap !=
                    asn1::f1ap::pre_emption_cap_e::nulltype,
                "Invalid preemption capability type");

  srsran_assert(asn1_drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_vulnerability !=
                    asn1::f1ap::pre_emption_vulnerability_opts::nulltype,
                "Invalid preemption vulnerability type");

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
    asn1::bool_to_enum(asn1_drb_info.drb_qos.reflective_qos_attribute,
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
    asn1_drb_info.notif_ctrl         = f1ap_notif_ctrl_to_asn1(qos_info.notif_ctrl.value());
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

/// \brief Convert \c f1ap_cell_ul_cfg to F1AP ASN.1.
/// \param[in] cell_ul_cfg The common type cell ul cfg.
/// \return The ASN.1 cell ul cfg.
inline asn1::f1ap::cell_ul_cfg_e cell_ul_cfg_to_asn1(const f1ap_cell_ul_cfg& cell_ul_cfg)
{
  asn1::f1ap::cell_ul_cfg_e asn1_cell_ul_cfg;

  switch (cell_ul_cfg) {
    case f1ap_cell_ul_cfg::none:
      asn1_cell_ul_cfg = asn1::f1ap::cell_ul_cfg_opts::options::none;
      break;
    case f1ap_cell_ul_cfg::ul:
      asn1_cell_ul_cfg = asn1::f1ap::cell_ul_cfg_opts::options::ul;
      break;
    case f1ap_cell_ul_cfg::sul:
      asn1_cell_ul_cfg = asn1::f1ap::cell_ul_cfg_opts::options::sul;
      break;
    case f1ap_cell_ul_cfg::ul_and_sul:
      asn1_cell_ul_cfg = asn1::f1ap::cell_ul_cfg_opts::options::ul_and_sul;
      break;
  }

  return asn1_cell_ul_cfg;
}

/// \brief Convert scell to be setup/setup mod item to F1AP ASN.1.
/// \param[out] asn1_scell_to_be_setup_mod_item The ASN.1 struct to store the result.
/// \param[in] scell_to_be_setup_mod_item The scell to be setup/setup item mod common type struct.
template <typename template_asn1_item>
inline void f1ap_scell_to_be_setup_mod_item_to_asn1(template_asn1_item& asn1_scell_to_be_setup_mod_item,
                                                    const f1ap_scell_to_be_setup_mod_item& scell_to_be_setup_mod_item)
{
  // scell id
  asn1_scell_to_be_setup_mod_item.scell_id.nr_cell_id.from_number(scell_to_be_setup_mod_item.scell_id.nci);
  asn1_scell_to_be_setup_mod_item.scell_id.plmn_id.from_string(scell_to_be_setup_mod_item.scell_id.plmn_hex);

  // scell idx
  asn1_scell_to_be_setup_mod_item.scell_idx = scell_to_be_setup_mod_item.scell_idx;

  // scell ul cfg
  if (scell_to_be_setup_mod_item.scell_ul_cfg.has_value()) {
    asn1_scell_to_be_setup_mod_item.scell_ul_cfg_present = true;
    asn1_scell_to_be_setup_mod_item.scell_ul_cfg = cell_ul_cfg_to_asn1(scell_to_be_setup_mod_item.scell_ul_cfg.value());
  }
}

/// \brief Convert \c f1ap_dupl_ind to F1AP ASN.1.
/// \param[in] dupl_ind The common type dupl ind.
/// \return The ASN.1 dupl ind.
inline asn1::f1ap::dupl_ind_e f1ap_dupl_ind_to_asn1(const f1ap_dupl_ind& dupl_ind)
{
  asn1::f1ap::dupl_ind_e asn1_dupl_ind;

  switch (dupl_ind) {
    case f1ap_dupl_ind::true_value:
      asn1_dupl_ind = asn1::f1ap::dupl_ind_opts::options::true_value;
      break;
    case f1ap_dupl_ind::false_value:
      asn1_dupl_ind = asn1::f1ap::dupl_ind_opts::options::false_value;
      break;
  }

  return asn1_dupl_ind;
}

/// \brief Convert srbs to be setup/setup mod item to F1AP ASN.1.
/// \param[out] asn1_srbs_to_be_setup_mod_item The ASN.1 struct to store the result.
/// \param[in] srbs_to_be_setup_mod_item The srbs to be setup/setup item mod common type struct.
template <typename template_asn1_item>
inline void f1ap_srbs_to_be_setup_mod_item_to_asn1(template_asn1_item&                   asn1_srbs_to_be_setup_mod_item,
                                                   const f1ap_srbs_to_be_setup_mod_item& srbs_to_be_setup_mod_item)
{
  // srb id
  asn1_srbs_to_be_setup_mod_item.srb_id = srb_id_to_uint(srbs_to_be_setup_mod_item.srb_id);

  // dupl ind
  if (srbs_to_be_setup_mod_item.dupl_ind.has_value()) {
    asn1_srbs_to_be_setup_mod_item.dupl_ind = f1ap_dupl_ind_to_asn1(srbs_to_be_setup_mod_item.dupl_ind.value());
  }
}

/// \brief Convert \c f1ap_ul_ue_cfg to F1AP ASN.1.
/// \param[in] ul_ue_cfg The common type ul ue cfg.
/// \return The ASN.1 ul ue cfg.
inline asn1::f1ap::ul_ue_cfg_e f1ap_ul_ue_cfg_to_asn1(const f1ap_ul_ue_cfg& ul_ue_cfg)
{
  asn1::f1ap::ul_ue_cfg_e asn1_ul_ue_cfg;

  switch (ul_ue_cfg) {
    case f1ap_ul_ue_cfg::no_data:
      asn1_ul_ue_cfg = asn1::f1ap::ul_ue_cfg_opts::options::no_data;
      break;
    case f1ap_ul_ue_cfg::shared:
      asn1_ul_ue_cfg = asn1::f1ap::ul_ue_cfg_opts::options::shared;
      break;
    case f1ap_ul_ue_cfg::only:
      asn1_ul_ue_cfg = asn1::f1ap::ul_ue_cfg_opts::options::only;
      break;
  }

  return asn1_ul_ue_cfg;
}

/// \brief Convert \c f1ap_dupl_activation to F1AP ASN.1.
/// \param[in] dupl_activation The common type dupl activation.
/// \return The ASN.1 dupl activation.
inline asn1::f1ap::dupl_activation_e f1ap_dupl_activation_to_asn1(const f1ap_dupl_activation& dupl_activation)
{
  asn1::f1ap::dupl_activation_e asn1_dupl_activation;

  switch (dupl_activation) {
    case f1ap_dupl_activation::active:
      asn1_dupl_activation = asn1::f1ap::dupl_activation_e::dupl_activation_opts::options::active;
      break;
    case f1ap_dupl_activation::inactive:
      asn1_dupl_activation = asn1::f1ap::dupl_activation_e::dupl_activation_opts::options::inactive;
      break;
  }

  return asn1_dupl_activation;
}

/// \brief Convert drbs to be setup/setup mod item to F1AP ASN.1.
/// \param[out] asn1_drbs_to_be_setup_mod_item The ASN.1 struct to store the result.
/// \param[in] drbs_to_be_setup_mod_item The drbs to be setup/setup item mod common type struct.
template <typename template_asn1_item>
inline void f1ap_drbs_to_be_setup_mod_item_to_asn1(template_asn1_item&                   asn1_drb_to_be_setup_mod_item,
                                                   const f1ap_drbs_to_be_setup_mod_item& drb_to_be_setup_mod_item)
{
  // drb id
  asn1_drb_to_be_setup_mod_item.drb_id = drb_id_to_uint(drb_to_be_setup_mod_item.drb_id);

  // qos info
  asn1_drb_to_be_setup_mod_item.qos_info = f1ap_qos_info_to_asn1(drb_to_be_setup_mod_item.qos_info);

  // ul up tnl info to be setup list
  for (const auto& ul_up_tnl_info_item : drb_to_be_setup_mod_item.ul_up_tnl_info_to_be_setup_list) {
    asn1::f1ap::ul_up_tnl_info_to_be_setup_item_s item;
    up_transport_layer_info_to_asn1(item.ul_up_tnl_info, ul_up_tnl_info_item);
    asn1_drb_to_be_setup_mod_item.ul_up_tnl_info_to_be_setup_list.push_back(item);
  }

  // rlc mode
  asn1_drb_to_be_setup_mod_item.rlc_mode = rlc_mode_to_f1ap_asn1(drb_to_be_setup_mod_item.rlc_mod);

  // ul cfg
  if (drb_to_be_setup_mod_item.ul_cfg.has_value()) {
    asn1_drb_to_be_setup_mod_item.ul_cfg_present = true;
    asn1_drb_to_be_setup_mod_item.ul_cfg.ul_ue_cfg =
        f1ap_ul_ue_cfg_to_asn1(drb_to_be_setup_mod_item.ul_cfg.value().ul_ue_cfg);
  }

  // dupl activation
  if (drb_to_be_setup_mod_item.dupl_activation.has_value()) {
    asn1_drb_to_be_setup_mod_item.dupl_activation_present = true;
    asn1_drb_to_be_setup_mod_item.dupl_activation =
        f1ap_dupl_activation_to_asn1(drb_to_be_setup_mod_item.dupl_activation.value());
  }
}

/// \brief Convert \c f1ap_tx_action_ind to F1AP ASN.1.
/// \param[in] tx_action_ind The common type tx action ind.
/// \return The ASN.1 tx action ind.
inline asn1::f1ap::tx_action_ind_e f1ap_tx_action_ind_to_asn1(const f1ap_tx_action_ind& tx_action_ind)
{
  asn1::f1ap::tx_action_ind_e asn1_tx_action_ind;

  switch (tx_action_ind) {
    case f1ap_tx_action_ind::stop:
      asn1_tx_action_ind = asn1::f1ap::tx_action_ind_opts::options::stop;
      break;
    case f1ap_tx_action_ind::restart:
      asn1_tx_action_ind = asn1::f1ap::tx_action_ind_opts::options::restart;
      break;
  }

  return asn1_tx_action_ind;
}

/// \brief Convert \c f1ap_rrc_recfg_complete_ind to F1AP ASN.1.
/// \param[in] rrc_recfg_complete_ind The common type rrc recfg complete ind.
/// \return The ASN.1 rrc recfg complete ind.
inline asn1::f1ap::rrc_recfg_complete_ind_e
f1ap_rrc_recfg_complete_ind_to_asn1(const f1ap_rrc_recfg_complete_ind& rrc_recfg_complete_ind)
{
  asn1::f1ap::rrc_recfg_complete_ind_e asn1_rrc_recfg_complete_ind;

  switch (rrc_recfg_complete_ind) {
    case f1ap_rrc_recfg_complete_ind::true_value:
      asn1_rrc_recfg_complete_ind = asn1::f1ap::rrc_recfg_complete_ind_opts::options::true_value;
      break;
    case f1ap_rrc_recfg_complete_ind::fail:
      asn1_rrc_recfg_complete_ind = asn1::f1ap::rrc_recfg_complete_ind_opts::options::fail;
      break;
  }

  return asn1_rrc_recfg_complete_ind;
}

/// \brief Calculate the 5G-S-TMSI from the common type 5G-S-TMSI struct.
inline uint64_t five_g_s_tmsi_struct_to_number(const cu_cp_five_g_s_tmsi& five_g_s_tmsi)
{
  // 5G-S-TMSI is a 48 bit string consisting of <aMFSetId (10 bit)><aMFPointer (6 bit)><5G-TMSI (32 bit)>
  return ((uint64_t)five_g_s_tmsi.amf_set_id << 38) + ((uint64_t)five_g_s_tmsi.amf_pointer << 32) +
         five_g_s_tmsi.five_g_tmsi;
}

/// \brief Convert F1AP ASN.1 to \c cu_cp_tx_bw.
/// \param[in] asn1_tx_bw The ASN.1 type tx bw.
/// \return The common type tx bw.
inline cu_cp_tx_bw f1ap_asn1_to_tx_bw(const asn1::f1ap::tx_bw_s& asn1_tx_bw)
{
  cu_cp_tx_bw tx_bw;

  // nr scs
  tx_bw.nr_scs = to_subcarrier_spacing(asn1_tx_bw.nr_scs.to_string());

  // nr nrb
  tx_bw.nr_nrb = asn1_tx_bw.nr_nrb.to_number();

  return tx_bw;
}

/// \brief Convert F1AP ASN.1 to \c cu_cp_nr_freq_info.
/// \param[in] asn1_nr_freq_info The ASN.1 type nr freq info.
/// \return The common type nr freq info.
inline cu_cp_nr_freq_info f1ap_asn1_to_nr_freq_info(const asn1::f1ap::nr_freq_info_s& asn1_nr_freq_info)
{
  cu_cp_nr_freq_info nr_freq_info;

  // nr arfcn
  nr_freq_info.nr_arfcn = asn1_nr_freq_info.nr_arfcn;

  // sul info
  if (asn1_nr_freq_info.sul_info_present) {
    cu_cp_sul_info sul_info;

    // sul nr arfcn
    sul_info.sul_nr_arfcn = asn1_nr_freq_info.sul_info.sul_nr_arfcn;

    // sul tx bw
    sul_info.sul_tx_bw = f1ap_asn1_to_tx_bw(asn1_nr_freq_info.sul_info.sul_tx_bw);

    nr_freq_info.sul_info = sul_info;
  }

  // freq band list nr
  for (const auto& asn1_freq_band : asn1_nr_freq_info.freq_band_list_nr) {
    cu_cp_freq_band_nr_item freq_band;

    // freq band ind
    freq_band.freq_band_ind_nr = asn1_freq_band.freq_band_ind_nr;

    // supported sul band list
    for (const auto& asn1_sul_band : asn1_freq_band.supported_sul_band_list) {
      freq_band.supported_sul_band_list.push_back(cu_cp_supported_sul_freq_band_item{asn1_sul_band.freq_band_ind_nr});
    }

    nr_freq_info.freq_band_list_nr.push_back(freq_band);
  }

  return nr_freq_info;
}

/// \brief Convert F1AP ASN.1 to \c cu_cp_nr_mode_info.
/// \param[in] asn1_nr_mode_info The ASN.1 type nr mode info.
/// \return The common type nr mode info.
inline cu_cp_nr_mode_info f1ap_asn1_to_nr_mode_info(const asn1::f1ap::nr_mode_info_c& asn1_nr_mode_info)
{
  cu_cp_nr_mode_info nr_mode_info;

  // fdd
  if (asn1_nr_mode_info.type() == asn1::f1ap::nr_mode_info_c::types_opts::fdd) {
    auto& asn1_fdd_info = asn1_nr_mode_info.fdd();

    cu_cp_fdd_info fdd_info;

    // ul nr freq info
    fdd_info.ul_nr_freq_info = f1ap_asn1_to_nr_freq_info(asn1_fdd_info.ul_nr_freq_info);

    // dl nr freq info
    fdd_info.dl_nr_freq_info = f1ap_asn1_to_nr_freq_info(asn1_fdd_info.dl_nr_freq_info);

    // ul tx bw
    fdd_info.ul_tx_bw = f1ap_asn1_to_tx_bw(asn1_fdd_info.ul_tx_bw);

    // dl tx bw
    fdd_info.dl_tx_bw = f1ap_asn1_to_tx_bw(asn1_fdd_info.dl_tx_bw);

    nr_mode_info.fdd = fdd_info;
  } else if (asn1_nr_mode_info.type() == asn1::f1ap::nr_mode_info_c::types_opts::tdd) {
    auto& asn1_tdd_info = asn1_nr_mode_info.tdd();

    cu_cp_tdd_info tdd_info;

    // nr freq info
    tdd_info.nr_freq_info = f1ap_asn1_to_nr_freq_info(asn1_tdd_info.nr_freq_info);

    // tx bw
    tdd_info.tx_bw = f1ap_asn1_to_tx_bw(asn1_tdd_info.tx_bw);

    nr_mode_info.tdd = tdd_info;
  } else {
    report_fatal_error("Invalid NR mode.");
  }

  return nr_mode_info;
}

/// \brief Convert \c f1ap_rat_freq_prio_info to F1AP ASN.1.
/// \param[in] rat_freq_prio_info The common type rat freq prio info.
/// \return The ASN.1 rat freq prio info.
inline asn1::f1ap::rat_freq_prio_info_c
f1ap_rat_freq_prio_info_to_asn1(const f1ap_rat_freq_prio_info& rat_freq_prio_info)
{
  asn1::f1ap::rat_freq_prio_info_c asn1_rat_freq_prio_info;

  if (rat_freq_prio_info.endc.has_value()) {
    asn1_rat_freq_prio_info.set_endc();
    asn1_rat_freq_prio_info.endc() = rat_freq_prio_info.endc.value();
  } else if (rat_freq_prio_info.ngran.has_value()) {
    asn1_rat_freq_prio_info.set_ngran();
    asn1_rat_freq_prio_info.ngran() = rat_freq_prio_info.ngran.value();
  } else {
    report_fatal_error("Cannot convert rat freq prio info to ASN.1 type");
  }

  return asn1_rat_freq_prio_info;
}

/// \brief Convert \c nr_cell_global_id_t to F1AP ASN.1.
/// \param[in] nr_cgi The common type nr cgi.
/// \return The ASN.1 nr cgi.
inline asn1::f1ap::nr_cgi_s nr_cgi_to_f1ap_asn1(const nr_cell_global_id_t& nr_cgi)
{
  asn1::f1ap::nr_cgi_s asn1_nr_cgi;

  // nr cell id
  asn1_nr_cgi.nr_cell_id.from_number(nr_cgi.nci);

  // plmn id
  asn1_nr_cgi.plmn_id.from_string(nr_cgi.plmn_hex);

  return asn1_nr_cgi;
}

/// \brief Convert F1AP ASN.1 to \c nr_cell_global_id_t.
/// \param[in] asn1_nr_cgi The ASN.1 type nr cgi.
/// \return The common type nr cgi.
inline nr_cell_global_id_t f1ap_asn1_to_nr_cgi(const asn1::f1ap::nr_cgi_s& asn1_nr_cgi)
{
  nr_cell_global_id_t nr_cgi;

  // nr cell id
  nr_cgi.nci = asn1_nr_cgi.nr_cell_id.to_number();

  // plmn id
  nr_cgi.plmn_hex = asn1_nr_cgi.plmn_id.to_string();

  return nr_cgi;
}

/// \brief Convert F1AP ASN.1 drbs setup/setup mod item to common type.
/// \param[in] asn1_drbs_to_be_setup_mod_item The ASN.1 drbs setup/setup mod item struct.
/// \return The drbs setup/setup item mod common type struct.
template <typename template_asn1_item>
inline f1ap_drbs_setup_mod_item asn1_to_f1ap_drbs_setup_mod_item(const template_asn1_item& asn1_drbs_setup_mod_item)
{
  f1ap_drbs_setup_mod_item drb_setup_mod_item;

  // drb id
  drb_setup_mod_item.drb_id = uint_to_drb_id(asn1_drbs_setup_mod_item.drb_id);

  // Add DL UP TNL to be setup list
  for (auto asn1_dl_up_tnl_info_to_be_setup_item : asn1_drbs_setup_mod_item.dl_up_tnl_info_to_be_setup_list) {
    f1ap_dl_up_tnl_info_to_be_setup_item dl_up_tnl_info_to_be_setup_item;
    dl_up_tnl_info_to_be_setup_item.dl_up_tnl_info =
        asn1_to_up_transport_layer_info(asn1_dl_up_tnl_info_to_be_setup_item.dl_up_tnl_info);
    drb_setup_mod_item.dl_up_tnl_info_to_be_setup_list.push_back(dl_up_tnl_info_to_be_setup_item);
  }

  if (asn1_drbs_setup_mod_item.lcid_present) {
    drb_setup_mod_item.lcid = uint_to_lcid(asn1_drbs_setup_mod_item.lcid);
  }

  return drb_setup_mod_item;
}

/// \brief Convert F1AP ASN.1 srbs failed to be setup/setup mod item to common type.
/// \param[in] asn1_srbs_failed_to_be_setup_mod_item The ASN.1 srbs failed to be setup/setup mod item struct.
/// \return The srbs failed to be setup/setup item mod common type struct.
template <typename template_asn1_item>
inline f1ap_srbs_failed_to_be_setup_mod_item
asn1_to_f1ap_srbs_failed_to_be_setup_mod_item(const template_asn1_item& asn1_srbs_failed_to_be_setup_mod_item)
{
  f1ap_srbs_failed_to_be_setup_mod_item srbs_failed_to_be_setup_mod_item;

  // srb id
  srbs_failed_to_be_setup_mod_item.srb_id = int_to_srb_id(asn1_srbs_failed_to_be_setup_mod_item.srb_id);
  if (asn1_srbs_failed_to_be_setup_mod_item.cause_present) {
    srbs_failed_to_be_setup_mod_item.cause = f1ap_asn1_to_cause(asn1_srbs_failed_to_be_setup_mod_item.cause);
  }

  return srbs_failed_to_be_setup_mod_item;
}

// \brief Convert F1AP ASN.1 drbs failed to be setup/setup mod item to common type.
/// \param[in] asn1_drbs_failed_to_be_setup_mod_item The ASN.1 drbs failed to be setup/setup mod item struct.
/// \return The drbs failed to be setup/setup item mod common type struct.
template <typename template_asn1_item>
inline f1ap_drbs_failed_to_be_setup_mod_item
asn1_to_f1ap_drbs_failed_to_be_setup_mod_item(const template_asn1_item& asn1_drbs_failed_to_be_setup_mod_item)
{
  f1ap_drbs_failed_to_be_setup_mod_item drbs_failed_to_be_setup_mod_item;

  // drb id
  drbs_failed_to_be_setup_mod_item.drb_id = uint_to_drb_id(asn1_drbs_failed_to_be_setup_mod_item.drb_id);
  if (asn1_drbs_failed_to_be_setup_mod_item.cause_present) {
    drbs_failed_to_be_setup_mod_item.cause = f1ap_asn1_to_cause(asn1_drbs_failed_to_be_setup_mod_item.cause);
  }

  return drbs_failed_to_be_setup_mod_item;
}

/// \brief Convert F1AP ASN.1 srbs setup/setup mod item to common type.
/// \param[in] asn1_srbs_to_be_setup_mod_item The ASN.1 srbs setup/setup mod item struct.
/// \return The srbs setup/setup item mod common type struct.
template <typename template_asn1_item>
inline f1ap_srbs_setup_mod_item asn1_to_f1ap_srbs_setup_mod_item(const template_asn1_item& asn1_srbs_setup_mod_item)
{
  f1ap_srbs_setup_mod_item srbs_setup_mod_item;

  // srb id
  srbs_setup_mod_item.srb_id = int_to_srb_id(asn1_srbs_setup_mod_item.srb_id);

  // lcid
  srbs_setup_mod_item.lcid = uint_to_lcid(asn1_srbs_setup_mod_item.lcid);

  return srbs_setup_mod_item;
}

} // namespace srs_cu_cp
} // namespace srsran
