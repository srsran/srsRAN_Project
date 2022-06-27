/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1c_configuration_helpers.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"

using namespace srsgnb;

byte_buffer srsgnb::srs_du::make_asn1_rrc_cell_mib_buffer(const du_cell_config& du_cfg)
{
  using namespace asn1::rrc_nr;

  mib_s mib;
  switch (du_cfg.scs_common) {
    case srsgnb::subcarrier_spacing::kHz15:
    case srsgnb::subcarrier_spacing::kHz60:
      mib.sub_carrier_spacing_common.value = mib_s::sub_carrier_spacing_common_opts::scs15or60;
      break;
    case srsgnb::subcarrier_spacing::kHz30:
    case srsgnb::subcarrier_spacing::kHz120:
      mib.sub_carrier_spacing_common.value = mib_s::sub_carrier_spacing_common_opts::scs30or120;
      break;
    default:
      srsran_terminate("Invalid SCS common");
  }
  mib.ssb_subcarrier_offset            = du_cfg.ssb_cfg.ssb_subcarrier_offset;
  mib.dmrs_type_a_position.value       = mib_s::dmrs_type_a_position_opts::pos2; // TODO.
  mib.pdcch_cfg_sib1.coreset_zero      = du_cfg.coreset0_id;
  mib.pdcch_cfg_sib1.search_space_zero = du_cfg.searchspace0_id;
  mib.cell_barred.value                = mib_s::cell_barred_opts::not_barred;
  mib.intra_freq_resel.value           = mib_s::intra_freq_resel_opts::not_allowed;

  byte_buffer       buf;
  asn1::bit_ref     bref{buf};
  asn1::SRSASN_CODE ret = mib.pack(bref);
  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack MIB");
  return buf;
}

static asn1::rrc_nr::subcarrier_spacing_e get_asn1_scs(subcarrier_spacing scs)
{
  return asn1::rrc_nr::subcarrier_spacing_e{static_cast<asn1::rrc_nr::subcarrier_spacing_opts::options>(scs)};
}

static asn1::rrc_nr::coreset_s make_asn1_rrc_coreset(const coreset_configuration& cfg)
{
  using namespace asn1::rrc_nr;
  coreset_s cs;
  cs.coreset_id = cfg.id;
  cs.freq_domain_res.from_number(cfg.freq_domain_resources.to_uint64());
  cs.dur = cfg.duration;
  // TODO: Remaining
  return cs;
}

static asn1::rrc_nr::dl_cfg_common_sib_s make_asn1_rrc_dl_config_common(const dl_config_common& cfg)
{
  using namespace asn1::rrc_nr;
  dl_cfg_common_sib_s out;
  // generic params
  out.init_dl_bwp.generic_params.cp_present               = cfg.init_dl_bwp.generic_params.cp_extended;
  out.init_dl_bwp.generic_params.subcarrier_spacing.value = get_asn1_scs(cfg.init_dl_bwp.generic_params.scs);
  // See TS 38.331, BWP.locationAndBandwidth and TS 38.213 clause 12.
  out.init_dl_bwp.generic_params.location_and_bw =
      sliv_from_s_and_l(275, cfg.init_dl_bwp.generic_params.crbs.start(), cfg.init_dl_bwp.generic_params.crbs.length());
  // PDCCH-ConfigCommon.
  out.init_dl_bwp.pdcch_cfg_common_present = true;
  pdcch_cfg_common_s& pdcch                = out.init_dl_bwp.pdcch_cfg_common.set_setup();
  pdcch.coreset_zero_present               = false; // Sent by MIB.
  pdcch.common_coreset_present             = cfg.init_dl_bwp.pdcch_common.common_coreset.has_value();
  if (pdcch.common_coreset_present) {
    pdcch.common_coreset = make_asn1_rrc_coreset(cfg.init_dl_bwp.pdcch_common.common_coreset.value());
  }
  pdcch.search_space_zero_present           = false; // Sent by MIB.
  pdcch.search_space_sib1_present           = true;
  pdcch.search_space_sib1                   = cfg.init_dl_bwp.pdcch_common.sib1_search_space_id;
  pdcch.search_space_other_sys_info_present = false;
  pdcch.paging_search_space_present         = true;
  pdcch.paging_search_space                 = cfg.init_dl_bwp.pdcch_common.paging_search_space_id;
  pdcch.ra_search_space_present             = true;
  pdcch.ra_search_space                     = (unsigned)cfg.init_dl_bwp.pdcch_common.ra_search_space_id;
  // PDSCH-ConfigCommon.
  out.init_dl_bwp.pdsch_cfg_common_present = true;
  pdsch_cfg_common_s& pdsch                = out.init_dl_bwp.pdsch_cfg_common.set_setup();
  pdsch.pdsch_time_domain_alloc_list.resize(cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.size());
  for (unsigned i = 0; i != pdsch.pdsch_time_domain_alloc_list.size(); ++i) {
    pdsch.pdsch_time_domain_alloc_list[i].k0_present = cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[i].k0 != 0;
    if (pdsch.pdsch_time_domain_alloc_list[i].k0_present) {
      pdsch.pdsch_time_domain_alloc_list[i].k0 = cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[i].k0;
    }
    pdsch.pdsch_time_domain_alloc_list[i].map_type.value =
        cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[i].map_type == pdsch_mapping_type::typeA
            ? pdsch_time_domain_res_alloc_s::map_type_opts::type_a
            : pdsch_time_domain_res_alloc_s::map_type_opts::type_b;
    pdsch.pdsch_time_domain_alloc_list[i].start_symbol_and_len =
        sliv_from_s_and_l(NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                          cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[i].symbols.start(),
                          cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[i].symbols.length());
  }
  // TODO: Fill remaining fields.

  return out;
}

