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

#include "asn1_helpers.h"
#include "cu_cp/f1ap_asn1_converters.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/ran/qos/five_qi_qos_mapping.h"

using namespace srsran;
using namespace asn1::f1ap;

f1ap_cause_t srsran::asn1_to_cause(asn1::f1ap::cause_c asn1_cause)
{
  f1ap_cause_t cause;

  switch (asn1_cause.type()) {
    case asn1::f1ap::cause_c::types_opts::radio_network:
      cause = static_cast<f1ap_cause_radio_network_t>(asn1_cause.radio_network().value);
      break;
    case asn1::f1ap::cause_c::types_opts::transport:
      cause = static_cast<f1ap_cause_transport_t>(asn1_cause.transport().value);
      break;
    case asn1::f1ap::cause_c::types_opts::protocol:
      cause = static_cast<cause_protocol_t>(asn1_cause.protocol().value);
      break;
    case asn1::f1ap::cause_c::types_opts::misc:
      cause = static_cast<cause_misc_t>(asn1_cause.misc().value);
      break;
    default:
      report_fatal_error("Cannot convert F1AP ASN.1 cause {} to common type", fmt::underlying(asn1_cause.type().value));
  }

  return cause;
}

/// \brief Convert \c f1ap_cause_t type to F1AP cause.
/// \param cause The f1ap_cause_t type.
/// \return The F1AP cause.
asn1::f1ap::cause_c srsran::cause_to_asn1(f1ap_cause_t cause)
{
  asn1::f1ap::cause_c asn1_cause;

  if (const auto* result = std::get_if<f1ap_cause_radio_network_t>(&cause)) {
    asn1_cause.set_radio_network() = static_cast<asn1::f1ap::cause_radio_network_opts::options>(*result);
    return asn1_cause;
  }
  if (const auto* result = std::get_if<f1ap_cause_transport_t>(&cause)) {
    asn1_cause.set_transport() = static_cast<asn1::f1ap::cause_transport_opts::options>(*result);
    return asn1_cause;
  }
  if (const auto* result = std::get_if<cause_protocol_t>(&cause)) {
    asn1_cause.set_protocol() = static_cast<asn1::f1ap::cause_protocol_opts::options>(*result);
    return asn1_cause;
  }
  if (const auto* result = std::get_if<cause_misc_t>(&cause)) {
    asn1_cause.set_misc() = static_cast<asn1::f1ap::cause_misc_opts::options>(*result);
    return asn1_cause;
  }

  report_fatal_error("Cannot convert cause to F1AP type: {}", cause);
  return asn1_cause;
}

expected<nr_cell_global_id_t> srsran::cgi_from_asn1(const asn1::f1ap::nr_cgi_s& asn1_cgi)
{
  auto plmn = plmn_identity::from_bytes(asn1_cgi.plmn_id.to_bytes());
  if (not plmn.has_value()) {
    return make_unexpected(plmn.error());
  }
  auto nci = nr_cell_identity::create(asn1_cgi.nr_cell_id.to_number());
  if (not nci.has_value()) {
    return make_unexpected(nci.error());
  }
  return nr_cell_global_id_t{plmn.value(), nci.value()};
}

asn1::f1ap::nr_cgi_s srsran::cgi_to_asn1(const nr_cell_global_id_t& cgi)
{
  asn1::f1ap::nr_cgi_s asn1_nr_cgi;
  asn1_nr_cgi.nr_cell_id.from_number(cgi.nci.value());
  asn1_nr_cgi.plmn_id = cgi.plmn_id.to_bytes();
  return asn1_nr_cgi;
}

expected<nr_cell_identity> srsran::nr_cell_id_from_asn1(asn1::f1ap::nr_cgi_s& f1ap_cgi)
{
  return nr_cell_identity::create(f1ap_cgi.nr_cell_id.to_number());
}

static pdcp_sn_size pdcp_sn_size_from_f1ap_asn1(const pdcp_sn_len_e& asn1_pdcp_sn_size)
{
  switch (asn1_pdcp_sn_size) {
    case pdcp_sn_len_opts::twelve_bits:
      return pdcp_sn_size::size12bits;
    case pdcp_sn_len_opts::eighteen_bits:
      return pdcp_sn_size::size18bits;
    default:
      return pdcp_sn_size::invalid;
  }
}

static pdcp_sn_len_e pdcp_sn_size_to_f1ap_asn1(pdcp_sn_size sn_size)
{
  switch (sn_size) {
    case pdcp_sn_size::size12bits:
      return asn1::f1ap::pdcp_sn_len_opts::twelve_bits;
    case pdcp_sn_size::size18bits:
      return asn1::f1ap::pdcp_sn_len_opts::eighteen_bits;
    default:
      return asn1::f1ap::pdcp_sn_len_opts::nulltype;
  }
}

