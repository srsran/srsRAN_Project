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

#include "rrc_du_impl.h"
#include "ue/rrc_measurement_types_asn1_converters.h"
#include "ue/rrc_ue_helpers.h"
#include "srsran/adt/expected.h"
#include "srsran/asn1/rrc_nr/cell_group_config.h"
#include "srsran/asn1/rrc_nr/dl_ccch_msg.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/plmn_identity.h"

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_du_impl::rrc_du_impl(const rrc_cfg_t& cfg_) : cfg(cfg_), logger(srslog::fetch_basic_logger("RRC", false))
{
  for (const auto& qos : cfg.drb_config) {
    logger.debug("5QI DRB config: {} {}", qos.first, qos.second.pdcp);
  }
}

std::map<nr_cell_global_id_t, rrc_cell_info>
rrc_du_impl::get_cell_info(const std::vector<cu_cp_du_served_cells_item>& served_cell_list) const
{
  std::map<nr_cell_global_id_t, rrc_cell_info> cell_info_map;
  for (const auto& served_cell : served_cell_list) {
    if (!served_cell.gnb_du_sys_info.has_value()) {
      logger.error("Missing gnb_du_sys_info for served cell");
      return {};
    }

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
    cell_info.nr_pci = served_cell.served_cell_info.nr_pci;

    asn1::cbit_ref                  bref_meas{served_cell.served_cell_info.meas_timing_cfg};
    asn1::rrc_nr::meas_timing_cfg_s asn1_meas_timing_cfg;
    if (asn1_meas_timing_cfg.unpack(bref_meas) != asn1::SRSASN_SUCCESS) {
      logger.error("Failed to unpack Measurement Timing Config container");
      return {};
    }
    if (asn1_meas_timing_cfg.crit_exts.type() != meas_timing_cfg_s::crit_exts_c_::types_opts::c1 ||
        asn1_meas_timing_cfg.crit_exts.c1().type() !=
            meas_timing_cfg_s::crit_exts_c_::c1_c_::types_opts::meas_timing_conf ||
        asn1_meas_timing_cfg.crit_exts.c1().meas_timing_conf().meas_timing.size() == 0) {
      logger.error("Invalid Measurement Timing Config container");
      return {};
    }

    for (const auto& asn1_meas_timing : asn1_meas_timing_cfg.crit_exts.c1().meas_timing_conf().meas_timing) {
      cell_info.meas_timings.push_back(asn1_to_meas_timing(asn1_meas_timing));
    }

    if (!served_cell.gnb_du_sys_info.has_value()) {
      // Note that "For NG-RAN, the gNB-DU shall include the gNB-DU System Information IE and the TAI Slice Support List
      // IE in the F1 SETUP REQUEST message.", see TS 38.473 sectuion 8.2.3.2.
      logger.error("Missing gnb_du_sys_info for served cell");
      return {};
    }

    // Unpack SIB1 to store timers.
    asn1::rrc_nr::sib1_s sib1_msg;
    asn1::cbit_ref       bref2(served_cell.gnb_du_sys_info.value().sib1_msg);
    if (sib1_msg.unpack(bref2) != asn1::SRSASN_SUCCESS) {
      logger.error("Failed to unpack SIB1");
      return {};
    }
    cell_info.timers.t300 = std::chrono::milliseconds{sib1_msg.ue_timers_and_consts.t300.to_number()};
    cell_info.timers.t301 = std::chrono::milliseconds{sib1_msg.ue_timers_and_consts.t301.to_number()};
    cell_info.timers.t310 = std::chrono::milliseconds{sib1_msg.ue_timers_and_consts.t310.to_number()};
    cell_info.timers.t311 = std::chrono::milliseconds{sib1_msg.ue_timers_and_consts.t311.to_number()};

    // Store selectedPLMN-Identities. Iterate over all PLMN identities in SIB1 and store them in the cell info.
    // TS 38.331 section 6.3.2:
    // plmn-IdentityInfoList
    // The plmn-IdentityInfoList is used to configure a set of PLMN-IdentityInfo elements. Each of those elements
    // contains a list of one or more PLMN Identities and additional information associated with those PLMNs. A
    // PLMN-identity can be included only once, and in only one entry of the PLMN-IdentityInfoList. The PLMN index is
    // defined as b1+b2+…+b(n-1)+i for the PLMN included at the n-th entry of PLMN-IdentityInfoList and the i-th entry
    // of its corresponding PLMN-IdentityInfo, where b(j) is the number of PLMN-Identity entries in each
    // PLMN-IdentityInfo, respectively.
    for (const auto& plmn_id_info : sib1_msg.cell_access_related_info.plmn_id_info_list) {
      for (const auto& plmn_info : plmn_id_info.plmn_id_list) {
        if (plmn_info.mcc_present) {
          expected<mobile_country_code> mcc = mobile_country_code::from_bytes(plmn_info.mcc);
          if (!mcc.has_value()) {
            logger.error("Invalid MCC in SIB1 RRC container");
            return {};
          }
          expected<mobile_network_code> mnc = mobile_network_code::from_bytes(plmn_info.mnc);
          if (!mnc.has_value()) {
            logger.error("Invalid MNC in SIB1 RRC container");
            return {};
          }
          logger.debug("Found PLMN identity {} in SIB1 RRC container", plmn_identity(mcc.value(), mnc.value()));
          cell_info.plmn_identity_list.emplace_back(mcc.value(), mnc.value());
        }
      }
    }

    if (cell_info.plmn_identity_list.empty()) {
      logger.error("No PLMN identities found in SIB1 RRC container");
      return {};
    }

    if (cell_info.plmn_identity_list.size() > 12U) {
      logger.error("Too many PLMN identities found in SIB1 RRC container ({}>12)", cell_info.plmn_identity_list.size());
      return {};
    }

    cell_info_map.emplace(served_cell.served_cell_info.nr_cgi, cell_info);
  }

  return cell_info_map;
}

