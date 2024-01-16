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

#include "rrc_du_impl.h"
#include "../ran/gnb_format.h"
#include "ue/rrc_measurement_types_asn1_converters.h"
#include "srsran/asn1/rrc_nr/rrc_nr.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/nr_cgi_helpers.h"

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_du_impl::rrc_du_impl(const rrc_cfg_t&                 cfg_,
                         rrc_ue_du_processor_notifier&    rrc_ue_du_proc_notif_,
                         rrc_ue_nas_notifier&             nas_notif_,
                         rrc_ue_control_notifier&         ngap_ctrl_notif_,
                         rrc_ue_reestablishment_notifier& cu_cp_notif_,
                         cell_meas_manager&               cell_meas_mng_) :
  cfg(cfg_),
  rrc_ue_du_proc_notifier(rrc_ue_du_proc_notif_),
  nas_notifier(nas_notif_),
  ngap_ctrl_notifier(ngap_ctrl_notif_),
  cu_cp_notifier(cu_cp_notif_),
  cell_meas_mng(cell_meas_mng_),
  logger(srslog::fetch_basic_logger("RRC", false))
{
  for (const auto& qos : cfg.drb_config) {
    logger.debug("5QI DRB config: {} {}", qos.first, qos.second.pdcp);
  }
}

bool rrc_du_impl::handle_served_cell_list(const std::vector<cu_cp_du_served_cells_item>& served_cell_list)
{
  for (const auto& served_cell : served_cell_list) {
    rrc_cell_info cell_info;

    // TODO: which freq band to use here?
    if (served_cell.served_cell_info.nr_mode_info.fdd.has_value()) {
      cell_info.band = (nr_band)served_cell.served_cell_info.nr_mode_info.fdd.value()
                           .dl_nr_freq_info.freq_band_list_nr.begin()
                           ->freq_band_ind_nr;
    } else {
      cell_info.band = (nr_band)served_cell.served_cell_info.nr_mode_info.tdd.value()
                           .nr_freq_info.freq_band_list_nr.begin()
                           ->freq_band_ind_nr;
    }

    asn1::cbit_ref                  bref_meas{served_cell.served_cell_info.meas_timing_cfg};
    asn1::rrc_nr::meas_timing_cfg_s asn1_meas_timing_cfg;
    if (asn1_meas_timing_cfg.unpack(bref_meas) != asn1::SRSASN_SUCCESS) {
      logger.error("Failed to unpack Measurement Timing Config container.");
      return false;
    }
    if (asn1_meas_timing_cfg.crit_exts.type() != meas_timing_cfg_s::crit_exts_c_::types_opts::c1 ||
        asn1_meas_timing_cfg.crit_exts.c1().type() !=
            meas_timing_cfg_s::crit_exts_c_::c1_c_::types_opts::meas_timing_conf ||
        asn1_meas_timing_cfg.crit_exts.c1().meas_timing_conf().meas_timing.size() == 0) {
      logger.error("Invalid Measurement Timing Config container.");
      return false;
    }

    for (const auto& asn1_meas_timing : asn1_meas_timing_cfg.crit_exts.c1().meas_timing_conf().meas_timing) {
      cell_info.meas_timings.push_back(asn1_to_meas_timing(asn1_meas_timing));
    }

    cell_info_db.emplace(served_cell.served_cell_info.nr_cgi.nci, cell_info);

    // fill cell meas config
    serving_cell_meas_config meas_cfg;
    meas_cfg.nci  = served_cell.served_cell_info.nr_cgi.nci;
    meas_cfg.band = cell_info.band;
    // TODO: which meas timing to use here?
    meas_cfg.ssb_mtc   = cell_info.meas_timings.begin()->freq_and_timing.value().ssb_meas_timing_cfg;
    meas_cfg.ssb_arfcn = cell_info.meas_timings.begin()->freq_and_timing.value().carrier_freq;
    meas_cfg.ssb_scs   = cell_info.meas_timings.begin()->freq_and_timing.value().ssb_subcarrier_spacing;

    // Update cell config in cell meas manager
    logger.debug("Updating cell={} with band={} ssb_arfcn={} ssb_scs={}",
                 meas_cfg.nci,
                 nr_band_to_uint(meas_cfg.band.value()),
                 meas_cfg.ssb_arfcn.value(),
                 to_string(meas_cfg.ssb_scs.value()));
    cell_meas_mng.update_cell_config(served_cell.served_cell_info.nr_cgi.nci, meas_cfg);
  }

  return true;
}

rrc_ue_interface* rrc_du_impl::add_ue(up_resource_manager& resource_mng, const rrc_ue_creation_message& msg)
{
  // Unpack DU to CU container
  asn1::rrc_nr::cell_group_cfg_s cell_group_cfg;
  asn1::cbit_ref                 bref_cell({msg.du_to_cu_container.begin(), msg.du_to_cu_container.end()});
  if (cell_group_cfg.unpack(bref_cell) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to unpack DU to CU RRC container - aborting user creation");
    return nullptr;
  }

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    cell_group_cfg.to_json(js);
    logger.debug("Containerized MasterCellGroup: {}", js.to_string());
  }

  // create UE object
  ue_index_t   ue_index                 = msg.ue_index;
  rrc_ue_cfg_t ue_cfg                   = {};
  ue_cfg.force_reestablishment_fallback = cfg.force_reestablishment_fallback;
  ue_cfg.rrc_procedure_timeout_ms       = cfg.rrc_procedure_timeout_ms;
  ue_cfg.int_algo_pref_list             = cfg.int_algo_pref_list;
  ue_cfg.enc_algo_pref_list             = cfg.enc_algo_pref_list;
  ue_cfg.meas_timings                   = cell_info_db.at(msg.cell.cgi.nci).meas_timings;

  // Copy RRC cell and add SSB ARFCN.
  rrc_cell_context rrc_cell = msg.cell;
  rrc_cell.ssb_arfcn        = ue_cfg.meas_timings.front().freq_and_timing.value().carrier_freq;

  auto res = ue_db.emplace(ue_index,
                           std::make_unique<rrc_ue_impl>(resource_mng,
                                                         rrc_ue_du_proc_notifier,
                                                         *msg.f1ap_pdu_notifier,
                                                         nas_notifier,
                                                         ngap_ctrl_notifier,
                                                         cu_cp_notifier,
                                                         cell_meas_mng,
                                                         msg.ue_index,
                                                         msg.c_rnti,
                                                         rrc_cell,
                                                         ue_cfg,
                                                         msg.du_to_cu_container.copy(),
                                                         *msg.ue_task_sched,
                                                         reject_users,
                                                         msg.rrc_context));

  if (res.second) {
    auto& u = ue_db.at(ue_index);
    return u.get();
  }
  return nullptr;
}

void rrc_du_impl::remove_ue(ue_index_t ue_index)
{
  ue_db.erase(ue_index);
}

void rrc_du_impl::release_ues()
{
  // release all UEs connected to this RRC entity
}

bool rrc_du_impl::is_rrc_connect_allowed()
{
  return !reject_users;
}

void rrc_du_impl::handle_amf_connection()
{
  reject_users = false;
}

void rrc_du_impl::handle_amf_connection_drop()
{
  reject_users = true;
}
