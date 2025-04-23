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

#pragma once

#include "../asn1_helpers.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_ue_context_update.h"
#include "srsran/ran/cause/f1ap_cause.h"
#include "srsran/ran/nr_cgi.h"
#include <string>
#include <variant>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

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

/// \brief Convert cu to du rrc container to F1AP ASN.1.
/// \param[out] asn1_cu_to_du_rrc_info The ASN.1 struct to store the result.
/// \param[in] cu_to_du_rrc_info The cu to du rrc container common type struct.
inline void cu_to_du_rrc_info_to_asn1(asn1::f1ap::cu_to_du_rrc_info_s& asn1_cu_to_du_rrc_info,
                                      const f1ap_cu_to_du_rrc_info&    cu_to_du_rrc_info)
{
  // cg cfg info
  asn1_cu_to_du_rrc_info.cg_cfg_info = cu_to_du_rrc_info.cg_cfg_info.copy();

  // ue cap rat container list
  asn1_cu_to_du_rrc_info.ue_cap_rat_container_list = cu_to_du_rrc_info.ue_cap_rat_container_list.copy();

  // meas cfg
  asn1_cu_to_du_rrc_info.meas_cfg = cu_to_du_rrc_info.meas_cfg.copy();

  if (cu_to_du_rrc_info.ie_exts.has_value()) {
    asn1_cu_to_du_rrc_info.ie_exts_present = true;

    // ho prep info
    if (cu_to_du_rrc_info.ie_exts.value().ho_prep_info.has_value()) {
      asn1_cu_to_du_rrc_info.ie_exts.ho_prep_info_present = true;
      asn1_cu_to_du_rrc_info.ie_exts.ho_prep_info = cu_to_du_rrc_info.ie_exts.value().ho_prep_info.value().copy();
    }

    // TODO: Add missing optional values
    // cell group cfg
    // meas timing cfg
    // ue assis info
    // cg cfg
    // ue assis info eutra
    // location meas info
    // mu si m gap cfg
    // need for gaps info nr
    // need for gap ncsg info nr
    // need for gap ncsg info eutra
    // cfg restrict info daps
  }
}

/// \brief Convert scell to be setup/setup mod item to F1AP ASN.1.
/// \param[out] asn1_scell_to_be_setup_mod_item The ASN.1 struct to store the result.
/// \param[in] scell_to_be_setup_mod_item The scell to be setup/setup item mod common type struct.
template <typename template_asn1_item>
inline void f1ap_scell_to_be_setup_mod_item_to_asn1(template_asn1_item& asn1_scell_to_be_setup_mod_item,
                                                    const f1ap_scell_to_be_setup_mod_item& scell_to_be_setup_mod_item)
{
  // scell id
  asn1_scell_to_be_setup_mod_item.scell_id.nr_cell_id.from_number(scell_to_be_setup_mod_item.scell_id.nci.value());
  asn1_scell_to_be_setup_mod_item.scell_id.plmn_id = scell_to_be_setup_mod_item.scell_id.plmn_id.to_bytes();

  // scell idx
  asn1_scell_to_be_setup_mod_item.scell_idx = scell_to_be_setup_mod_item.scell_idx;

  // scell ul cfg
  if (scell_to_be_setup_mod_item.scell_ul_cfg.has_value()) {
    asn1_scell_to_be_setup_mod_item.scell_ul_cfg_present = true;
    asn1_scell_to_be_setup_mod_item.scell_ul_cfg = cell_ul_cfg_to_asn1(scell_to_be_setup_mod_item.scell_ul_cfg.value());
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