// Helper function to fill asn1 DRBs-SetupMod and DRBs-Modified types.
template <typename ASN1Type>
static void fill_drb_setup_mod_common(ASN1Type& asn1obj, const f1ap_drb_setupmod& drb)
{
  asn1obj.drb_id       = drb_id_to_uint(drb.drb_id);
  asn1obj.lcid_present = drb.lcid.has_value();
  if (asn1obj.lcid_present) {
    asn1obj.lcid = drb.lcid.value();
  }
  asn1obj.dl_up_tnl_info_to_be_setup_list.resize(drb.dluptnl_info_list.size());
  for (unsigned j = 0; j != drb.dluptnl_info_list.size(); ++j) {
    up_transport_layer_info_to_asn1(asn1obj.dl_up_tnl_info_to_be_setup_list[j].dl_up_tnl_info,
                                    drb.dluptnl_info_list[j]);
  }
}

static std::vector<up_transport_layer_info>
make_ul_up_tnl_info_list(const asn1::f1ap::ul_up_tnl_info_to_be_setup_list_l& asn1_list)
{
  std::vector<up_transport_layer_info> uluptnl_info_list;
  uluptnl_info_list.reserve(asn1_list.size());
  for (const auto& tnl_info : asn1_list) {
    uluptnl_info_list.push_back(asn1_to_up_transport_layer_info(tnl_info.ul_up_tnl_info));
  }
  return uluptnl_info_list;
}

static std::vector<up_transport_layer_info>
make_dl_up_tnl_info_list(const asn1::f1ap::dl_up_tnl_info_to_be_setup_list_l& asn1_list)
{
  std::vector<up_transport_layer_info> uptnl_info_list;
  uptnl_info_list.reserve(asn1_list.size());
  for (const auto& tnl_info : asn1_list) {
    uptnl_info_list.push_back(asn1_to_up_transport_layer_info(tnl_info.dl_up_tnl_info));
  }
  return uptnl_info_list;
}

static ul_up_tnl_info_to_be_setup_list_l make_asn1_ul_up_tnl_info_list(span<const up_transport_layer_info> list)
{
  asn1::f1ap::ul_up_tnl_info_to_be_setup_list_l ret;
  ret.resize(list.size());
  for (unsigned i = 0; i != list.size(); ++i) {
    up_transport_layer_info_to_asn1(ret[i].ul_up_tnl_info, list[i]);
  }
  return ret;
}

static rlc_mode get_rlc_mode(const asn1::f1ap::rlc_mode_e& asn1type)
{
  switch (asn1type) {
    case asn1::f1ap::rlc_mode_opts::rlc_am:
      return rlc_mode::am;
    case asn1::f1ap::rlc_mode_opts::rlc_um_bidirectional:
      return rlc_mode::um_bidir;
    case asn1::f1ap::rlc_mode_opts::rlc_um_unidirectional_ul:
      return rlc_mode::um_unidir_ul;
    case asn1::f1ap::rlc_mode_opts::rlc_um_unidirectional_dl:
      return rlc_mode::um_unidir_dl;
    default:
      break;
  }
  report_fatal_error("Invalid RLC mode");
}

/// \brief Convert \c rlc_mode to F1AP ASN.1.
/// \param rlc_mod The common type RLC mode.
/// \return The ASN.1 RLC mode.
static rlc_mode_e rlc_mode_to_f1ap_asn1(const rlc_mode& mode)
{
  rlc_mode_e asn1type;
  switch (mode) {
    case rlc_mode::am:
      return rlc_mode_opts::rlc_am;
    case rlc_mode::um_bidir:
      return rlc_mode_opts::rlc_um_bidirectional;
    case rlc_mode::um_unidir_ul:
      return rlc_mode_opts::rlc_um_unidirectional_ul;
    case rlc_mode::um_unidir_dl:
      return rlc_mode_opts::rlc_um_unidirectional_dl;
    default:
      break;
  }
  return asn1type;
}

