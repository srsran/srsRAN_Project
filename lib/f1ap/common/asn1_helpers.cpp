/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "asn1_helpers.h"
#include "../cu_cp/f1ap_asn1_converters.h"
#include "srsran/asn1/f1ap/common.h"

using namespace srsran;

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

pdcp_sn_size srsran::pdcp_sn_size_from_f1ap_asn1(const asn1::f1ap::pdcp_sn_len_e& asn1_pdcp_sn_size)
{
  switch (asn1_pdcp_sn_size) {
    case asn1::f1ap::pdcp_sn_len_opts::twelve_bits:
      return pdcp_sn_size::size12bits;
    case asn1::f1ap::pdcp_sn_len_opts::eighteen_bits:
      return pdcp_sn_size::size18bits;
    default:
      return pdcp_sn_size::invalid;
  }
}

asn1::f1ap::pdcp_sn_len_e srsran::pdcp_sn_size_to_f1ap_asn1(pdcp_sn_size sn_size)
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

// helper function to fill asn1 DRBs-SetupMod and DRBs-Modified types.
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

std::vector<up_transport_layer_info>
srsran::make_ul_up_tnl_info_list(const asn1::f1ap::ul_up_tnl_info_to_be_setup_list_l& asn1_list)
{
  std::vector<up_transport_layer_info> uluptnl_info_list;
  uluptnl_info_list.reserve(asn1_list.size());
  for (const auto& tnl_info : asn1_list) {
    uluptnl_info_list.push_back(asn1_to_up_transport_layer_info(tnl_info.ul_up_tnl_info));
  }
  return uluptnl_info_list;
}

asn1::f1ap::ul_up_tnl_info_to_be_setup_list_l
srsran::make_asn1_ul_up_tnl_info_list(span<const up_transport_layer_info> list)
{
  asn1::f1ap::ul_up_tnl_info_to_be_setup_list_l ret;
  ret.resize(list.size());
  for (unsigned i = 0; i != list.size(); ++i) {
    up_transport_layer_info_to_asn1(ret[i].ul_up_tnl_info, list[i]);
  }
  return ret;
}

f1ap_drb_to_modify srsran::make_drb_to_modify(const asn1::f1ap::drbs_to_be_modified_item_s& drb_item)
{
  f1ap_drb_to_modify drb_obj;
  drb_obj.drb_id            = static_cast<drb_id_t>(drb_item.drb_id);
  drb_obj.uluptnl_info_list = make_ul_up_tnl_info_list(drb_item.ul_up_tnl_info_to_be_setup_list);
  return drb_obj;
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
  }
  return list;
}

template <typename ASN1Type>
void fill_drb_failed_item(ASN1Type& asn1obj, const f1ap_drb_failed_to_setupmod& drb_obj)
{
  asn1obj.drb_id        = drb_id_to_uint(drb_obj.drb_id);
  asn1obj.cause_present = drb_obj.cause.has_value();
  if (asn1obj.cause_present) {
    asn1obj.cause = srs_cu_cp::cause_to_asn1(drb_obj.cause.value());
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