void rrc_du_impl::store_cell_info_db(const std::map<nr_cell_global_id_t, rrc_cell_info>& cell_infos)
{
  for (const auto& [cgi, cell_info] : cell_infos) {
    cell_info_db.emplace(cgi.nci, cell_info);
  }
}

byte_buffer rrc_du_impl::get_rrc_reject()
{
  // Pack RRC Reconfig.
  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1().set_rrc_reject().crit_exts.set_rrc_reject();
  return pack_into_pdu(dl_ccch_msg, "RRCReject");
}

rrc_ue_interface* rrc_du_impl::add_ue(const rrc_ue_creation_message& msg)
{
  // If the DU to CU container is missing, assume the DU can't serve the UE, so the CU-CP should reject the UE, see
  // TS 38.473 section 8.4.1.2.
  if (!msg.du_to_cu_container.empty()) {
    // Unpack DU to CU container.
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
  }

  // Create UE object.
  ue_index_t   ue_index                 = msg.ue_index;
  rrc_ue_cfg_t ue_cfg                   = {};
  ue_cfg.force_reestablishment_fallback = cfg.force_reestablishment_fallback;
  ue_cfg.rrc_procedure_guard_time_ms    = cfg.rrc_procedure_guard_time_ms;
  ue_cfg.meas_timings                   = cell_info_db.at(msg.cell.cgi.nci).meas_timings;

  // Copy RRC cell and add SSB ARFCN.
  rrc_cell_context rrc_cell   = msg.cell;
  rrc_cell.ssb_arfcn          = ue_cfg.meas_timings.front().freq_and_timing.value().carrier_freq;
  rrc_cell.timers             = cell_info_db.at(msg.cell.cgi.nci).timers;
  rrc_cell.plmn_identity_list = cell_info_db.at(msg.cell.cgi.nci).plmn_identity_list;

  // Add RRC UE to RRC DU adapter.
  rrc_ue_rrc_du_adapters.emplace(ue_index, rrc_ue_rrc_du_adapter{get_rrc_du_connection_event_handler()});

  auto res = ue_db.emplace(ue_index,
                           std::make_unique<rrc_ue_impl>(*msg.f1ap_pdu_notifier,
                                                         *msg.ngap_notifier,
                                                         *msg.rrc_ue_cu_cp_notifier,
                                                         *msg.measurement_notifier,
                                                         *msg.cu_cp_ue_notifier,
                                                         rrc_ue_rrc_du_adapters.at(ue_index),
                                                         msg.ue_index,
                                                         msg.c_rnti,
                                                         rrc_cell,
                                                         ue_cfg,
                                                         msg.du_to_cu_container.copy(),
                                                         msg.rrc_context));

  if (res.second) {
    auto& u = ue_db.at(ue_index);
    return u.get();
  }
  return nullptr;
}

void rrc_du_impl::remove_ue(ue_index_t ue_index)
{
  auto ue_it = ue_db.find(ue_index);
  if (ue_it == ue_db.end()) {
    logger.warning("ue={}: Can't remove UE. Cause: UE not found", ue_index);
    return;
  }

  // Delete RRC UE to RRC DU adapter.
  rrc_ue_rrc_du_adapters.erase(ue_index);

  // Notify metrics.
  metrics_aggregator.aggregate_successful_rrc_release();

  // Delete RRC UE.
  ue_db.erase(ue_it);
}

void rrc_du_impl::handle_successful_rrc_setup(std::optional<establishment_cause_t> cause)
{
  if (cause.has_value()) {
    metrics_aggregator.aggregate_successful_connection_establishment(cause.value());
    return;
  }
  metrics_aggregator.aggregate_successful_rrc_setup();
}

void rrc_du_impl::handle_successful_rrc_release()
{
  metrics_aggregator.aggregate_successful_rrc_release();
}

void rrc_du_impl::handle_attempted_rrc_setup(establishment_cause_t cause)
{
  metrics_aggregator.aggregate_attempted_connection_establishment(cause);
}

void rrc_du_impl::handle_attempted_rrc_reestablishment()
{
  metrics_aggregator.aggregate_attempted_connection_reestablishment();
}

void rrc_du_impl::handle_successful_rrc_reestablishment()
{
  metrics_aggregator.aggregate_successful_connection_reestablishment(true);
}

void rrc_du_impl::handle_successful_rrc_reestablishment_fallback()
{
  metrics_aggregator.aggregate_successful_connection_reestablishment(false);
}

void rrc_du_impl::release_ues()
{
  // TODO: release all UEs connected to this RRC entity.
}