static f1ap_drb_info drb_info_from_f1ap_asn1(const asn1::f1ap::qos_info_c& asn1_qos)
{
  f1ap_drb_info out;

  // TODO: Handle Dynamic 5QI.
  const drb_info_s& asn1_drb_info = asn1_qos.choice_ext().value().drb_info();
  // Convert non-dynamic 5QI descriptor.
  const non_dyn_5qi_descriptor_s& asn1_non_dyn_5qi = asn1_drb_info.drb_qos.qos_characteristics.non_dyn_5qi();
  out.drb_qos.qos_desc                             = non_dyn_5qi_descriptor{};
  non_dyn_5qi_descriptor& nondyn_5qi               = out.drb_qos.qos_desc.get_nondyn_5qi();
  nondyn_5qi.five_qi                               = uint_to_five_qi(asn1_non_dyn_5qi.five_qi);
  out.drb_qos.alloc_retention_prio.prio_level_arp  = asn1_drb_info.drb_qos.ngra_nalloc_retention_prio.prio_level;
  out.s_nssai.sst                                  = slice_service_type{(uint8_t)asn1_drb_info.snssai.sst.to_number()};
  if (asn1_drb_info.snssai.sd_present) {
    out.s_nssai.sd = slice_differentiator::create(asn1_drb_info.snssai.sd.to_number()).value();
  }

  const auto* five_qi_params = get_5qi_to_qos_characteristics_mapping(nondyn_5qi.five_qi);
  const bool  is_gbr         = five_qi_params && (five_qi_params->res_type == qos_flow_resource_type::gbr ||
                                         five_qi_params->res_type == qos_flow_resource_type::delay_critical_gbr);

  // Note: As per TS 48.473, 9.3.1.45, "This IE shall be present for GBR QoS Flows only and is ignored otherwise."
  if (is_gbr and asn1_drb_info.drb_qos.gbr_qos_flow_info_present) {
    auto& gbr     = out.drb_qos.gbr_qos_info.emplace();
    gbr.max_br_dl = asn1_drb_info.drb_qos.gbr_qos_flow_info.max_flow_bit_rate_dl;
    gbr.max_br_ul = asn1_drb_info.drb_qos.gbr_qos_flow_info.max_flow_bit_rate_ul;
    gbr.gbr_dl    = asn1_drb_info.drb_qos.gbr_qos_flow_info.guaranteed_flow_bit_rate_dl;
    gbr.gbr_ul    = asn1_drb_info.drb_qos.gbr_qos_flow_info.guaranteed_flow_bit_rate_ul;
    if (asn1_drb_info.drb_qos.gbr_qos_flow_info.max_packet_loss_rate_dl_present) {
      gbr.max_packet_loss_rate_dl.emplace(asn1_drb_info.drb_qos.gbr_qos_flow_info.max_packet_loss_rate_dl);
    }
    if (asn1_drb_info.drb_qos.gbr_qos_flow_info.max_packet_loss_rate_ul_present) {
      gbr.max_packet_loss_rate_dl.emplace(asn1_drb_info.drb_qos.gbr_qos_flow_info.max_packet_loss_rate_ul);
    }
  }

  return out;
}

/// \brief Convert \c qos_characteristics_t to F1AP ASN.1.
/// \param qos_characteristics The common type QoS characteristics.
/// \return The ASN.1 QoS characteristics.
static asn1::f1ap::qos_characteristics_c qos_characteristics_to_f1ap_asn1(const qos_characteristics& qos_desc)
{
  asn1::f1ap::qos_characteristics_c asn1_qos_characteristics;

  if (qos_desc.is_dyn_5qi()) {
    auto&                     asn1_dyn_5qi      = asn1_qos_characteristics.set_dyn_5qi();
    const dyn_5qi_descriptor& dyn_5qi           = qos_desc.get_dyn_5qi();
    asn1_dyn_5qi.qos_prio_level                 = dyn_5qi.qos_prio_level.value();
    asn1_dyn_5qi.packet_delay_budget            = dyn_5qi.packet_delay_budget;
    asn1_dyn_5qi.packet_error_rate.per_scalar   = dyn_5qi.per.scalar;
    asn1_dyn_5qi.packet_error_rate.per_exponent = dyn_5qi.per.exponent;

    if (dyn_5qi.five_qi.has_value()) {
      asn1_dyn_5qi.five_qi_present = true;
      asn1_dyn_5qi.five_qi         = five_qi_to_uint(dyn_5qi.five_qi.value());
    }

    if (dyn_5qi.is_delay_critical.has_value()) {
      asn1_dyn_5qi.delay_crit_present = true;
      asn1_dyn_5qi.delay_crit.value   = dyn_5qi.is_delay_critical.value()
                                            ? asn1::f1ap::dyn_5qi_descriptor_s::delay_crit_opts::delay_crit
                                            : asn1::f1ap::dyn_5qi_descriptor_s::delay_crit_opts::non_delay_crit;
    }

    if (dyn_5qi.averaging_win.has_value()) {
      asn1_dyn_5qi.averaging_win_present = true;
      asn1_dyn_5qi.averaging_win         = dyn_5qi.averaging_win.value();
    }

    if (dyn_5qi.max_data_burst_volume.has_value()) {
      asn1_dyn_5qi.max_data_burst_volume_present = true;
      asn1_dyn_5qi.max_data_burst_volume         = dyn_5qi.max_data_burst_volume.value();
    }

  } else {
    auto&                         asn1_non_dyn_5qi = asn1_qos_characteristics.set_non_dyn_5qi();
    const non_dyn_5qi_descriptor& non_dyn_5qi      = qos_desc.get_nondyn_5qi();

    asn1_non_dyn_5qi.five_qi = five_qi_to_uint(non_dyn_5qi.five_qi);

    if (non_dyn_5qi.qos_prio_level.has_value()) {
      asn1_non_dyn_5qi.qos_prio_level_present = true;
      asn1_non_dyn_5qi.qos_prio_level         = non_dyn_5qi.qos_prio_level.value().value();
    }

    if (non_dyn_5qi.averaging_win.has_value()) {
      asn1_non_dyn_5qi.averaging_win_present = true;
      asn1_non_dyn_5qi.averaging_win         = non_dyn_5qi.averaging_win.value();
    }

    if (non_dyn_5qi.max_data_burst_volume.has_value()) {
      asn1_non_dyn_5qi.max_data_burst_volume_present = true;
      asn1_non_dyn_5qi.max_data_burst_volume         = non_dyn_5qi.max_data_burst_volume.value();
    }
  }

  return asn1_qos_characteristics;
}

