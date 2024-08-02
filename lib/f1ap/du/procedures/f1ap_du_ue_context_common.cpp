/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_ue_context_common.h"
#include "srsran/asn1/f1ap/common.h"

using namespace srsran;
using namespace srs_du;

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

template <typename ASN1Type>
static void fill_common_drb_config_request_fields(f1ap_drb_setup_request& drb_obj, const ASN1Type& drb_item)
{
  drb_obj.drb_id            = static_cast<drb_id_t>(drb_item.drb_id);
  drb_obj.uluptnl_info_list = make_ul_up_tnl_info_list(drb_item.ul_up_tnl_info_to_be_setup_list);

  // TODO: Handle Dynamic 5QI.
  const auto& asn1_drbinfo   = drb_item.qos_info.choice_ext().value().drb_info();
  drb_obj.five_qi            = uint_to_five_qi(asn1_drbinfo.drb_qos.qos_characteristics.non_dyn_5qi().five_qi);
  drb_obj.arp_priority_level = asn1_drbinfo.drb_qos.ngra_nalloc_retention_prio.prio_level;
  drb_obj.s_nssai.sst        = asn1_drbinfo.snssai.sst.to_number();
  if (asn1_drbinfo.snssai.sd_present) {
    drb_obj.s_nssai.sd = drb_item.qos_info.choice_ext().value().drb_info().snssai.sd.to_number();
  }
  // TODO: Do not populate gbr_flow_info for non-GBR flows.
  if (asn1_drbinfo.drb_qos.gbr_qos_flow_info_present) {
    drb_obj.gbr_flow_info.emplace();
    auto& gbr                      = drb_obj.gbr_flow_info.value();
    gbr.max_flow_dl_bitrate        = asn1_drbinfo.drb_qos.gbr_qos_flow_info.max_flow_bit_rate_dl;
    gbr.max_flow_ul_bitrate        = asn1_drbinfo.drb_qos.gbr_qos_flow_info.max_flow_bit_rate_ul;
    gbr.guaranteed_flow_dl_bitrate = asn1_drbinfo.drb_qos.gbr_qos_flow_info.guaranteed_flow_bit_rate_dl;
    gbr.guaranteed_flow_ul_bitrate = asn1_drbinfo.drb_qos.gbr_qos_flow_info.guaranteed_flow_bit_rate_ul;
    if (asn1_drbinfo.drb_qos.gbr_qos_flow_info.max_packet_loss_rate_dl_present) {
      gbr.max_packet_loss_rate_dl.emplace(asn1_drbinfo.drb_qos.gbr_qos_flow_info.max_packet_loss_rate_dl);
    }
    if (asn1_drbinfo.drb_qos.gbr_qos_flow_info.max_packet_loss_rate_ul_present) {
      gbr.max_packet_loss_rate_dl.emplace(asn1_drbinfo.drb_qos.gbr_qos_flow_info.max_packet_loss_rate_ul);
    }
  }
}

f1ap_drb_setup_request srsran::srs_du::make_drb_config_request(const asn1::f1ap::drbs_to_be_setup_item_s& drb_item)
{
  f1ap_drb_setup_request drb_obj;
  fill_common_drb_config_request_fields(drb_obj, drb_item);

  drb_obj.mode = get_rlc_mode(drb_item.rlc_mode);

  if (drb_item.ie_exts_present) {
    drb_obj.pdcp_sn_len = pdcp_sn_size_from_f1ap_asn1(drb_item.ie_exts.dl_pdcp_sn_len);
  }

  return drb_obj;
}

f1ap_drb_setup_request srsran::srs_du::make_drb_config_request(const asn1::f1ap::drbs_to_be_setup_mod_item_s& drb_item)
{
  f1ap_drb_setup_request drb_obj;
  fill_common_drb_config_request_fields(drb_obj, drb_item);

  drb_obj.mode = get_rlc_mode(drb_item.rlc_mode);

  if (drb_item.ie_exts_present) {
    if (drb_item.ie_exts.dl_pdcp_sn_len_present) {
      drb_obj.pdcp_sn_len = pdcp_sn_size_from_f1ap_asn1(drb_item.ie_exts.dl_pdcp_sn_len);
    }
  }

  return drb_obj;
}