static asn1::rrc_nr::serving_cell_cfg_common_sib_s make_asn1_rrc_cell_serving_cell_common(const du_cell_config& du_cfg)
{
  using namespace asn1::rrc_nr;
  serving_cell_cfg_common_sib_s cell;
  cell.dl_cfg_common = make_asn1_rrc_dl_config_common(du_cfg.dl_cfg_common);
  // TODO: Fill remaining fields.
  return cell;
}

byte_buffer srsgnb::srs_du::make_asn1_rrc_cell_sib1_buffer(const du_cell_config& du_cfg)
{
  using namespace asn1::rrc_nr;

  sib1_s sib1;
  //  bool                          cell_sel_info_present           = false;
  //  bool                          conn_est_fail_ctrl_present      = false;
  //  bool                          si_sched_info_present           = false;
  //  bool                          serving_cell_cfg_common_present = false;
  //  bool                          ims_emergency_support_present   = false;
  //  bool                          ecall_over_ims_support_present  = false;
  //  bool                          ue_timers_and_consts_present    = false;
  //  bool                          uac_barr_info_present           = false;
  //  bool                          use_full_resume_id_present      = false;
  //  bool                          non_crit_ext_present            = false;
  //  cell_sel_info_s_              cell_sel_info;
  //  cell_access_related_info_s    cell_access_related_info;
  //  conn_est_fail_ctrl_s          conn_est_fail_ctrl;
  //  si_sched_info_s               si_sched_info;
  //  serving_cell_cfg_common_sib_s serving_cell_cfg_common;
  //  ue_timers_and_consts_s        ue_timers_and_consts;
  //  uac_barr_info_s_              uac_barr_info;
  // TODO: Fill remaining fields.
  sib1.serving_cell_cfg_common_present = true;
  sib1.serving_cell_cfg_common         = make_asn1_rrc_cell_serving_cell_common(du_cfg);

  byte_buffer   buf;
  asn1::bit_ref bref{buf};
  //  asn1::SRSASN_CODE ret = sib1.pack(bref);
  //  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack MIB");
  return buf;
}

void srsgnb::srs_du::fill_asn1_f1_setup_request(asn1::f1ap::f1_setup_request_s& request,
                                                const du_setup_params&          setup_params,
                                                span<const du_cell_config*>     cells_to_add)
{
  byte_buffer buf;
  // TODO: Add other inputs and set values accordingly

  request->gnb_du_id.value = setup_params.gnb_du_id;
  request->gnb_du_name.value.from_string(setup_params.gnb_du_name);
  request->gnb_du_rrc_version.value.latest_rrc_version.from_number(setup_params.rrc_version);

  request->gnb_du_served_cells_list_present = true;
  for (const du_cell_config* cell_cfg : cells_to_add) {
    // Add Cell in list of served cells.
    request->gnb_du_served_cells_list->push_back({});
    request->gnb_du_served_cells_list->back().load_info_obj(ASN1_F1AP_ID_G_NB_DU_SERVED_CELLS_LIST);
    asn1::f1ap::gnb_du_served_cells_item_s& f1_cell =
        request->gnb_du_served_cells_list->back()->gnb_du_served_cells_item();

    // Fill Served Cell Information.
    f1_cell.served_cell_info.nrpci = cell_cfg->pci;
    // TODO: Add remaining fields.

    // Add System Information related to the cell.
    f1_cell.gnb_du_sys_info_present = true;
    buf                             = make_asn1_rrc_cell_mib_buffer(*cell_cfg);
    f1_cell.gnb_du_sys_info.mib_msg.resize(buf.length());
    std::copy(buf.begin(), buf.end(), f1_cell.gnb_du_sys_info.mib_msg.begin());
    buf = make_asn1_rrc_cell_sib1_buffer(*cell_cfg);
    f1_cell.gnb_du_sys_info.mib_msg.resize(buf.length());
    std::copy(buf.begin(), buf.end(), f1_cell.gnb_du_sys_info.sib1_msg.begin());
  }
}