static gbr_qos_flow_info_s gbr_qos_info_to_f1ap_asn1(const gbr_qos_flow_information& gbr_qos_info)
{
  gbr_qos_flow_info_s asn1_gbr_qos_info;
  asn1_gbr_qos_info.max_flow_bit_rate_dl        = gbr_qos_info.max_br_dl;
  asn1_gbr_qos_info.max_flow_bit_rate_ul        = gbr_qos_info.max_br_ul;
  asn1_gbr_qos_info.guaranteed_flow_bit_rate_dl = gbr_qos_info.gbr_dl;
  asn1_gbr_qos_info.guaranteed_flow_bit_rate_ul = gbr_qos_info.gbr_ul;

  if (gbr_qos_info.max_packet_loss_rate_dl.has_value()) {
    asn1_gbr_qos_info.max_packet_loss_rate_dl_present = true;
    asn1_gbr_qos_info.max_packet_loss_rate_dl         = gbr_qos_info.max_packet_loss_rate_dl.value();
  }

  if (gbr_qos_info.max_packet_loss_rate_ul.has_value()) {
    asn1_gbr_qos_info.max_packet_loss_rate_ul_present = true;
    asn1_gbr_qos_info.max_packet_loss_rate_ul         = gbr_qos_info.max_packet_loss_rate_ul.value();
  }

  return asn1_gbr_qos_info;
}

static qos_flow_level_qos_params_s
qos_flow_level_qos_params_to_f1ap_asn1(const qos_flow_level_qos_parameters& qos_params)
{
  qos_flow_level_qos_params_s asn1_qos_params;

  // Fill QoS characteristics.
  asn1_qos_params.qos_characteristics = qos_characteristics_to_f1ap_asn1(qos_params.qos_desc);

  // Fill alloc and retention prio.
  asn1_qos_params.ngra_nalloc_retention_prio.prio_level = qos_params.alloc_retention_prio.prio_level_arp.value();
  asn1_qos_params.ngra_nalloc_retention_prio.pre_emption_cap.value =
      qos_params.alloc_retention_prio.may_trigger_preemption
          ? asn1::f1ap::pre_emption_cap_opts::may_trigger_pre_emption
          : asn1::f1ap::pre_emption_cap_opts::shall_not_trigger_pre_emption;
  asn1_qos_params.ngra_nalloc_retention_prio.pre_emption_vulnerability.value =
      qos_params.alloc_retention_prio.is_preemptable ? asn1::f1ap::pre_emption_vulnerability_opts::pre_emptable
                                                     : asn1::f1ap::pre_emption_vulnerability_opts::not_pre_emptable;

  // Fill GBR QoS info.
  if (qos_params.gbr_qos_info.has_value()) {
    asn1_qos_params.gbr_qos_flow_info_present = true;
    asn1_qos_params.gbr_qos_flow_info         = gbr_qos_info_to_f1ap_asn1(qos_params.gbr_qos_info.value());
  }

  // Fill reflective QoS attribute.
  if (qos_params.reflective_qos_attribute_subject_to) {
    asn1_qos_params.reflective_qos_attribute_present = true;
    asn1_qos_params.reflective_qos_attribute.value =
        asn1::f1ap::qos_flow_level_qos_params_s::reflective_qos_attribute_opts::subject_to;
  }

  return asn1_qos_params;
}

/// \brief Convert QoS info to F1AP ASN.1.
/// \param[in] qos_info The common type QoS info.
/// \return The ASN.1 QoS info.
static qos_info_c qos_info_to_f1ap_asn1(const f1ap_drb_info& drb_info)
{
  qos_info_c asn1type;
  asn1type.set_choice_ext();
  auto& choice_ext = asn1type.choice_ext();
  choice_ext.load_info_obj(ASN1_F1AP_ID_DRB_INFO);
  auto& asn1_drb_info = choice_ext.value().drb_info();

  // Fill QoS flow level qos params.
  asn1_drb_info.drb_qos = qos_flow_level_qos_params_to_f1ap_asn1(drb_info.drb_qos);

  // Fill S-NSSAI.
  asn1_drb_info.snssai.sst.from_number(drb_info.s_nssai.sst.value());
  if (drb_info.s_nssai.sd.is_set()) {
    asn1_drb_info.snssai.sd_present = true;
    asn1_drb_info.snssai.sd.from_number(drb_info.s_nssai.sd.value());
  }

  // Fill notif ctrl.
  if (drb_info.notif_ctrl.has_value()) {
    asn1_drb_info.notif_ctrl_present = true;
    asn1_drb_info.notif_ctrl         = drb_info.notif_ctrl.value() == srsran::drb_notification_control::active
                                           ? notif_ctrl_opts::options::active
                                           : notif_ctrl_opts::options::not_active;
  }

  // Fill flows mapped to DRB list.
  for (const auto& qos_flow : drb_info.flows_mapped_to_drb_list) {
    asn1::f1ap::flows_mapped_to_drb_item_s asn1_flow;
    asn1_flow.qos_flow_id               = qos_flow_id_to_uint(qos_flow.qos_flow_id);
    asn1_flow.qos_flow_level_qos_params = qos_flow_level_qos_params_to_f1ap_asn1(qos_flow.qos_flow_level_qos_params);

    asn1_drb_info.flows_mapped_to_drb_list.push_back(asn1_flow);
  }

  return asn1type;
}

f1ap_srb_to_setup srsran::make_srb_to_setup(const asn1::f1ap::srbs_to_be_setup_item_s& asn1_type)
{
  f1ap_srb_to_setup srb_obj;
  srb_obj.srb_id = int_to_srb_id(asn1_type.srb_id);
  return srb_obj;
}

srbs_to_be_setup_list_l srsran::make_srb_to_setup_list(span<const f1ap_srb_to_setup> srbs)
{
  srbs_to_be_setup_list_l asn1_type(srbs.size());
  for (unsigned i = 0; i != srbs.size(); ++i) {
    asn1_type[i].load_info_obj(ASN1_F1AP_ID_SRBS_TO_BE_SETUP_ITEM);
    asn1_type[i].value().srbs_to_be_setup_item().srb_id = srb_id_to_uint(srbs[i].srb_id);
  }
  return asn1_type;
}

srbs_to_be_setup_mod_list_l srsran::make_srb_to_setupmod_list(span<const f1ap_srb_to_setup> srbs)
{
  srbs_to_be_setup_mod_list_l asn1_type(srbs.size());
  for (unsigned i = 0; i != srbs.size(); ++i) {
    asn1_type[i].load_info_obj(ASN1_F1AP_ID_SRBS_TO_BE_SETUP_MOD_ITEM);
    asn1_type[i].value().srbs_to_be_setup_mod_item().srb_id = srb_id_to_uint(srbs[i].srb_id);
  }
  return asn1_type;
}

srbs_setup_list_l srsran::make_srb_setup_list(span<const srb_id_t> srbs)
{
  srbs_setup_list_l asn1_list(srbs.size());
  for (unsigned i = 0; i != srbs.size(); ++i) {
    asn1_list[i].load_info_obj(ASN1_F1AP_ID_SRBS_SETUP_ITEM);
    asn1_list[i].value().srbs_setup_item().srb_id = srb_id_to_uint(srbs[i]);
    asn1_list[i].value().srbs_setup_item().lcid   = srb_id_to_lcid(srbs[i]);
  }
  return asn1_list;
}

srbs_setup_mod_list_l srsran::make_srb_setupmod_list(span<const srb_id_t> srbs)
{
  srbs_setup_mod_list_l asn1_list(srbs.size());
  for (unsigned i = 0; i != srbs.size(); ++i) {
    asn1_list[i].load_info_obj(ASN1_F1AP_ID_SRBS_SETUP_MOD_ITEM);
    asn1_list[i].value().srbs_setup_mod_item().srb_id = srb_id_to_uint(srbs[i]);
    asn1_list[i].value().srbs_setup_mod_item().lcid   = srb_id_to_lcid(srbs[i]);
  }
  return asn1_list;
}

template <typename T, typename ASN1Type>
void fill_common_srb_failed_to_setupmod(T& srb_item, const ASN1Type& asn1_type)
{
  srb_item.srb_id = int_to_srb_id(asn1_type.srb_id);
  if (asn1_type.cause_present) {
    srb_item.cause = asn1_to_cause(asn1_type.cause);
  }
}

f1ap_srb_failed_to_setup srsran::make_srb_failed_to_setupmod(const srbs_failed_to_be_setup_item_s& asn1_type)
{
  f1ap_srb_failed_to_setup srb_item;
  fill_common_srb_failed_to_setupmod(srb_item, asn1_type);
  return srb_item;
}

f1ap_srb_failed_to_setup srsran::make_srb_failed_to_setupmod(const srbs_failed_to_be_setup_mod_item_s& asn1_type)
{
  f1ap_srb_failed_to_setup srb_item;
  fill_common_srb_failed_to_setupmod(srb_item, asn1_type);
  return srb_item;
}

// Helper method common to DRBs-toBeSetup-Item and DRBs-toBeSetupMod-Item.
template <typename T, typename ASN1Type>
static void fill_drb_setup_common_fields(T& drb_obj, const ASN1Type& asn1_type)
{
  drb_obj.drb_id            = uint_to_drb_id(asn1_type.drb_id);
  drb_obj.mode              = get_rlc_mode(asn1_type.rlc_mode);
  drb_obj.uluptnl_info_list = make_ul_up_tnl_info_list(asn1_type.ul_up_tnl_info_to_be_setup_list);
  drb_obj.qos_info          = drb_info_from_f1ap_asn1(asn1_type.qos_info);
}

f1ap_drb_to_setup srsran::make_drb_to_setup(const asn1::f1ap::drbs_to_be_setup_item_s& drb_item)
{
  f1ap_drb_to_setup drb_obj;
  fill_drb_setup_common_fields(drb_obj, drb_item);
  if (drb_item.ie_exts_present) {
    drb_obj.pdcp_sn_len = pdcp_sn_size_from_f1ap_asn1(drb_item.ie_exts.dl_pdcp_sn_len);
  }
  return drb_obj;
}

f1ap_drb_to_setup srsran::make_drb_to_setup(const asn1::f1ap::drbs_to_be_setup_mod_item_s& drb_item)
{
  f1ap_drb_to_setup drb_obj;
  fill_drb_setup_common_fields(drb_obj, drb_item);
  if (drb_item.ie_exts_present) {
    if (drb_item.ie_exts.dl_pdcp_sn_len_present) {
      drb_obj.pdcp_sn_len = pdcp_sn_size_from_f1ap_asn1(drb_item.ie_exts.dl_pdcp_sn_len);
    }
  }
  return drb_obj;
}

f1ap_drb_to_modify srsran::make_drb_to_modify(const asn1::f1ap::drbs_to_be_modified_item_s& drb_item)
{
  f1ap_drb_to_modify drb_obj;
  drb_obj.drb_id            = uint_to_drb_id(drb_item.drb_id);
  drb_obj.uluptnl_info_list = make_ul_up_tnl_info_list(drb_item.ul_up_tnl_info_to_be_setup_list);
  return drb_obj;
}

drbs_to_be_setup_item_s srsran::make_drb_to_setup(const f1ap_drb_to_setup& drb_item)
{
  drbs_to_be_setup_item_s asn1type;
  asn1type.drb_id                          = drb_id_to_uint(drb_item.drb_id);
  asn1type.qos_info                        = qos_info_to_f1ap_asn1(drb_item.qos_info);
  asn1type.ul_up_tnl_info_to_be_setup_list = make_asn1_ul_up_tnl_info_list(drb_item.uluptnl_info_list);
  asn1type.rlc_mode                        = rlc_mode_to_f1ap_asn1(drb_item.mode);
  asn1type.ie_exts_present                 = true;
  asn1type.ie_exts.dl_pdcp_sn_len          = pdcp_sn_size_to_f1ap_asn1(drb_item.pdcp_sn_len);
  return asn1type;
}

drbs_to_be_setup_mod_item_s srsran::make_drb_to_setupmod(const f1ap_drb_to_setup& drb_item)
{
  drbs_to_be_setup_mod_item_s asn1type;
  asn1type.drb_id                          = drb_id_to_uint(drb_item.drb_id);
  asn1type.qos_info                        = qos_info_to_f1ap_asn1(drb_item.qos_info);
  asn1type.ul_up_tnl_info_to_be_setup_list = make_asn1_ul_up_tnl_info_list(drb_item.uluptnl_info_list);
  asn1type.rlc_mode                        = rlc_mode_to_f1ap_asn1(drb_item.mode);
  asn1type.ie_exts_present                 = true;
  asn1type.ie_exts.dl_pdcp_sn_len_present  = true;
  asn1type.ie_exts.dl_pdcp_sn_len          = pdcp_sn_size_to_f1ap_asn1(drb_item.pdcp_sn_len);
  return asn1type;
}

drbs_to_be_modified_item_s srsran::make_drb_to_mod(const f1ap_drb_to_modify& drb_item)
{
  drbs_to_be_modified_item_s asn1type;
  asn1type.drb_id                          = drb_id_to_uint(drb_item.drb_id);
  asn1type.ul_up_tnl_info_to_be_setup_list = make_asn1_ul_up_tnl_info_list(drb_item.uluptnl_info_list);
  return asn1type;
}

drbs_to_be_setup_list_l srsran::make_drb_to_setup_list(span<const f1ap_drb_to_setup> drb_list)
{
  drbs_to_be_setup_list_l asn1_list(drb_list.size());
  for (unsigned i = 0; i != drb_list.size(); ++i) {
    asn1_list[i].load_info_obj(ASN1_F1AP_ID_DRBS_TO_BE_SETUP_ITEM);
    asn1_list[i]->drbs_to_be_setup_item() = make_drb_to_setup(drb_list[i]);
  }
  return asn1_list;
}

drbs_to_be_setup_mod_list_l srsran::make_drb_to_setupmod_list(span<const f1ap_drb_to_setup> drb_list)
{
  drbs_to_be_setup_mod_list_l asn1_list(drb_list.size());
  for (unsigned i = 0; i != drb_list.size(); ++i) {
    asn1_list[i].load_info_obj(ASN1_F1AP_ID_DRBS_TO_BE_SETUP_MOD_ITEM);
    asn1_list[i]->drbs_to_be_setup_mod_item() = make_drb_to_setupmod(drb_list[i]);
  }
  return asn1_list;
}

drbs_to_be_modified_list_l srsran::make_drb_to_modify_list(span<const f1ap_drb_to_modify> drb_list)
{
  drbs_to_be_modified_list_l asn1_list(drb_list.size());
  for (unsigned i = 0; i != drb_list.size(); ++i) {
    asn1_list[i].load_info_obj(ASN1_F1AP_ID_DRBS_TO_BE_MODIFIED_ITEM);
    asn1_list[i]->drbs_to_be_modified_item() = make_drb_to_mod(drb_list[i]);
  }
  return asn1_list;
}

asn1::f1ap::drbs_setup_list_l srsran::make_drbs_setup_list(span<const f1ap_drb_setupmod> drbs)
{
  asn1::f1ap::drbs_setup_list_l list(drbs.size());
  for (unsigned i = 0; i != drbs.size(); ++i) {
    list[i].load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_ITEM);
    fill_drb_setup_mod_common(list[i]->drbs_setup_item(), drbs[i]);
  }
  return list;
}

asn1::f1ap::drbs_setup_mod_list_l srsran::make_drbs_setup_mod_list(span<const f1ap_drb_setupmod> drbs)
{
  asn1::f1ap::drbs_setup_mod_list_l list(drbs.size());
  for (unsigned i = 0; i != drbs.size(); ++i) {
    list[i].load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_MOD_ITEM);
    fill_drb_setup_mod_common(list[i]->drbs_setup_mod_item(), drbs[i]);
  }
  return list;
}

asn1::f1ap::drbs_modified_list_l srsran::make_drbs_modified_list(span<const f1ap_drb_setupmod> drbs)
{
  asn1::f1ap::drbs_modified_list_l list(drbs.size());
  for (unsigned i = 0; i != drbs.size(); ++i) {
    list[i].load_info_obj(ASN1_F1AP_ID_DRBS_MODIFIED_ITEM);
    fill_drb_setup_mod_common(list[i]->drbs_modified_item(), drbs[i]);
    if (drbs[i].rlc_reestablished) {
      list[i]->drbs_modified_item().ie_exts_present                    = true;
      list[i]->drbs_modified_item().ie_exts.rlc_status_present         = true;
      list[i]->drbs_modified_item().ie_exts.rlc_status.reest_ind.value = asn1::f1ap::reest_ind_opts::reestablished;
    }
  }
  return list;
}

template <typename T, typename ASN1Type>
void fill_common_drb_setup(T& out, const ASN1Type& asn1type)
{
  out.drb_id = uint_to_drb_id(asn1type.drb_id);
  if (asn1type.lcid_present) {
    out.lcid = uint_to_lcid(asn1type.lcid);
  }
  out.dluptnl_info_list = make_dl_up_tnl_info_list(asn1type.dl_up_tnl_info_to_be_setup_list);
}

f1ap_drb_setupmod srsran::make_drb_setupmod(const asn1::f1ap::drbs_setup_item_s& drb)
{
  f1ap_drb_setupmod drb_obj;
  fill_common_drb_setup(drb_obj, drb);
  return drb_obj;
}

f1ap_drb_setupmod srsran::make_drb_setupmod(const asn1::f1ap::drbs_setup_mod_item_s& drb)
{
  f1ap_drb_setupmod drb_obj;
  fill_common_drb_setup(drb_obj, drb);
  return drb_obj;
}

f1ap_drb_setupmod srsran::make_drb_setupmod(const asn1::f1ap::drbs_modified_item_s& drb)
{
  f1ap_drb_setupmod drb_obj;
  fill_common_drb_setup(drb_obj, drb);
  return drb_obj;
}

template <typename ASN1Type>
void fill_drb_failed_item(ASN1Type& asn1obj, const f1ap_drb_failed_to_setupmod& drb_obj)
{
  asn1obj.drb_id        = drb_id_to_uint(drb_obj.drb_id);
  asn1obj.cause_present = drb_obj.cause.has_value();
  if (asn1obj.cause_present) {
    asn1obj.cause = cause_to_asn1(drb_obj.cause.value());
  }
}

asn1::f1ap::drbs_failed_to_be_setup_list_l
srsran::make_drbs_failed_to_be_setup_list(span<const f1ap_drb_failed_to_setupmod> failed_drbs)
{
  asn1::f1ap::drbs_failed_to_be_setup_list_l list(failed_drbs.size());
  for (unsigned i = 0; i != failed_drbs.size(); ++i) {
    list[i].load_info_obj(ASN1_F1AP_ID_DRBS_FAILED_TO_BE_SETUP_ITEM);
    fill_drb_failed_item(list[i]->drbs_failed_to_be_setup_item(), failed_drbs[i]);
  }
  return list;
}

asn1::f1ap::drbs_failed_to_be_setup_mod_list_l
srsran::make_drbs_failed_to_be_setup_mod_list(span<const f1ap_drb_failed_to_setupmod> failed_drbs)
{
  asn1::f1ap::drbs_failed_to_be_setup_mod_list_l list(failed_drbs.size());
  for (unsigned i = 0; i != failed_drbs.size(); ++i) {
    list[i].load_info_obj(ASN1_F1AP_ID_DRBS_FAILED_TO_BE_SETUP_MOD_ITEM);
    fill_drb_failed_item(list[i]->drbs_failed_to_be_setup_mod_item(), failed_drbs[i]);
  }
  return list;
}

asn1::f1ap::drbs_failed_to_be_modified_list_l
srsran::make_drbs_failed_to_be_modified_list(span<const f1ap_drb_failed_to_setupmod> failed_drbs)
{
  asn1::f1ap::drbs_failed_to_be_modified_list_l list(failed_drbs.size());
  for (unsigned i = 0; i != failed_drbs.size(); ++i) {
    list[i].load_info_obj(ASN1_F1AP_ID_DRBS_FAILED_TO_BE_MODIFIED_ITEM);
    fill_drb_failed_item(list[i]->drbs_failed_to_be_modified_item(), failed_drbs[i]);
  }
  return list;
}

asn1::f1ap::serving_cell_mo_encoded_in_cgc_list_l
srsran::make_serving_cell_mo_encoded_in_cgc_list(span<const uint8_t> serving_cell_mos)
{
  asn1::f1ap::serving_cell_mo_encoded_in_cgc_list_l list(serving_cell_mos.size());
  for (unsigned i = 0; i != serving_cell_mos.size(); ++i) {
    list[i].serving_cell_mo = serving_cell_mos[i];
  }
  return list;
}

template <typename T, typename ASN1Type>
static void fill_failed_drb(T& drb_item, const ASN1Type& asn1_type)
{
  drb_item.drb_id = uint_to_drb_id(asn1_type.drb_id);
  if (asn1_type.cause_present) {
    drb_item.cause = asn1_to_cause(asn1_type.cause);
  }
}

f1ap_drb_failed_to_setupmod
srsran::make_drb_failed_to_setupmod(const asn1::f1ap::drbs_failed_to_be_setup_item_s& asn1_type)
{
  f1ap_drb_failed_to_setupmod drb_item;
  fill_failed_drb(drb_item, asn1_type);
  return drb_item;
}
f1ap_drb_failed_to_setupmod
srsran::make_drb_failed_to_setupmod(const asn1::f1ap::drbs_failed_to_be_setup_mod_item_s& asn1_type)
{
  f1ap_drb_failed_to_setupmod drb_item;
  fill_failed_drb(drb_item, asn1_type);
  return drb_item;
}
f1ap_drb_failed_to_setupmod
srsran::make_drb_failed_to_setupmod(const asn1::f1ap::drbs_failed_to_be_modified_item_s& asn1_type)
{
  f1ap_drb_failed_to_setupmod drb_item;
  fill_failed_drb(drb_item, asn1_type);
  return drb_item;
}
